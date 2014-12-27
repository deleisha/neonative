class NeoError {
    public:
    NeoError(int r ):errno_(uv_errno_t(r))
    {
        assert( r <= 0);
    }
    ~NeoError() {
    }

    uv_errno_t getCode() const
    {
        return  errno_;
    }

    const char* getName() const {
        return  uv_err_name(errno_);
    }
    const char *str() const {
        return  uv_strerror(errno_);
    }

    private:
    uv_errno_t errno_;
};

