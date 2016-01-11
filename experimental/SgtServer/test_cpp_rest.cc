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
#include <SgtCore/json.hpp>
#include <SgtCore/Network.h>
#include <SgtCore/Parser.h>

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

using namespace Sgt;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

using Json = Sgt::json;

class SgtServer
{
public:

    SgtServer(const std::string& fName, const std::string& url);

    pplx::task<void> open() {return listener_.open();}
    pplx::task<void> close() {return listener_.close();}

private:

    void handleGet(http_request message);
    void handlePut(http_request message);
    // void handlePost(http_request message);
    void handleDelete(http_request message);

    void handleGetNetwork(http_request message);

    http_listener listener_;   
    
    std::map<std::string, std::unique_ptr<Network>> nws_;
};

static std::unique_ptr<SgtServer> gSgtServer;

void on_initialize(const std::string& fname, const std::string& address)
{
    uri_builder uri(address);

    auto addr = uri.to_uri().to_string();
    gSgtServer = std::unique_ptr<SgtServer>(new SgtServer(fname, addr));
    gSgtServer->open().wait();
    
    std::cout << "Listening for requests at: " << addr << std::endl;
}

void on_shutdown()
{
    gSgtServer->close().wait();
}

int main(int argc, char *argv[])
{
    sgtAssert(argc == 2, "Usage: solve_network infile");
    std::string fname = argv[1];
    std::string port = "34568";

    std::string address = "http://localhost:";
    address.append(port);
    std::cout << "address = " << address << std::endl;

    on_initialize(fname, address);
    std::cout << "Press ENTER to exit." << std::endl;

    std::string line;
    std::getline(std::cin, line);

    on_shutdown();
    return 0;
}

SgtServer::SgtServer(const std::string& fname, const std::string& url) : listener_(url)
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

    auto getBusses = [&]()
    {
        if (paths.size() == 0)
        {
            reply = Json(nw->busses());
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
            else if (nwSub == "busses")
            {
                getBusses();
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
    std::cout << "message: " << messageContentJson << "." << std::endl;

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
            std::string networkId = pathsVec[1];
            std::string fname = messageContentJson["matpower_filename"];
            Network* nw = new Network();
            std::string yamlStr = std::string("--- [{matpower : {input_file : ") + fname + ", default_kV_base : 11}}]";
            YAML::Node n = YAML::Load(yamlStr);
            Parser<Network> p;
            p.parse(n, *nw);
            nws_[networkId].reset(nw);
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
