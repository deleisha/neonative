#ifndef __NEO_LOOP_
#define __NEO_LOOP_
#include<uv.h>
#include "macros.h"


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
NEO_NAMESPACE_END
#endif //__NEO_LOOP_
