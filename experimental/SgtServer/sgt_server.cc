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
    void handleDelete(http_request message);

    void handleGetNetwork(http_request message);

    http_listener listener_;   

    boost::filesystem::path dataDir_;
    map<string, unique_ptr<Network>> nws_;
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

    sgtAssert(argc == 2, "Usage: test_cpp_rest data_dir");
    string dataDir(argv[1]);
    string port = "34568";

    string address = "http://localhost:";
    address.append(port);

    on_initialize(address, dataDir);
    cout << "Press ENTER to exit." << endl;

    string line;
    getline(cin, line);

    on_shutdown();
    return 0;
}

SgtServer::SgtServer(const string& url, const string& dataDir) : 
    listener_(url), dataDir_(dataDir.c_str())
{
    listener_.support(methods::GET, bind(&SgtServer::handleGet, this, placeholders::_1));
    listener_.support(methods::PUT, bind(&SgtServer::handlePut, this, placeholders::_1));
    // listener_.support(methods::POST, bind(&SgtServer::handlePost, this, placeholders::_1));
    listener_.support(methods::DEL, bind(&SgtServer::handleDelete, this, placeholders::_1));
}

void SgtServer::handleGet(http_request message)
{
    cout << "GET received." << endl;

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    deque<decltype(pathsVec)::value_type> paths(pathsVec.begin(), pathsVec.end());
    Json reply;
    auto status = status_codes::OK;

    Network* nw;
    Bus* bus;
    BranchAbc* branch;
    GenAbc* gen;
    ZipAbc* zip;

    auto getBranchProperty = [&]()
    {
        // ... a specific property.
        string propName = paths.front();
        paths.pop_front();

        try
        {
            auto& prop = branch->properties()[propName];
            if (prop.isGettable())
            {
                reply = {{propName, prop.json(*branch)}};
            }
        }
        catch (out_of_range e)
        {
            status = status_codes::NotFound;
        }
    };

    auto getBranch = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the branch.
            reply = componentJson(*branch);
        }
        else
        {
            // We're interested in the branch's properties...
            // ... a specific property.
            getBranchProperty();
        }
    };

    auto getBranches = [&]()
    {
        if (paths.size() == 0)
        {
            // All the branches.
            reply = componentsJson(nw->branches());
        }
        else
        {
            // A single branch.
            string branchId = paths.front();
            paths.pop_front();

            branch = nw->branches()[branchId];
            if (branch == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                getBranch();
            }
        }
    };

    auto getBusProperty = [&]()
    {
        // ... a specific property.
        string propName = paths.front();
        paths.pop_front();

        try
        {
            auto& prop = bus->properties()[propName];
            if (prop.isGettable())
            {
                reply = {{propName, prop.json(*bus)}};
            }
        }
        catch (out_of_range e)
        {
            status = status_codes::NotFound;
        }
    };

    auto getBus = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the bus.
            reply = componentJson(*bus);
        }
        else
        {
            // We're interested in the bus's properties...
            // ... a specific property.
            getBusProperty();
        }
    };

    auto getBuses = [&]()
    {
        if (paths.size() == 0)
        {
            // All the buses.
            reply = componentsJson(nw->buses());
        }
        else
        {
            // A single bus.
            string busId = paths.front();
            paths.pop_front();

            bus = nw->buses()[busId];
            if (bus == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                getBus();
            }
        }
    };

    auto getGenProperty = [&]()
    {
        // ... a specific property.
        string propName = paths.front();
        paths.pop_front();

        try
        {
            auto& prop = gen->properties()[propName];
            if (prop.isGettable())
            {
                reply = {{propName, prop.json(*gen)}};
            }
        }
        catch (out_of_range e)
        {
            status = status_codes::NotFound;
        }
    };

    auto getGen = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the gen.
            reply = componentJson(*gen);
        }
        else
        {
            // We're interested in the gen's properties...
            // ... a specific property.
            getGenProperty();
        }
    };

    auto getGens = [&]()
    {
        if (paths.size() == 0)
        {
            // All the gens.
            reply = componentsJson(nw->gens());
        }
        else
        {
            // A single gen.
            string genId = paths.front();
            paths.pop_front();

            gen = nw->gens()[genId];
            if (gen == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                getGen();
            }
        }
    };

    auto getZipProperty = [&]()
    {
        // ... a specific property.
        string propName = paths.front();
        paths.pop_front();

        try
        {
            auto& prop = gen->properties()[propName];
            if (prop.isGettable())
            {
                reply = {{propName, prop.json(*zip)}};
            }
        }
        catch (out_of_range e)
        {
            status = status_codes::NotFound;
        }
    };

    auto getZip = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the zip.
            reply = componentJson(*zip);
        }
        else
        {
            // We're interested in the zip's properties...
            // ... a specific property.
            getZipProperty();
        }
    };

    auto getZips = [&]()
    {
        if (paths.size() == 0)
        {
            // All the zips.
            reply = componentsJson(nw->zips());
        }
        else
        {
            // A single zip.
            string zipId = paths.front();
            paths.pop_front();

            zip = nw->zips()[zipId];
            if (zip == nullptr)
            {
                status = status_codes::NotFound;
            }
            else
            {
                getZip();
            }
        }
    };

    auto getNetwork = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the network.
            reply = {
                {"branches", componentsJson(nw->branches())},
                {"buses", componentsJson(nw->buses())},
                {"gens", componentsJson(nw->gens())},
                {"zips", componentsJson(nw->zips())}};
        }
        else
        {
            string nwSub = paths.front();
            paths.pop_front();

            if (nwSub == "branches")
            {
                getBranches();
            }
            else if (nwSub == "gens")
            {
                getGens();
            }
            else if (nwSub == "buses")
            {
                getBuses();
            }
            else if (nwSub == "zips")
            {
                getZips();
            }
        }
    };

    auto getNetworks = [&]()
    {
        if (paths.size() == 0)
        {
            status = status_codes::BadRequest;
        }
        else
        {
            string nwId = paths.front();
            paths.pop_front();

            try
            {
                nw = nws_.at(nwId).get();
                getNetwork();
            }
            catch (out_of_range)
            {
                status = status_codes::NotFound;
            }
        }
    };
       
    auto getYamlNetworkFiles = [&]()
    {
        using namespace boost::filesystem;
        if (paths.size() != 0)
        {
            message.reply(status_codes::BadRequest);
        }
        else
        {
            directory_iterator it(dataDir_);
            vector<string> files;
            vector<string> mFiles;
            transform(it, directory_iterator(), back_inserter(files),
                    [](decltype(*it)& entry){return entry.path().filename().string();});
            copy_if(files.begin(), files.end(), back_inserter(mFiles), 
                    [](const string& s){return regex_search(s, regex(".*\\.yaml$"));});
            reply = Json(mFiles); 
        }
    };
       
    auto getMatpowerNetworkFiles = [&]()
    {
        using namespace boost::filesystem;
        if (paths.size() != 0)
        {
            message.reply(status_codes::BadRequest);
        }
        else
        {
            directory_iterator it(dataDir_);
            vector<string> files;
            vector<string> mFiles;
            transform(it, directory_iterator(), back_inserter(files),
                    [](decltype(*it)& entry){return entry.path().filename().string();});
            copy_if(files.begin(), files.end(), back_inserter(mFiles), 
                    [](const string& s){return regex_search(s, regex(".*\\.m$"));});
            reply = Json(mFiles); 
        }
    };
       
    if (paths.size() == 0)
    {
        status = status_codes::BadRequest;
    }
    else
    {
        const string& pathsFront = paths.front();
        paths.pop_front();

        if (pathsFront == "networks")
        {
            getNetworks();
        }
        else if (pathsFront == "yaml_network_files")
        {
            getYamlNetworkFiles();
        }
        else if (pathsFront == "matpower_network_files")
        {
            getMatpowerNetworkFiles();
        }
        else
        {
            status = status_codes::BadRequest;
        }
    }

    message.reply(status, reply.dump(2));
}

