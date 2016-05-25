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

#include <regex>

using namespace Sgt;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

using Json = Sgt::json;

class SgtServer
{
public:

    SgtServer(const std::string& url, const std::string& dataDir);

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
    std::map<std::string, std::unique_ptr<Network>> nws_;
};

static std::unique_ptr<SgtServer> gServer;

void on_initialize(const std::string& address, const std::string& dataDir)
{
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    gServer = std::unique_ptr<SgtServer>(new SgtServer(addr, dataDir));
    gServer->open().wait();
    
    std::cout << "Listening for requests at: " << addr << std::endl;
}

void on_shutdown()
{
    gServer->close().wait();
}

int main(int argc, char *argv[])
{
    sgtAssert(argc == 2, "Usage: test_cpp_rest data_dir");
    std::string dataDir(argv[1]);
    std::string port = "34568";

    std::string address = "http://localhost:";
    address.append(port);

    on_initialize(address, dataDir);
    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    on_shutdown();
    return 0;
}

SgtServer::SgtServer(const std::string& url, const std::string& dataDir) : 
    listener_(url), dataDir_(dataDir.c_str())
{
    listener_.support(methods::GET, std::bind(&SgtServer::handleGet, this, std::placeholders::_1));
    listener_.support(methods::PUT, std::bind(&SgtServer::handlePut, this, std::placeholders::_1));
    // listener_.support(methods::POST, std::bind(&SgtServer::handlePost, this, std::placeholders::_1));
    listener_.support(methods::DEL, std::bind(&SgtServer::handleDelete, this, std::placeholders::_1));
}

void SgtServer::handleGet(http_request message)
{
    std::cout << "GET received." << std::endl;

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    std::deque<decltype(pathsVec)::value_type> paths(pathsVec.begin(), pathsVec.end());
    Json reply = {{"message_type", "GET"}};
    auto status = status_codes::OK;

    Network* nw;
    Bus* bus;

    auto getBusProperty = [&]()
    {
        // ... a specific property.
        std::string propName = paths.front();
        paths.pop_front();

        try
        {
            auto prop = bus->properties().at(propName);
            if (prop->isGettable())
            {
                reply = {{propName, prop->toJson(*bus)}};
            }
        }
        catch (std::out_of_range e)
        {
            status = status_codes::NotFound;
        }
    };

    auto getBus = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the bus.
            reply = Json(*bus);
        }
        else
        {
            std::string busSub = paths.front();
            paths.pop_front();

            if (busSub == "properties")
            {
                // We're interested in the bus's properties...
                if (paths.size() == 0)
                {
                    // ... all the properties.
                    for (auto p : bus->properties())
                    {
                        if (p.second->isGettable())
                        {
                            reply[p.first] = p.second->toJson(*bus);
                        }
                    }
                }
                else
                {
                    // ... a specific property.
                    getBusProperty();
                }
            }
        }
    };

    auto getBranches = [&]()
    {
        if (paths.size() == 0)
        {
            reply = Json(nw->branches());
        }
        else
        {
            status = status_codes::BadRequest;
        }
    };

    auto getGens = [&]()
    {
        if (paths.size() == 0)
        {
            reply = Json(nw->gens());
        }
        else
        {
            status = status_codes::BadRequest;
        }
    };

    auto getBuses = [&]()
    {
        if (paths.size() == 0)
        {
            reply = Json(nw->buses());
        }
        else
        {
            std::string busId = paths.front();
            paths.pop_front();

            bus = nw->bus(busId);
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

    auto getZips = [&]()
    {
        if (paths.size() == 0)
        {
            reply = Json(nw->zips());
        }
        else
        {
            status = status_codes::BadRequest;
        }
    };
    
    auto getNetwork = [&]()
    {
        if (paths.size() == 0)
        {
            // Just return the network.
            reply = Json(*nw);
        }
        else
        {
            std::string nwSub = paths.front();
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
            std::string nwId = paths.front();
            paths.pop_front();

            try
            {
                nw = nws_.at(nwId).get();
                getNetwork();
            }
            catch (std::out_of_range)
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
            std::vector<std::string> files;
            std::vector<std::string> mFiles;
            std::transform(it, directory_iterator(), std::back_inserter(files),
                    [](decltype(*it)& entry){return entry.path().filename().string();});
            std::copy_if(files.begin(), files.end(), std::back_inserter(mFiles), 
                    [](const std::string& s){return std::regex_search(s, std::regex(".*\\.yaml$"));});
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
            std::vector<std::string> files;
            std::vector<std::string> mFiles;
            std::transform(it, directory_iterator(), std::back_inserter(files),
                    [](decltype(*it)& entry){return entry.path().filename().string();});
            std::copy_if(files.begin(), files.end(), std::back_inserter(mFiles), 
                    [](const std::string& s){return std::regex_search(s, std::regex(".*\\.m$"));});
            reply = Json(mFiles); 
        }
    };

    if (paths.size() == 0)
    {
        status = status_codes::BadRequest;
    }
    else
    {
        const std::string& pathsFront = paths.front();
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
    std::cout << "PUT received." << std::endl;

    Json messageContentJson = Json::parse(message.extract_string(true).get());

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    std::deque<decltype(pathsVec)::value_type> paths(pathsVec.begin(), pathsVec.end());

    if (paths.size() == 0)
    {
        message.reply(status_codes::BadRequest);
    }
    else
    {
        std::string first = paths.front();
        paths.pop_front();
        if (first == "yaml_networks")
        {
            std::string networkId = pathsVec[1];
            std::string fname = messageContentJson["yaml_filename"];
            Network* nw = new Network();
            std::string yamlStr = std::string("--- [{matpower : {input_file : ") 
                + dataDir_.string() + fname + ", default_kV_base : 11}}]";
            std::cout << yamlStr << std::endl;
            YAML::Node n = YAML::Load(yamlStr);
            Parser<Network> p;
            p.parse(dataDir_.string() + fname, *nw);
            nws_[networkId].reset(nw);
        }
        else if (first == "matpower_networks")
        {
            std::string networkId = pathsVec[1];
            std::string fname = messageContentJson["matpower_filename"];
            Network* nw = new Network();
            std::string yamlStr = std::string("--- [{matpower : {input_file : ") 
                + dataDir_.string() + fname + ", default_kV_base : 11}}]";
            std::cout << yamlStr << std::endl;
            YAML::Node n = YAML::Load(yamlStr);
            Parser<Network> p;
            p.parse(n, *nw);
            nws_[networkId].reset(nw);
        }
        else
        {
            message.reply(status_codes::BadRequest);
        }
    }
    message.reply(status_codes::OK);
}

void SgtServer::handleDelete(http_request message)
{
    std::cout << "DELETE received." << std::endl;

    auto pathsVec = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (pathsVec.size() != 2)
    {
        message.reply(status_codes::BadRequest);
    }
    else
    {
        std::string first = pathsVec[0];
        if (first != "networks")
        {
            message.reply(status_codes::BadRequest);
        }
        else
        {
            std::string nwId = pathsVec[1];
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
