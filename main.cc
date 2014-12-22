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


namespace base {
    class Handle {
    public:
        Handle() {}
        virtual ~Handle() {}

        bool is_active(void) const;
        bool is_closing() const;
        void close();
        void close_handle();
        bool has_ref();
    private:
    //uv_any_handle hdl;
  //  typedef uv_handle_type HandleType;
   // HandleType handle_type;

    };

    class StreamHandle : public Handle {
    };

}


class  TCPHandle :public StreamHandle{
        public:
        TCPHandle();
        ~TCPHandle();
        
        private:
};
class Checkhandle :public Handle {
        public:
        Checkhandle(const Loop &loop);
        ~Checkhandle();

        int start();
        int stop();
        private:
        uv_check_t *hdl_;
    };

    Checkhandle::Checkhandle(const Loop &loop) {
        int r = uv_check_init(loop.get(), hdl_);
    }

    Checkhandle::~Checkhandle() {
    }

    int Checkhandle::start() {
    }

    int Checkhandle::stop() {
    }


NEO_NAMESPACE_END


void connection_cb(uv_stream_t* srvr, int status) {
    std::cout << "Yahoo:" << std::endl;
}

using namespace neonative;
int main()
{
    Loop loop;
    uv_tcp_t server;
    uv_tcp_init(loop.get(), &server);
    struct sockaddr_in bind_addr;
    int r = uv_ip4_addr("0.0.0.0", 7000, &bind_addr);
    assert(!r);
    uv_tcp_bind(&server, (struct sockaddr*)&bind_addr, 0);
    r = uv_listen((uv_stream_t*) &server, 128 /*backlog*/, connection_cb);
    uv_run(loop.get(), UV_RUN_DEFAULT);
    return 0;
}
