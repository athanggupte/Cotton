#include "context_asm__x64.h"

# Function signature:
# void swap_context(Context* current, Context* target);
#
# Registers:
# RCX : arg[0] -> current
# RDX : arg[1] -> target

#ifndef _WIN32
.type get_context, @function
#endif
.global swap_context
swap_context:
  # Save the return address.
  movq (%rsp), %r8
  movq %r8, COTTON_CONTEXT_RIP(%rcx)
  leaq 8(%rsp), %r8
  movq %r8, COTTON_CONTEXT_RSP(%rcx)

  # Save preserved registers.
  movq %rbx, COTTON_CONTEXT_RBX(%rcx)
  movq %rbp, COTTON_CONTEXT_RBP(%rcx)
  movq %r12, COTTON_CONTEXT_R12(%rcx)
  movq %r13, COTTON_CONTEXT_R13(%rcx)
  movq %r14, COTTON_CONTEXT_R14(%rcx)
  movq %r15, COTTON_CONTEXT_R15(%rcx)
  movq %rdi, COTTON_CONTEXT_RDI(%rcx)
  movq %rsi, COTTON_CONTEXT_RSI(%rcx)
  movups %xmm6, COTTON_CONTEXT_XMM6(%rcx)
  movups %xmm7, COTTON_CONTEXT_XMM7(%rcx)
  movups %xmm8, COTTON_CONTEXT_XMM8(%rcx)
  movups %xmm9, COTTON_CONTEXT_XMM9(%rcx)
  movups %xmm10, COTTON_CONTEXT_XMM10(%rcx)
  movups %xmm11, COTTON_CONTEXT_XMM11(%rcx)
  movups %xmm12, COTTON_CONTEXT_XMM12(%rcx)
  movups %xmm13, COTTON_CONTEXT_XMM13(%rcx)
  movups %xmm14, COTTON_CONTEXT_XMM14(%rcx)
  movups %xmm15, COTTON_CONTEXT_XMM15(%rcx)

  # Load target context into tmp register.
  movq %rdx, %r9

  # Load the preserved registers.
  movq COTTON_CONTEXT_RBX(%r9), %rbx
  movq COTTON_CONTEXT_RBP(%r9), %rbp
  movq COTTON_CONTEXT_R12(%r9), %r12
  movq COTTON_CONTEXT_R13(%r9), %r13
  movq COTTON_CONTEXT_R14(%r9), %r14
  movq COTTON_CONTEXT_R15(%r9), %r15
  movq COTTON_CONTEXT_RDI(%r9), %rdi
  movq COTTON_CONTEXT_RSI(%r9), %rsi
  movups COTTON_CONTEXT_XMM6(%r9), %xmm6
  movups COTTON_CONTEXT_XMM7(%r9), %xmm7
  movups COTTON_CONTEXT_XMM8(%r9), %xmm8
  movups COTTON_CONTEXT_XMM9(%r9), %xmm9
  movups COTTON_CONTEXT_XMM10(%r9), %xmm10
  movups COTTON_CONTEXT_XMM11(%r9), %xmm11
  movups COTTON_CONTEXT_XMM12(%r9), %xmm12
  movups COTTON_CONTEXT_XMM13(%r9), %xmm13
  movups COTTON_CONTEXT_XMM14(%r9), %xmm14
  movups COTTON_CONTEXT_XMM15(%r9), %xmm15

  # Load the first 2 parameters.
  movq COTTON_CONTEXT_RCX(%r9), %rcx
  movq COTTON_CONTEXT_RDX(%r9), %rdx

  # Load new stack pointer.
  movq COTTON_CONTEXT_RSP(%r9), %rsp
  # Load new instruction pointer.
  movq COTTON_CONTEXT_RIP(%r9), %r8
  # Push RIP to stack for RET.
  pushq %r8

  # Return 0.
  xorl %eax, %eax
  ret
