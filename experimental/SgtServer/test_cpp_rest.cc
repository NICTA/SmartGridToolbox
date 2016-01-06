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

#include "Common.h"
#include "Network.h"
#include "Parser.h"

#include "json.hpp"

#include <armadillo>

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <random>

# include <sys/time.h>

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

#include <complex>
#include <iostream>
#include <map>
#include <vector>

using namespace Sgt;

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

class Server
{

public:

    Server(const std::string& fName, const std::string& url);

    pplx::task<void> open() {return listener_.open();}
    pplx::task<void> close() {return listener_.close();}

private:

    void handle_get(http_request message);
    // void handle_put(http_request message);
    // void handle_post(http_request message);
    // void handle_delete(http_request message);

    http_listener listener_;   
    
    Network nw_{100.0};
};

static std::unique_ptr<Server> gServer;

void on_initialize(const std::string& fname, const std::string& address)
{
    uri_builder uri(address);
    uri.append_path("network");

    auto addr = uri.to_uri().to_string();
    gServer = std::unique_ptr<Server>(new Server(fname, addr));
    gServer->open().wait();
    
    std::cout << "Listening for requests at: " << addr << std::endl;
}

void on_shutdown()
{
    gServer->close().wait();
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

Server::Server(const std::string& fname, const std::string& url) : listener_(url)
{
    std::string yamlStr = std::string("--- [{matpower : {input_file : ") + fname + ", default_kV_base : 11}}]";
    YAML::Node n = YAML::Load(yamlStr);
    Parser<Network> p;
    p.parse(n, nw_);
    std::cout << toJson(nw_).dump(2) << std::endl;

    listener_.support(methods::GET, std::bind(&Server::handle_get, this, std::placeholders::_1));
    // listener_.support(methods::PUT, std::bind(&Server::handle_put, this, std::placeholders::_1));
    // listener_.support(methods::POST, std::bind(&Server::handle_post, this, std::placeholders::_1));
    // listener_.support(methods::DEL, std::bind(&Server::handle_delete, this, std::placeholders::_1));
}

void Server::handle_get(http_request message)
{
    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    for (auto path : paths)
    {
        std::cout << path << std::endl;
    }
    nlohmann::json responseJson;
    auto status = status_codes::OK;
    if (paths.size() >= 1)
    {
        if (paths[0] == "busses")
        {
            if (paths.size() == 1)
            {
                responseJson.push_back(toJson(nw_.busses()));
            }
            else if (paths.size() == 2)
            {
                auto bus = nw_.bus(paths[1]);
                if (bus != nullptr)
                {
                    responseJson.push_back(toJson(*bus));
                }
                else
                {
                    status = status_codes::NotFound;
                }
            }
            else
            {
                status = status_codes::BadRequest;
            }
        }
        else if (paths[0] == "branches")
        {
            responseJson.push_back(toJson(nw_.branches()));
        }
    }
    message.reply(status, responseJson.dump(2));
};
