#!/bin/python3
import sys
import os
import subprocess
import re

assert len(sys.argv) >= 3, "Usage: %s <filepath> <symbol> [options...]" % (sys.argv[0])

scriptPath = os.path.dirname(os.path.abspath(__file__))
filepath = sys.argv[1]
symbolName = sys.argv[2]
origSymbolName = symbolName

for opt in sys.argv[3:]:
    if opt == '--strip-symbol':
        symbolName = symbolName[:symbolName.find('__')]

objdump = scriptPath + '..\\vendor\\bin\\objdump.exe' if sys.platform == 'win32' else 'objdump'

def extract_objdump_from_file(filepath, symbolName):
    result = subprocess.run([objdump, '--disassemble=' + symbolName, filepath], stdout=subprocess.PIPE)
    out = result.stdout.split(b'\n', 7)[7].decode('ascii')

    byte_block = []
    for line in out.splitlines():
        # print("Line #: ", line)
        st=re.match(r'^\s+([0-9A-Fa-f]+):\s+?'    # Address starting with one or more space
                    r'(([0-9A-Fa-f]{2}\s)+)'      # Bytes
        #            r'\s+([a-zA-Z]+)\s+'           # Instruction
        #            r'(\S+)$'                   # Parameters
                    ,line)
        
        if st:
            byte_line = []
            for byte_ in st.group(2).split(' '):
                if byte_:
                    byte_line += [ "0x%s" % byte_ ]
            byte_block += [ ', '.join(byte_line) ]
    return byte_block

out_filepath = os.path.join(os.path.dirname(filepath), '../embed', origSymbolName + '.c')
byte_block = extract_objdump_from_file(filepath, symbolName)
with open(out_filepath, 'w') as file:
    file.write('COTTON_ALLOCATE_TEXT_SECTION\n')
    file.write('static unsigned char %s[] = {\n\t' % (symbolName + '_embedded').split('.')[0])
    file.write(',\n\t'.join(byte_block))
    file.write('\n};\n')