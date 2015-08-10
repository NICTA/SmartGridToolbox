#ifndef SGT_SERVER_DOT_H
#define SGT_SERVER_DOT_H

#include "server_http.hpp"

#include <SgtSim/Simulation.h>

#include <memory>

namespace Sgt
{
    class SgtSimEnvironment
    {
        public:
            Sgt::Simulation sim;
    };

    class SgtServer
    {
        public:

            SgtServer(size_t nThreads = 4) : server_(std::unique_ptr<HttpServer>(new HttpServer(8080, nThreads)))
            {
                server_->resource["^/simulations"]["GET"] = 
                    [this](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) 
                    {
                        std::cout << "Get sim" << std::endl;
                        response << "Get sim" << std::endl;
                    };
                server_->resource["^/simulations"]["PUT"] = 
                    [this](HttpServer::Response& response, std::shared_ptr<HttpServer::Request> request) 
                    {
                        auto& sim = simulations_[request->path];
                        if (sim == nullptr)
                        {
                            sim.reset(new Sgt::Simulation);
                        }
                        std::cout << "Created sim" << std::endl;
                        response << "Created sim" << std::endl;
                    };
            }

            void run()
            {
                // std::thread server_thread([this](){server_->start();});
                // server_thread.join();   
                server_->start();
            }

        private:

            typedef SimpleWeb::Server<SimpleWeb::HTTP> HttpServer;

        private:

            std::unique_ptr<HttpServer> server_;
            std::map<std::string, std::unique_ptr<Sgt::Simulation>> simulations_;
    };
}

#endif // SGT_SERVER_DOT_H
