#include <cmw/croutine/croutine_context.h>

namespace hnu {
namespace cmw {
namespace croutine{

//  The stack layout looks as follows:
//
//              +------------------+
//              |      Reserved    |
//              +------------------+
//              |  Return Address  |   f1
//              +------------------+
//              |        RDI       |   arg
//              +------------------+
//              |        R12       |
//              +------------------+
//              |        R13       |
//              +------------------+
//              |        ...       |
//              +------------------+
// ctx->sp  =>  |        RBP       |
//              +------------------+

void MakeContext(const func &f1, const void *arg, RoutineContext *ctx){
    ctx->sp = ctx->stack + STACK_SIZE - 2 * sizeof(void*) - REGISTERS_SIZE;
    std::memset(ctx->sp, 0, REGISTERS_SIZE);
#ifdef __aarch64__
  char *sp = ctx->stack + STACK_SIZE - sizeof(void *);
#else
  char *sp = ctx->stack + STACK_SIZE - 2 * sizeof(void *);
#endif    
  *reinterpret_cast<void **>(sp) = reinterpret_cast<void *>(f1);
  sp -= sizeof(void *);
  *reinterpret_cast<void **>(sp) = const_cast<void *>(arg);
}

}
}
}