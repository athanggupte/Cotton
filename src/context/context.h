#ifdef _WIN32
    #include <emmintrin.h>
#endif

#include <stddef.h>

struct Context
{
    void *rip, *rsp;
    void *rbx, *rbp, *r12, *r13, *r14, *r15;
#ifdef _WIN32
    void *rdi, *rsi;
    __m128i xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif

// Arguments #1 and #2
#if defined(__unix__) || defined(UNIX)
    void *rdi, *rsi;
#elif defined(_WIN32)
    void *rcx, *rdx;
#endif
};

extern void (*get_context)(Context*);
extern void (*set_context)(Context*);
extern void (*swap_context)(Context* current, Context* target);

void init_stack(Context* ctx, char* stack, size_t stack_size);
