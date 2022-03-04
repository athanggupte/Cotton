// Auto-generated using `scripts/amalgamate_embedded_files.py`
// Target architecture: x64_win

#if defined(__GNUC__) || defined(__clang__)
    #define COTTON_ALLOCATE_TEXT_SECTION \
    __attribute__((section (".text#")))
#elif defined(_MSC_VER)
    #pragma section(".text")
    #define COTTON_ALLOCATE_TEXT_SECTION \
    __declspec(allocate(".text"))
#endif

COTTON_ALLOCATE_TEXT_SECTION
static unsigned char get_context_embedded[] = {
	0x4c, 0x8b, 0x04, 0x24,
	0x4c, 0x89, 0x01,
	0x4c, 0x8d, 0x44, 0x24, 0x08,
	0x4c, 0x89, 0x41, 0x08,
	0x48, 0x89, 0x59, 0x10,
	0x48, 0x89, 0x69, 0x18,
	0x4c, 0x89, 0x61, 0x20,
	0x4c, 0x89, 0x69, 0x28,
	0x4c, 0x89, 0x71, 0x30,
	0x4c, 0x89, 0x79, 0x38,
	0x48, 0x89, 0x79, 0x40,
	0x48, 0x89, 0x71, 0x48,
	0x0f, 0x11, 0x71, 0x50,
	0x0f, 0x11, 0x79, 0x60,
	0x44, 0x0f, 0x11, 0x41, 0x70,
	0x44, 0x0f, 0x11, 0x89, 0x80, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0x91, 0x90, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0x99, 0xa0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xa1, 0xb0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xa9, 0xc0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xb1, 0xd0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xb9, 0xe0, 0x00, 0x00,
	0x00,
	0x31, 0xc0,
	0xc3
};

COTTON_ALLOCATE_TEXT_SECTION
static unsigned char set_context_embedded[] = {
	0x48, 0x8b, 0x61, 0x08,
	0x48, 0x8b, 0x59, 0x10,
	0x48, 0x8b, 0x69, 0x18,
	0x4c, 0x8b, 0x61, 0x20,
	0x4c, 0x8b, 0x69, 0x28,
	0x4c, 0x8b, 0x71, 0x30,
	0x4c, 0x8b, 0x79, 0x38,
	0x48, 0x8b, 0x79, 0x40,
	0x48, 0x8b, 0x71, 0x48,
	0x0f, 0x10, 0x71, 0x50,
	0x0f, 0x10, 0x79, 0x60,
	0x44, 0x0f, 0x10, 0x41, 0x70,
	0x44, 0x0f, 0x10, 0x89, 0x80, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0x91, 0x90, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0x99, 0xa0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0xa1, 0xb0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0xa9, 0xc0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0xb1, 0xd0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x10, 0xb9, 0xe0, 0x00, 0x00,
	0x00,
	0x4c, 0x8b, 0x01,
	0x41, 0x50,
	0x31, 0xc0,
	0xc3
};

COTTON_ALLOCATE_TEXT_SECTION
static unsigned char swap_context_embedded[] = {
	0x4c, 0x8b, 0x04, 0x24,
	0x4c, 0x89, 0x01,
	0x4c, 0x8d, 0x44, 0x24, 0x08,
	0x4c, 0x89, 0x41, 0x08,
	0x48, 0x89, 0x59, 0x10,
	0x48, 0x89, 0x69, 0x18,
	0x4c, 0x89, 0x61, 0x20,
	0x4c, 0x89, 0x69, 0x28,
	0x4c, 0x89, 0x71, 0x30,
	0x4c, 0x89, 0x79, 0x38,
	0x48, 0x89, 0x79, 0x40,
	0x48, 0x89, 0x71, 0x48,
	0x0f, 0x11, 0x71, 0x50,
	0x0f, 0x11, 0x79, 0x60,
	0x44, 0x0f, 0x11, 0x41, 0x70,
	0x44, 0x0f, 0x11, 0x89, 0x80, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0x91, 0x90, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0x99, 0xa0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xa1, 0xb0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xa9, 0xc0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xb1, 0xd0, 0x00, 0x00,
	0x00,
	0x44, 0x0f, 0x11, 0xb9, 0xe0, 0x00, 0x00,
	0x00,
	0x49, 0x89, 0xd1,
	0x49, 0x8b, 0x59, 0x10,
	0x49, 0x8b, 0x69, 0x18,
	0x4d, 0x8b, 0x61, 0x20,
	0x4d, 0x8b, 0x69, 0x28,
	0x4d, 0x8b, 0x71, 0x30,
	0x4d, 0x8b, 0x79, 0x38,
	0x49, 0x8b, 0x79, 0x40,
	0x49, 0x8b, 0x71, 0x48,
	0x41, 0x0f, 0x10, 0x71, 0x50,
	0x41, 0x0f, 0x10, 0x79, 0x60,
	0x45, 0x0f, 0x10, 0x41, 0x70,
	0x45, 0x0f, 0x10, 0x89, 0x80, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0x91, 0x90, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0x99, 0xa0, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0xa1, 0xb0, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0xa9, 0xc0, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0xb1, 0xd0, 0x00, 0x00,
	0x00,
	0x45, 0x0f, 0x10, 0xb9, 0xe0, 0x00, 0x00,
	0x00,
	0x49, 0x8b, 0x89, 0xf0, 0x00, 0x00, 0x00,
	0x49, 0x8b, 0x91, 0xf8, 0x00, 0x00, 0x00,
	0x49, 0x8b, 0x61, 0x08,
	0x4d, 0x8b, 0x01,
	0x41, 0x50,
	0x31, 0xc0,
	0xc3
};


typedef struct Context Context;

void (*get_context)(Context*) = (void (*)(Context*))get_context_embedded;
void (*set_context)(Context*) = (void (*)(Context*))set_context_embedded;
void (*swap_context)(Context*, Context*) = (void (*)(Context*, Context*))swap_context_embedded;
