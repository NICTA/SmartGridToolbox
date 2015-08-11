#ifndef SGT_SERVER_DOT_H
#define SGT_SERVER_DOT_H

#include "server_http.hpp"

#include <SgtSim/Simulation.h>

#include <memory>
#include <thread>

namespace Sgt
{
    struct SimInstance
    {
        Sgt::Simulation sim_;
        std::mutex mutex_;
    };

    class SgtServer
    {
        public:

            SgtServer(size_t nThreads = 4) : server_(std::unique_ptr<HttpServer>(new HttpServer(8080, nThreads)))
            {
                server_->resource["^/simulations/([a-zA-Z_][a-zA-Z0-9_]*)"]["GET"] = 
                    [this](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) 
                    {
                        std::string simId = request->path_match[1];
                        try
                        {
                            auto& sim = simulations_.at(simId);
                            std::stringstream ss;
                            request->content >> ss.rdbuf();
                            string content=ss.str();
                            response
                                << "HTTP/1.1 200 OK" << "\r\n"
                                << "Content-Length: " << content.length() << "\r\n\r\n"
                                << content;
                        }
                        catch (std::out_of_range)
                        {
                            std::string content("Error 404: Not Found. Simulation not found.");
                            response
                                << "HTTP/1.1 404 Not Found" << "\r\n"
                                << "Content-Length: " << content.length() << "\r\n\r\n"
                                << content;
                        }
                    };
                server_->resource["^/simulations/([a-zA-Z_][a-zA-Z0-9_]*)"]["PUT"] = 
                    [this](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) 
                    {
                        std::string simId = request->path_match[1];
                        auto& sim = simulations_[simId];
                        if (sim == nullptr)
                        {
                            sim.reset(new SimInstance);
                        }
                        response << "HTTP/1.1 200 OK\r\nContent-Length: 3" << "\r\n\r\n" << "aaa";
                    };
            }

            void run()
            {
                server_->start();
            }

        private:

            typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

        private:

            std::unique_ptr<HttpServer> server_;
            std::map<std::string, std::unique_ptr<SimInstance>> simulations_;
    };
};

#endif // SGT_SERVER_DOT_H
