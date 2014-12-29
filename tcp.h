#ifndef __NEO_TCP_HANDLE__
#define __NEO_TCP_HANDLE__
#include "macros.h"
#include<uv.h>
#include<cstring>


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
            tcp_->data = this;
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

class  TCPServer :public TCP
{
        public:
            TCPServer(const Loop &lp):TCP(lp)
            {}

        //listen for connections
        int listen(
            std::string host, 
            unsigned port,
            std::function<int(const std::string &)> callback,
            int backlog = 128
        )
        {

            struct sockaddr_in bind_addr;
            int r = uv_ip4_addr(host.c_str(), port, &bind_addr);
            assert(!r);
            
            if( !_bind((struct sockaddr*) &bind_addr, 0))
            {
                //listen here
                uv_connection_cb cb = handle_conn;
                r = uv_listen(reinterpret_cast<uv_stream_t*>(tcp_), backlog, cb);
                if(r) {
                    std::cerr << "listen: " << uv_strerror(r) << std::endl;
                }
            }
            return  r;

        }

        //TODO : implement it later
        int close( )
        {
            return  0;
        }

        protected:
        int _bind(const sockaddr* addr, unsigned int flags) {
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

        static void handle_conn(uv_stream_t* server_handle, int status)
        {
            if ( status )
            {
                return;
            }

            uv_tcp_t *client = new uv_tcp_t;
            uv_tcp_init(server_handle->loop, client);

            int r = uv_accept(server_handle, reinterpret_cast<uv_stream_t*>(client));
            if(r) {
                std::cerr << "accept: " << uv_strerror(r) << std::endl;
                uv_close((uv_handle_t*) client, on_close);
                return;
            }
            //Read the request
            uv_read_start((uv_stream_t*) client, alloc_cb, read_cb);
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
                    std::cerr << "Read Error: " << uv_strerror(nread) << std::endl;
                }
            }
            else { //Write result back
                uv_write_t req;
                memset(&req, 0, sizeof(req));
                int r = uv_write(&req, client, &buf, 1, NULL);
                if(r) {
                    std::cerr << "Error: " << uv_strerror(r) << std::endl;
                }
            }

            free(buf->base);
        }

        //call back for uv_close
        static void on_close(uv_handle_t* handle) {
            delete(handle);
            handle = 0;
            fprintf(stderr, "disconnected\n");
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

} //namespace net

NEO_NAMESPACE_END
#endif
