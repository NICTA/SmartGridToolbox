#define _TURN_OFF_PLATFORM_STRING

#include <cpprest/json.h>
#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include <cpprest/asyncrt_utils.h>

#include <SgtCore.h>

using namespace Sgt;

using namespace web;
using namespace http;
using namespace http::experimental::listener;
using namespace json;
using namespace utility;

class SgtServer
{
public:
    SgtServer() {}
    SgtServer(const std::string& url);

    pplx::task<void> open() { return listener_.open(); }
    pplx::task<void> close() { return listener_.close(); }

private:

    void handle_get(http_request message);

    http_listener listener_;   
};

SgtServer::SgtServer(const std::string& url) : listener_(url)
{
    listener_.support(methods::GET, std::bind(&SgtServer::handle_get, this, std::placeholders::_1));
}

void SgtServer::handle_get(http_request message)
{
    std::cout <<  message.to_string() << std::endl;

    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    std::cout << "paths size = " << paths.size() << std::endl;
    if (paths.empty())
    {
        auto obj = value::object();
        obj["four"] = 4;
        message.reply(status_codes::OK, obj);
    }
    else
    {
        std::vector<std::pair<std::string, value>> map;
        std::vector<value> vec;
        for (auto path : paths)
        {
            vec.push_back(value(path));
        }
        map.push_back(std::make_pair("paths", value::array(vec)));
        message.reply(status_codes::OK, value::object(map));
    }
    return;
};

std::unique_ptr<SgtServer> gSgtServer;

void on_initialize(const std::string& address)
{
    gSgtServer = std::unique_ptr<SgtServer>(new SgtServer(address));
    gSgtServer->open().wait();
    std::cout << "Listening for requests at: " << address << std::endl;
}

void on_shutdown()
{
    gSgtServer->close().wait();
}

int main(int argc, char *argv[])
{
    std::string address = "http://localhost:8000";

    on_initialize(address);
    std::cout << "Press ENTER to exit." << std::endl;
    std::string line;
    std::getline(std::cin, line);
    on_shutdown();
    return 0;
}
