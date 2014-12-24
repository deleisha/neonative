NEO_NAMESPACE_BEGIN
/*
 *The code below does not work yet
 *
 */

class Checkhandle :public base::Handle {
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
