#include<iostream>
#include<cassert>
#include<uv.h>


#define NEO_NAMESPACE_BEGIN \
    namespace neonative {

#define NEO_NAMESPACE_END    \
    }

NEO_NAMESPACE_BEGIN
class Loop {
    public:
    Loop(bool use_default = true):
        loop_(nullptr)
        ,is_default_loop_(use_default)
    {
        if(use_default) {
            loop_ = uv_default_loop();
        }
        else {
            loop_ = new uv_loop_t;
            uv_loop_init(loop_);
        }
    }
    ~Loop() {
        if(!is_default_loop_ && loop_) {
            int r = uv_loop_close(loop_);
            delete loop_;
            loop_ = nullptr;
        }
    }



    bool is_default_loop() const { return is_default_loop_ ; }
    uv_loop_t* get() const { return loop_; }


    bool is_alive() const;

    void stop();

    private:
    uv_loop_t *loop_;
    bool is_default_loop_;
};

bool Loop::is_alive() const {
    return !!uv_loop_alive(loop_);
}

void Loop::stop() 
{
    uv_stop(loop_);
}


class  TCP {
        public:
        TCP(const Loop &loop):tcp_(new uv_tcp_t)
        {
            uv_tcp_init(loop.get(), tcp_);
        }
        ~TCP()
        {
            if(tcp_) {
                delete tcp_;
                tcp_ = 0;
            }
        }

        int listen(const sockaddr* addr, unsigned int flags) {
            neo_bind(addr, flags);
            return _listen(128, connection_cb);
        }
        protected:
        int neo_bind(const sockaddr* addr, unsigned int flags) {
            int r = uv_tcp_bind(tcp_, addr,flags);
            if(r) {
                uv_strerror(r);
            }
        }
        int _listen(int backlog, uv_connection_cb cb) {
            return uv_listen(reinterpret_cast<uv_stream_t*>(tcp_), backlog, cb);
        }
        static void connection_cb(uv_stream_t* stream, int status) {

            std::cout << "Yahoo:" << std::endl;
        }

        private:
        uv_tcp_t* tcp_;
};


NEO_NAMESPACE_END


using namespace neonative;
int main()
{
    Loop loop;
    TCP server(loop);
    struct sockaddr_in bind_addr;
    int r = uv_ip4_addr("0.0.0.0", 6000, &bind_addr);
    assert(!r);
    r = server.listen((struct sockaddr*)&bind_addr, 0);
    if (r) {
        std::cout << "Error on listening" << uv_strerror(r) << std::endl;
    }
    uv_run(loop.get(), UV_RUN_DEFAULT);
    return 0;
}
