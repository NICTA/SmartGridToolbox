#include "SgtServer.h"
#include "server_http.hpp"
#include <thread>

int main()
{
    using namespace Sgt;
    SgtServer sgtServer(1);
    sgtServer.run();
};
