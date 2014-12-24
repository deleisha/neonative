/*
 *The code below does not work yet
 *
 */
class Idle : public Handle {
    public:
    Idle();
    ~Idle();
    virtual bool is_active() const;
    int start();
    int stop();

    bool is_cb_set() const { return idle_cb_ != 0;}


    void set_cb(uv_idle_cb callback) {
        if( !is_cb_set()) {
            idle_cb_ =  callback;
        }
    }
    private:
    uv_idle_cb idle_cb_;
    uv_idle_t *idle_handle_;
};


Idle::Idle():idle_handle_(new uv_idle_t), idle_cb_(0)
{
}


Idle::~Idle() {
    if(idle_handle_) {
        delete idle_handle_;
        idle_handle_ = 0;
    }
    idle_cb_  = 0;
}
bool Idle::is_active() const {
        return !!uv_is_active(reinterpret_cast<uv_handle_t*>(idle_handle_));
}


