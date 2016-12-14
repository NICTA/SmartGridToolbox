// Copyright 2015 National ICT Australia Limited (NICTA)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <SgtCore/Common.h>
#include <SgtCore/json.h>
#include <SgtCore/Network.h>
#include <SgtCore/NetworkParser.h>
#include <SgtCore/Parser.h>

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

#include <boost/filesystem.hpp>

#include <map>
#include <regex>

using namespace arma;
using namespace Sgt;
using namespace std;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

using Json = Sgt::json;

namespace
{
    Col<double> argDeg(const Col<Complex>& x)
    {
        Col<double> result(x.size(), fill::none);
        transform(x.begin(), x.end(), result.begin(), [](const Complex& y)->double{return arg(y);});
        return result * 180 / pi;
    }
        
    Col<double> VMag_kV_Sincal(const Bus& bus)
    {
        Col<double> result;
        Json j;
        if (j = bus.userData()["LFNodeResult"], !j.is_null())
        {
            result = Col<double>{j["U"]};
        }
        else if (j = bus.userData()["ULFNodeResult"], !j.is_null())
        {
            map<Phase, double> m{{Phase::A, j["U1"]}, {Phase::B, j["U2"]}, {Phase::C, j["U3"]}};
            result = Col<double>(bus.phases().size(), fill::none);
            for (size_t i = 0; i < bus.phases().size(); ++i) 
            {
                result(i) = m.at(bus.phases()[i]); 
            }
        }
        return result;
    }

    Col<double> VMagPuSincal(const Bus& bus)
    {
        Col<double> result;
        Json j;
        if (j = bus.userData()["LFNodeResult"], !j.is_null())
        {
            result = 0.01 * Col<double>{j["U_Un"]};
        }
        else if (j = bus.userData()["ULFNodeResult"], !j.is_null())
        {
            map<Phase, double> m{
                {Phase::A, 0.01 * j["U1_Un"].get<double>()},
                    {Phase::B, 0.01 * j["U2_Un"].get<double>()},
                    {Phase::C, 0.01 * j["U3_Un"].get<double>()}};
            result = Col<double>(bus.phases().size(), fill::none);
            for (size_t i = 0; i < bus.phases().size(); ++i) 
            {
                result(i) = m.at(bus.phases()[i]); 
            }
        }
        return result;
    }

    Col<double> VAngDegSincal(const Bus& bus)
    {
        Col<double> result;
        Json j;
        if (j = bus.userData()["LFNodeResult"], !j.is_null())
        {
            result = Col<double>{j["phi"]};
        }
        else if (j = bus.userData()["ULFNodeResult"], !j.is_null())
        {
            map<Phase, double> m{{Phase::A, j["phi1"]}, {Phase::B, j["phi2"]}, {Phase::C, j["phi3"]}};
            result = Col<double>(bus.phases().size(), fill::none);
            for (size_t i = 0; i < bus.phases().size(); ++i) 
            {
                result(i) = m.at(bus.phases()[i]); 
            }
        }
        return result;
    }

    Col<Complex> VSincal(const Bus& bus)
    {
        Col<double> VAngRad = (pi / 180.0) * VAngDegSincal(bus);
        return VMag_kV_Sincal(bus) % Col<Complex>(cx_mat(cos(VAngRad), sin(VAngRad)));
    }

    void addProperties()
    {

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [](const Bus& bus){return abs(bus.V());}, "VMag_kV");
        Bus::sProperties().addGetProperty<Bus, Col<double>>(VMag_kV_Sincal, "VMag_kV_Sincal");

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [](const Bus& bus){return abs(bus.V())/bus.VBase();}, "VMagPu");
        Bus::sProperties().addGetProperty<Bus, Col<double>>(VMagPuSincal, "VMagPuSincal");

        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(bus.V());}, "VRms_kV");
        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(VMag_kV_Sincal(bus));}, "VRms_kV_Sincal");

        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(bus.V())/bus.VBase();}, "VRmsPu");
        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(VMagPuSincal(bus));}, "VRmsPuSincal");

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [&](const Bus& bus)->Col<double>{return argDeg(bus.V());}, "VAngDeg");
        Bus::sProperties().addGetProperty<Bus, Col<double>>(VAngDegSincal, "VAngDegSincal");

        Bus::sProperties().addGetProperty<Bus, Col<Complex>>(VSincal, "VSincal");

        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus)->double{return rms(bus.V())/bus.VBase() - rms(VMagPuSincal(bus));}, "VError");
        
        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return real(bus.SZipTot() - bus.SGenTot());}, "PTot");
        
        GenericZip::sProperties().addGetProperty<GenericZip, double>(
                [&](const GenericZip& zip){return accu(real(zip.SConst()));}, "PTot");
        
        GenericGen::sProperties().addGetProperty<GenericGen, double>(
                [&](const GenericGen& gen){return sum(real(gen.S()));}, "PTot");
    }

    template<typename T> Json componentPropsJson(const T& comp)
    {
        Json result;
        for (const auto& propPair : comp.properties())
        {
            result[propPair.first] = propPair.second->json(comp);
        }
        return result;
    }

    template<typename T> Json componentJson(const T& comp)
    {
        return componentPropsJson(comp);
    }

    template<> Json componentJson<BranchAbc>(const BranchAbc& comp)
    {
        Json result = componentPropsJson(comp);
        result["bus0"] = comp.bus0() != nullptr ? Json(comp.bus0()->id()) : Json();
        result["bus1"] = comp.bus1() != nullptr ? Json(comp.bus1()->id()) : Json();
        return result;
    }

    template<> Json componentJson<Bus>(const Bus& comp)
    {
        Json result = componentPropsJson(comp);

        auto branches0 = comp.branches0();
        auto branches1 = comp.branches1();
        auto gens = comp.gens();
        auto zips = comp.zips();

        auto getId = [](const Component* x){return x->id();};

        vector<string> branch0Ids;
        transform(branches0.begin(), branches0.end(), back_inserter(branch0Ids), getId);
        result["branches0"] = branch0Ids;

        vector<string> branch1Ids;
        transform(branches1.begin(), branches1.end(), back_inserter(branch1Ids), getId);
        result["branches1"] = branch1Ids;

        vector<string> genIds;
        transform(gens.begin(), gens.end(), back_inserter(genIds), getId); 
        result["gens"] = genIds;

        vector<string> zipIds;
        transform(zips.begin(), zips.end(), back_inserter(zipIds), getId); 
        result["zips"] = zipIds;

        return result;
    }

    template<> Json componentJson<GenAbc>(const GenAbc& comp)
    {
        Json result = componentPropsJson(comp);
        result["bus"] = comp.bus() != nullptr ? Json(comp.bus()->id()) : Json();
        return result;
    }

    template<> Json componentJson<ZipAbc>(const ZipAbc& comp)
    {
        Json result = componentPropsJson(comp);
        result["bus"] = comp.bus() != nullptr ? Json(comp.bus()->id()) : Json();
        return result;
    }

    template<typename T> Json componentsJson(const T& compVec)
    {
        Json result;
        for (auto comp : compVec)
        {
            result.push_back(componentJson(*comp));
        }
        return result;
    }
}

