NEO_NAMESPACE_BEGIN
namespace base {
    /*
     Will be base class for other stream like SSL/TLS stream
    */
    class Stream {
        public:
            Stream() {}
            ~Stream() {}
        protected:
            uv_stream_t* stream_;
    };
}
NEO_NAMESPACE_END
