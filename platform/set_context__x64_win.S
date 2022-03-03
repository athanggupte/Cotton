#include "context_asm__x64.h"

# Function signature:
# void set_context(Context* target);
#
# Registers:
# RCX : arg[0] -> target
# RDX : arg[1]

#ifndef _WIN32
.type get_context, @function
#endif
.global set_context
set_context:
  # Load the new stack pointer.
  movq COTTON_CONTEXT_RSP(%rcx), %rsp

  # Load the preserved registers.
  movq COTTON_CONTEXT_RBX(%rcx), %rbx
  movq COTTON_CONTEXT_RBP(%rcx), %rbp
  movq COTTON_CONTEXT_R12(%rcx), %r12
  movq COTTON_CONTEXT_R13(%rcx), %r13
  movq COTTON_CONTEXT_R14(%rcx), %r14
  movq COTTON_CONTEXT_R15(%rcx), %r15
  movq COTTON_CONTEXT_RDI(%rcx), %rdi
  movq COTTON_CONTEXT_RSI(%rcx), %rsi
  movups COTTON_CONTEXT_XMM6(%rcx), %xmm6
  movups COTTON_CONTEXT_XMM7(%rcx), %xmm7
  movups COTTON_CONTEXT_XMM8(%rcx), %xmm8
  movups COTTON_CONTEXT_XMM9(%rcx), %xmm9
  movups COTTON_CONTEXT_XMM10(%rcx), %xmm10
  movups COTTON_CONTEXT_XMM11(%rcx), %xmm11
  movups COTTON_CONTEXT_XMM12(%rcx), %xmm12
  movups COTTON_CONTEXT_XMM13(%rcx), %xmm13
  movups COTTON_CONTEXT_XMM14(%rcx), %xmm14
  movups COTTON_CONTEXT_XMM15(%rcx), %xmm15

  # Should return to the address set with {get, swap}_context.
  movq COTTON_CONTEXT_RIP(%rcx), %r8 # RIP
  # Push RIP to stack for RET
  pushq %r8

  # Return.
  xorl %eax, %eax
  ret