class SgtServer
{
public:
    SgtServer(const string& url);

    pplx::task<void> open() {return listener_.open();}
    pplx::task<void> close() {return listener_.close();}

private:
    void handleGet(http_request message);
    void handlePut(http_request message);
    // void handlePost(http_request message);
    // void handleDelete(http_request message);

    std::pair<status_code, Json> getNetwork(deque<std::string>& paths);

    template<typename Comps> std::pair<status_code, Json>
    getComps(const Comps& comps, deque<std::string>& paths);

    template<typename Comp>
    std::pair<status_code, Json> getComp(deque<std::string>& paths, const Comp& comp);
    
    template<typename Comp>
    std::pair<status_code, Json> getCompProperty(deque<std::string>& paths, const Comp& comp);

private:
    http_listener listener_;   
    Network netw_;
};

static unique_ptr<SgtServer> gServer;

void on_initialize(const string& address)
{
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    gServer = unique_ptr<SgtServer>(new SgtServer(addr));
    gServer->open().wait();
    
    cout << "Listening for requests at: " << addr << endl;
}

void on_shutdown()
{
    gServer->close().wait();
}

int main(int argc, char *argv[])
{

    string port = "34568";

    string address = "http://localhost:";
    address.append(port);

    on_initialize(address);
    cout << "Press ENTER to exit." << endl;

    string line;
    getline(cin, line);

    on_shutdown();
    return 0;
}

SgtServer::SgtServer(const string& url) : 
    listener_(url)
{
    addProperties();

    listener_.support(methods::GET, bind(&SgtServer::handleGet, this, placeholders::_1));
    listener_.support(methods::PUT, bind(&SgtServer::handlePut, this, placeholders::_1));
}

void SgtServer::handleGet(http_request message)
{
    cout << "GET received." << endl;
    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    auto query = web::uri::split_query(message.relative_uri().query());
    
    status_code status = status_codes::OK;
    Json reply;
    try
    {
        std::string view = query.at("view"); 
        if (view == "network")
        {
            reply = {
                {"branches", componentsJson(netw_.branches())},
                {"buses", componentsJson(netw_.buses())},
                {"gens", componentsJson(netw_.gens())},
                {"zips", componentsJson(netw_.zips())}};
        }
        else if (view == "bus_voltages")
        {
            for (auto b : netw_.buses())
            {
                reply.push_back(Json(rms(b->V()) / b->VBase()));
            }
        }
        else if (view == "zip_powers")
        {
            auto& p = GenericZip::sProperties()["PTot"];
            for (auto z : netw_.zips())
            {
                reply.push_back(p.json(*z));
            }
        }
        else if (view == "gen_powers")
        {
            auto& p = GenericGen::sProperties()["PTot"];
            for (auto g : netw_.gens())
            {
                reply.push_back(p.json(*g));
            }
        }
        else if (view == "branch")
        {
            std::string id = query.at("id"); 
            reply = componentJson(*netw_.branches()[id]);
        }
        else if (view == "bus")
        {
            std::string id = query.at("id"); 
            reply = componentJson(*netw_.buses()[id]);
        }
        else if (view == "gen")
        {
            std::string id = query.at("id"); 
            reply = componentJson(*netw_.gens()[id]);
        }
        else if (view == "zip")
        {
            std::string id = query.at("id"); 
            reply = componentJson(*netw_.zips()[id]);
        }
    }
    catch (out_of_range e)
    {
        status = status_codes::BadRequest;
    }
    message.reply(status, reply.dump(2));
}

void SgtServer::handlePut(http_request message)
{
    cout << "PUT received." << endl;
    auto query = web::uri::split_query(message.relative_uri().query());

    status_code status = status_codes::OK;
    Json reply;
    try
    {
        std::string action = query.at("action"); 
        if (action == "load")
        {
            NetworkParser p;
            std::string fName = query.at("yaml_file");
            p.parse(fName, netw_);
        }
    }
    catch (out_of_range e)
    {
        status = status_codes::BadRequest;
    }
    message.reply(status, reply.dump(2));
}
