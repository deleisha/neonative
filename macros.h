#ifndef __NEO_MACROS__
#define __NEO_MACROS__
#define NEO_NAMESPACE_BEGIN \
    namespace neonative {

#define NEO_NAMESPACE_END    \
    }


//Unreachable statement
#if defined(__clang__ )
#  define NEO_UNREACHABLE(CODE) __builtin_unreachable();
#elif defined(GCC_VERSION)
#  if GCC_VERSION >= 40500 //Unreachable supported only for gcc 4.5 and above
#    define NEO_UNREACHABLE(CODE) __builtin_unreachable();
#  else
#    define NEO_UNREACHABLE(CODE) CODE
#  endif
#elif defined(_MSC_VER) //PEGASUS_OS_TYPE_WINDOWS
#  define NEO_UNREACHABLE(CODE) __assume(0);
# else
#  define NEO_UNREACHABLE(CODE) CODE
#endif 


#endif // __NEO_MACROS__
