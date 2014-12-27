#include<iostream>
#include<cassert>
#include<uv.h>
#include<unistd.h>
#include "macros.h"
#include "loop.h"



NEO_NAMESPACE_BEGIN
/*
 *
  TCP is subclass of Stream handle and we are not explicitly subclassing here
  as libuv already has subclassed it. 
  By subclassing it here we will have multiple copies of uv_handle, and
  uv_stream and also, many of the api's being exposed by libuv are not exposed
  here. it only exposes ONLY those parts which are used in neonative.

  But this is does not mean that we can not exposed it. Any interested parties
  can add those functionality.
 */

namespace net {
class TCP {
    public:
        TCP(const Loop &loop):tcp_(new uv_tcp_t)
        {
            uv_tcp_init(loop.get(), tcp_);
        }
        virtual ~TCP()
        {
            if(tcp_) {
                delete tcp_;
                tcp_ = 0;
            }
        }
        uv_tcp_t *get() const { return tcp_;};

    protected:
        uv_tcp_t* tcp_;
};

class TCPClient : public TCP {
    public:
        explicit TCPClient(const Loop &lp):TCP(lp) {}
};

void on_close(uv_handle_t* handle) {
    free(handle);
    handle = 0;
    fprintf(stderr, "disconnected\n");
}


class  TCPServer :public TCP
{
        public:
            TCPServer(const Loop &lp):TCP(lp)
            {}

        //listen for connections
        int listen(const sockaddr* addr, callback) {
            neo_bind(addr, 0);
            return _listen(128, connection_cb);
        }

        int listen(std::string host, unsigned port, int backlog, callback)
        {
        }

        int close( callback)
        {
        }

        bool bind(const sockaddr* addr, unsigned flags) {
            return neo_bind(addr, flags) == 0;
        }

        //Read from the given client
        int read( const TCPClient &client) {
            return 0;
        }


        protected:
        int neo_bind(const sockaddr* addr, unsigned int flags) {
            int r = uv_tcp_bind(tcp_, addr,flags);
            if(r) {
                std::cerr << uv_strerror(r) << std::endl;
            }
            return !!r;
        }
        //listen for connection
        int _listen(int backlog, uv_connection_cb cb) {
            return uv_listen(reinterpret_cast<uv_stream_t*>(tcp_), backlog, cb);
        }

        static void connection_cb(uv_stream_t* server_handle, int status) {
            if ( status ) {
                return;
            }

            //Create client socket now
            TCPClient *client = new TCPClient(server_handle->loop);
            uv_tcp_init(server_handle->loop, client->get());

            if (uv_accept(server_handle, reinterpret_cast<uv_stream_t*>(client->get())) == 0) {
                uv_read_start((uv_stream_t*) client->get(), alloc_cb, read_cb);
            }
            else
            {
                uv_close((uv_handle_t*) client->get(), on_close);
            }
        }

        static void alloc_cb(
            uv_handle_t *handle,
            size_t suggested_size,
            uv_buf_t *buf) 
        {
            buf->base = (char*)malloc(suggested_size);
            buf->len = suggested_size;
        }



        static void read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
        {
            if (nread < 0 ) {
                if (nread == UV_EOF)
                {
                    uv_close(reinterpret_cast<uv_handle_t*>(client), on_close);
                }
                else {
                    fprintf(stderr, "Read Error  %s\n", uv_strerror(nread));
                }
            }
            else {
                write(1, buf->base, nread);
            }

            free(buf->base);
        }
};


enum Type { CLIENT = 0 ,SERVER };

class TCPFactory {
    public:
    TCP *create(const Loop &lp, Type tp) {
        switch(tp) {
            case CLIENT:
                return new TCPClient(lp);
            case SERVER:
                return new TCPServer(lp);
            default:
                NEO_UNREACHABLE(assert(0);)
        }
    }
};

TCPServer* createServer(const Loop &lp) {
    return new TCPServer(lp);
}

}



NEO_NAMESPACE_END


using namespace neonative;
using namespace std;
int main()
{
    Loop loop;

    net::TCPServer *server = net::createServer(loop);
    struct sockaddr_in bind_addr;
    int r = uv_ip4_addr("0.0.0.0", 6000, &bind_addr);
    assert(!r);
    r = server->listen((struct sockaddr*)&bind_addr, 0);
    if (r) {
        std::cout << uv_strerror(r) << std::endl;
    }
    uv_run(loop.get(), UV_RUN_DEFAULT);

    return 0;
}
