#include "context_asm__x64.h"

# Function signature:
# void get_context(Context* current);
#
# Registers:
# RCX : arg[0] -> current
# RDX : arg[1]

#ifndef _WIN32
.type get_context, @function
#endif
.global get_context
get_context:
  # Save the return address and stack pointer.
  movq (%rsp), %r8
  movq %r8, COTTON_CONTEXT_RIP(%rcx) # RIP
  leaq 8(%rsp), %r8
  movq %r8, COTTON_CONTEXT_RSP(%rcx) # RSP

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

  # Return.
  xorl %eax, %eax
  ret
  