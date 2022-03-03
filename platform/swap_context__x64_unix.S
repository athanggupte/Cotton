#include "context_asm__x64.h"

# Function signature:
# void swap_context(Context* current, Context* target);
#
# Registers:
# RDI : arg[0] -> current
# RSI : arg[1] -> target

#ifndef _WIN32
.type get_context, @function
#endif
.global swap_context
swap_context:
  # Save the return address.
  movq (%rsp), %r8
  movq %r8, COTTON_CONTEXT_RIP(%rdi)
  leaq 8(%rsp), %r8
  movq %r8, COTTON_CONTEXT_RSP(%rdi)

  # Save preserved registers.
  movq %rbx, COTTON_CONTEXT_RBX(%rdi)
  movq %rbp, COTTON_CONTEXT_RBP(%rdi)
  movq %r12, COTTON_CONTEXT_R12(%rdi)
  movq %r13, COTTON_CONTEXT_R13(%rdi)
  movq %r14, COTTON_CONTEXT_R14(%rdi)
  movq %r15, COTTON_CONTEXT_R15(%rdi)

  # Load target context into tmp register.
  movq %rsi, %r9

  # Load preserved registers.
  movq COTTON_CONTEXT_RBX(%r9), %rbx
  movq COTTON_CONTEXT_RBP(%r9), %rbp
  movq COTTON_CONTEXT_R12(%r9), %r12
  movq COTTON_CONTEXT_R13(%r9), %r13
  movq COTTON_CONTEXT_R14(%r9), %r14
  movq COTTON_CONTEXT_R15(%r9), %r15

  # Load the first 2 parameters
  movq COTTON_CONTEXT_RDI(%r9), %rdi
  movq COTTON_CONTEXT_RSI(%r9), %rsi

  # Load new stack pointer.
  movq COTTON_CONTEXT_RSP(%r9), %rsp
  # Load new instruction pointer.
  movq COTTON_CONTEXT_RIP(%r9), %r8
  # Push RIP to stack for RET.
  pushq %r8

  # Return 0.
  xorl %eax, %eax
  ret
