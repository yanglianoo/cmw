#ifndef CMW_CROUTINE_ROUTINE_CONTEXT_H_
#define CMW_CROUTINE_ROUTINE_CONTEXT_H_

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <cmw/common/log.h>

extern "C"{
   extern void ctx_swap(void**, void**) asm("ctx_swap"); 
}

namespace hnu {
namespace cmw {
namespace croutine{

// 2M的栈大小
constexpr size_t STACK_SIZE = 2 * 1024 * 1024;

#if defined __aarch64__
constexpr size_t REGISTERS_SIZE = 160;
#else
constexpr size_t REGISTERS_SIZE = 56;
#endif

// 协程的执行体
typedef void (*func)(void*);

struct RoutineContext {
  char stack[STACK_SIZE];
  char* sp = nullptr;
#if defined __aarch64__
} __attribute__((aligned(16)));
#else
};
#endif

// 构建协程上下文
void MakeContext(const func& f1, const void* arg, RoutineContext* ctx);

// 协程上下文切换
inline void SwapContext(char** src_sp, char** dest_sp) {
  ctx_swap(reinterpret_cast<void**>(src_sp), reinterpret_cast<void**>(dest_sp));
}

}
}
}
#endif