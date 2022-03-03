#include "context_asm__x64.h"

# Function signature:
# void get_context(Context* current);
#
# Registers:
# RDI : arg[0] -> current
# RSI : arg[1]

#ifndef _WIN32
.type get_context, @function
#endif
.global get_context
get_context:
  # Save the return address and stack pointer.
  movq (%rsp), %r8
  movq %r8, COTTON_CONTEXT_RIP(%rdi) # RIP
  leaq 8(%rsp), %r8
  movq %r8, COTTON_CONTEXT_RSP(%rdi) # RSP

  # Save preserved registers.
  movq %rbx, COTTON_CONTEXT_RBX(%rdi)
  movq %rbp, COTTON_CONTEXT_RBP(%rdi)
  movq %r12, COTTON_CONTEXT_R12(%rdi)
  movq %r13, COTTON_CONTEXT_R13(%rdi)
  movq %r14, COTTON_CONTEXT_R14(%rdi)
  movq %r15, COTTON_CONTEXT_R15(%rdi)

  # Return.
  xorl %eax, %eax
  ret
