#include<iostream>
#include<cassert>
#include<uv.h>
#include<unistd.h>
#include<functional>
#include "macros.h"
#include "loop.h"




static int welcome(const std::string &str) {
    std::cout << "Welcome " << str << std::endl;
    return  0;
}
using namespace neonative;
using namespace std;
int main()
{
    Loop loop;

    net::TCPServer *server = net::createServer(loop);
    int r = server->listen("0.0.0.0", 6000, welcome);
    if (r) {
        std::cout << uv_strerror(r) << std::endl;
    }
    uv_run(loop.get(), UV_RUN_DEFAULT);

    return 0;
}
