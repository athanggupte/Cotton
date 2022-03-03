#!/bin/python3
import sys
import os

assert len(sys.argv) >= 1, "Usage: %s <arch_str> <dir_path> [options...]" % (sys.argv[0])
_script_file_name = os.path.basename(sys.argv[0])

arch_str = sys.argv[1]
dir_path = sys.argv[2] if len(sys.argv) > 1 else '.'

symbol_names = [ 'get_context', 'set_context', 'swap_context' ]
file_names = [ f'{dir_path}/{symbolName}__{arch_str}.c' for symbolName in symbol_names ]

preprocessor_str = '''
#if defined(__GNUC__) || defined(__clang__)
    #define COTTON_ALLOCATE_TEXT_SECTION \\
    __attribute__((section (".text#")))
#elif defined(_MSC_VER)
    #pragma section(".text")
    #define COTTON_ALLOCATE_TEXT_SECTION \\
    __declspec(allocate(".text"))
#endif
'''

declaration_str = '''
typedef struct Context Context;

void (*get_context)(Context*) = (void (*)(Context*))get_context_embedded;
void (*set_context)(Context*) = (void (*)(Context*))set_context_embedded;
void (*swap_context)(Context*, Context*) = (void (*)(Context*, Context*))swap_context_embedded;
'''

with open(f'{dir_path}/context__{arch_str}.c', 'w') as out_file:
    out_file.write(f'// Auto-generated using `scripts/{_script_file_name}`\n')
    out_file.write(f'// Target architecture: {arch_str}\n')
    out_file.write(preprocessor_str + '\n')

    for filepath in file_names:
        with open(filepath, 'r') as file:
            for line in file:
                out_file.write(line)
        out_file.write('\n')

    out_file.write(declaration_str)
