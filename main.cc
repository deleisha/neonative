#include<iostream>
#include<cassert>
#include<uv.h>
#include "macros.h"
#include "loop.h"



NEO_NAMESPACE_BEGIN
/*
 *
  TCP is subclass of Stream handle and we are not explicitly subclassing here
  as libuv already has subclassed it. 
  By subclassing it here we will have multiple copies of uv_handle, and
  uv_stream and also, many of the api's being exposed by libuv are not exposed
  here. it only exposes ONLY those parts which are used in neovim.

  But this is does not mean that we can not exposed it. Any interested parties
  can add those functionality.
 */
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
        TCPClient(const Loop &lp):TCP(lp) {}
};


void echo_write(uv_write_t *req, int status) {
    if (status < 0 ) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    char *base = (char*) req->data;
    free(base);
    free(req);
}


void read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0 ) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_strerror(nread));
        uv_close(reinterpret_cast<uv_handle_t*>(client), NULL);
        return;
    }

    uv_write_t *req = (uv_write_t *) malloc(sizeof(*req));
    req->data = (void*) buf->base;
    asm("int $5");
    uv_write(req, client, buf, 1, echo_write);
}



void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
        buf->base = (char*)malloc(suggested_size);
        buf->len = suggested_size;
}



class  TCPServer :public TCP{
        public:
            TCPServer(const Loop lp):TCP(lp){}

        int listen(const sockaddr* addr, unsigned int flags) {
            neo_bind(addr, flags);
            return _listen(128, connection_cb);
        }

        int read( const TCPClient &client) {
            //int uv_read_start(
                 //      uv_stream_t*,
                //       uv_alloc_cb alloc_cb,
                //       uv_read_cb read_cb
              //    );
        
            return 0;
        }


        protected:
        int neo_bind(const sockaddr* addr, unsigned int flags) {
            int r = uv_tcp_bind(tcp_, addr,flags);
            if(r) {
                std::cerr << uv_strerror(r) << std::endl;
            }
            return r;
        }
        //listen for connection
        int _listen(int backlog, uv_connection_cb cb) {
            return uv_listen(reinterpret_cast<uv_stream_t*>(tcp_), backlog, cb);
        }

        static void connection_cb(uv_stream_t* stream, int status) {
            if ( status ) {
                return;
            }
            //Create client socket now
            TCPClient client(reinterpret_cast<uv_handle_t*>(stream)->loop);
            if (uv_accept(stream, (uv_stream_t*) client.get()) == 0) {
                uv_read_start((uv_stream_t*) client.get(), alloc_buffer, read_cb);
            }
            else
            {
                uv_close((uv_handle_t*) client.get(), NULL);
            }
        }
};


enum Type { CLIENT = 0 ,SERVER };

class TCPFactory {
    public:
    static TCP *create(const Loop &lp, Type tp) {
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



NEO_NAMESPACE_END


using namespace neonative;
int main()
{
    Loop loop;
    TCPServer server(loop);
    struct sockaddr_in bind_addr;
    int r = uv_ip4_addr("0.0.0.0", 6000, &bind_addr);
    assert(!r);
    r = server.listen((struct sockaddr*)&bind_addr, 0);
    if (r) {
        std::cout << uv_strerror(r) << std::endl;
    }
    uv_run(loop.get(), UV_RUN_DEFAULT);
    return 0;
}