void SgtServer::handlePut(http_request message)
{
    cout << "PUT received." << endl;

    Json messageContentJson = Json::parse(message.extract_string(true).get());

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    deque<decltype(pathsVec)::value_type> paths(pathsVec.begin(), pathsVec.end());

    if (paths.size() == 0)
    {
        message.reply(status_codes::BadRequest);
    }
    else
    {
        string first = paths.front();
        paths.pop_front();
        Network* nw = new Network();
        if (first == "yaml_networks")
        {
            string networkId = pathsVec[1];
            string fname = messageContentJson["yaml_filename"];
            Parser<Network> p;
            p.parse(dataDir_.string() + "/" + fname, *nw);
            nws_[networkId].reset(nw);
        }
        else if (first == "matpower_networks")
        {
            string networkId = pathsVec[1];
            string fname = messageContentJson["matpower_filename"];
            string yamlStr = string("--- [{matpower : {input_file : ") 
                + dataDir_.string() + "/" + fname + ", default_kV_base : 11}}]";
            YAML::Node n = YAML::Load(yamlStr);
            Parser<Network> p;
            p.parse(n, *nw);
            nws_[networkId].reset(nw);
        }
        else
        {
            message.reply(status_codes::BadRequest);
        }

        try 
        {
            nw->solvePowerFlow();
        }
        catch (exception& e)
        {
            cerr << "Something went wrong with solving the network." << endl;
            cerr << e.what() << endl;
            // throw(e);
        }
    }
    message.reply(status_codes::OK);
}

void SgtServer::handleDelete(http_request message)
{
    cout << "DELETE received." << endl;

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (pathsVec.size() != 2)
    {
        message.reply(status_codes::BadRequest);
    }
    else
    {
        string first = pathsVec[0];
        if (first != "networks")
        {
            message.reply(status_codes::BadRequest);
        }
        else
        {
            string nwId = pathsVec[1];
            auto n = nws_.erase(nwId);
            if (n == 0)
            {
                message.reply(status_codes::NotFound);
            }
            else
            {
                message.reply(status_codes::OK);
            }
        }
    }
}
