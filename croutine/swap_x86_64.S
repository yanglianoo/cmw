
/** 
对于 x86-64 下的 System V AMD64 ABI 调用约定
函数的 第一个参数 被放在 %rdi 寄存器中。
函数的 第二个参数 被放在 %rsi 寄存器中
src_sp 被放入 %rdi，表示第一个参数
dest_sp 被放入 %rsi，表示第二个参数。
*/
.globl ctx_swap
.type  ctx_swap, @function
ctx_swap:
      pushq %rdi
      pushq %r12
      pushq %r13
      pushq %r14
      pushq %r15
      pushq %rbx
      pushq %rbp
      movq %rsp, (%rdi)

      movq (%rsi), %rsp
      popq %rbp
      popq %rbx
      popq %r15
      popq %r14
      popq %r13
      popq %r12
      popq %rdi
      ret