#include "SgtServer.h"
#include "WebServer/server_http.hpp"
#include <thread>

int main()
{
    using namespace Sgt;
    SgtServer sgtServer(1);
    sgtServer.run();
};
