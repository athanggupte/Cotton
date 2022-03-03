#include "context_asm__x64.h"

# Function signature:
# void set_context(Context* target);
#
# Registers:
# RDI : arg[0] -> target
# RSI : arg[1]

#ifndef _WIN32
.type get_context, @function
#endif
.global set_context
set_context:
  # Load new stack pointer.
  movq COTTON_CONTEXT_RSP(%rdi), %rsp

  # Load preserved registers.
  movq COTTON_CONTEXT_RBX(%rdi), %rbx
  movq COTTON_CONTEXT_RBP(%rdi), %rbp
  movq COTTON_CONTEXT_R12(%rdi), %r12
  movq COTTON_CONTEXT_R13(%rdi), %r13
  movq COTTON_CONTEXT_R14(%rdi), %r14
  movq COTTON_CONTEXT_R15(%rdi), %r15

  # Should return to the address set with {get, swap}_context.
  movq COTTON_CONTEXT_RIP(%rdi), %r8
  # Push RIP to stack for RET.
  pushq %r8

  # Return.
  xorl %eax, %eax
  ret
  