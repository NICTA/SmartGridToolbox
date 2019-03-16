// Copyright 2015-2016 National ICT Australia Limited (NICTA)
// Copyright 2016-2019 The Australian National University
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
        
    void addProperties()
    {

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [](const Bus& bus){return abs(bus.V());}, "VMag_kV");

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [](const Bus& bus){return abs(bus.V())/bus.VBase();}, "VMagPu");

        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(bus.V());}, "VRms_kV");

        Bus::sProperties().addGetProperty<Bus, double>(
                [&](const Bus& bus){return rms(bus.V())/bus.VBase();}, "VRmsPu");

        Bus::sProperties().addGetProperty<Bus, Col<double>>(
                [&](const Bus& bus)->Col<double>{return argDeg(bus.V());}, "VAngDeg");

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
    SgtServer(const string& url, const string& dataDir);

    pplx::task<void> open() {return listener_.open();}
    pplx::task<void> close() {return listener_.close();}

private:
    void handleGet(http_request message);
    void handlePut(http_request message);
    // void handlePost(http_request message);
    // void handleDelete(http_request message);

    Json getInputFiles(const std::string& suffix);

private:
    http_listener listener_;   
    unique_ptr<Network> netw_;
    boost::filesystem::path dataDir_;
};

static unique_ptr<SgtServer> gServer;

void on_initialize(const string& address, const string& dataDir)
{
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    gServer = unique_ptr<SgtServer>(new SgtServer(addr, dataDir));
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
    string dataDir(argv[1]);

    address.append(port);

    on_initialize(address, dataDir);
    cout << "Press ENTER to exit." << endl;

    string line;
    getline(cin, line);

    on_shutdown();
    return 0;
}

SgtServer::SgtServer(const string& url, const string& dataDir) : 
    listener_(url),
    dataDir_(dataDir.c_str())
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
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                reply = {
                    {"branches", componentsJson(netw_->branches())},
                    {"buses", componentsJson(netw_->buses())},
                    {"gens", componentsJson(netw_->gens())},
                    {"zips", componentsJson(netw_->zips())}};
            }
        }
        else if (view == "bus_voltages")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                for (auto b : netw_->buses())
                {
                    reply.push_back(Json(rms(b->V()) / b->VBase()));
                }
            }
        }
        else if (view == "zip_powers")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                auto& p = GenericZip::sProperties()["PTot"];
                for (auto z : netw_->zips())
                {
                    reply.push_back(p.json(*z));
                }
            }
        }
        else if (view == "gen_powers")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                auto& p = GenericGen::sProperties()["PTot"];
                for (auto g : netw_->gens())
                {
                    reply.push_back(p.json(*g));
                }
            }
        }
        else if (view == "branch")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                std::string id = query.at("id"); 
                reply = componentJson(*netw_->branches()[id]);
            }
        }
        else if (view == "bus")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                std::string id = query.at("id"); 
                reply = componentJson(*netw_->buses()[id]);
            }
        }
        else if (view == "gen")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                std::string id = query.at("id"); 
                reply = componentJson(*netw_->gens()[id]);
            }
        }
        else if (view == "zip")
        {
            if (netw_ == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                std::string id = query.at("id"); 
                reply = componentJson(*netw_->zips()[id]);
            }
        }
        else if (view == "input_files")
        {
            std::string suffix = query.at("suffix"); 
            reply = getInputFiles(suffix);
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
        if (action == "load_yaml")
        {
            std::string fName = query.at("file");
            netw_.reset(new Network);
            NetworkParser p;
            p.parse(dataDir_.string() + "/" + fName, *netw_);
        }
        else if (action == "load_matpower")
        {
            std::string fName = query.at("file");
            string yamlStr = string("--- [{matpower : {input_file : ") 
                + dataDir_.string() + "/" + fName + ", default_kV_base : 11}}]";
            YAML::Node n = YAML::Load(yamlStr);
            netw_.reset(new Network);
            NetworkParser p;
            p.parse(n, *netw_);
        }
    }
    catch (out_of_range e)
    {
        status = status_codes::BadRequest;
    }
    message.reply(status, reply.dump(2));
}

Json SgtServer::getInputFiles(const std::string& suffix)
{
    using namespace boost::filesystem;
    directory_iterator it(dataDir_);
    vector<string> files;
    vector<string> mFiles;
    transform(it, directory_iterator(), back_inserter(files),
            [](decltype(*it)& entry){return entry.path().filename().string();});
    copy_if(files.begin(), files.end(), back_inserter(mFiles), 
            [&suffix](const string& s){return regex_search(s, regex(".*\\." + suffix + "$"));});
    return Json(mFiles); 
}
