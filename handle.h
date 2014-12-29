#ifndef __NEO_HANDLE__
#define __NEO_HANDLE__

#include "macros.h"
#include<uv.h>

/*
 *The code below does not work yet
 *
 */

NEO_NAMESPACE_BEGIN
namespace base {
    class Handle {
    public:
        Handle() {}
        virtual ~Handle() {}

//        virtual bool is_active(void) const = 0;
//        bool is_closing() const;
//        void close();
//        void close_handle();
//        bool has_ref();
//        static void uv_ref(uv_handle_t* handle);
//        static void uv_unref(uv_handle_t* handle);
//        static int uv_has_ref(const uv_handle_t* handle);
//        size_t uv_handle_size(uv_handle_type type);
    private:
    uv_handle_t handle_;
  //  typedef uv_handle_type HandleType;
   // HandleType handle_type;

    };
}
NEO_NAMESPACE_END
#endif // 
