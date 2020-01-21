#!/usr/bin/env python3

'''
Script to pull opcodes from url below to a switch statement.
made because its like over 200 of them...
'''

import lxml.html as lh
import requests
import re

def get_table_elements(url):
    ''' Return tr elements from page as list '''
    response = requests.get(url)

    if response.status_code == 200:
        doc = lh.fromstring(response.content)
        return doc.xpath('//tr')
    
    return None

if __name__ == '__main__':
        tr = get_table_elements('https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html')

        count = 0
        ''' for normal opcode (8bit) '''  
        for x in tr[1:17]:
            for y in x[1:]:
                text = str(y.text_content()).replace('\xa0', ' ')
                print('case {}:'.format(hex(count).upper().replace('X', 'x')))
                if not text.isspace():
                    flags = text[-7:]
                    text = text[:-7]
                    size_and_cycles = text.split()[-2][-1] + ' ' + text.split()[-1]
                    inst = ''.join(text.split()[:-2]) + ' ' + text.split()[-2][:-1]
                    print('\t// ' + inst)
                    print('\t// ' + size_and_cycles)
                    print('\t// ' + flags)
                print('\tbreak;')
                count += 1
        ''' for prefix CB '''
        count = 0
        exit(0)
        for x in tr[18:34]:
            for y in x[1:]:
                text = str(y.text_content()).replace('\xa0', ' ')
                print('case {}:'.format(hex(count).upper().replace('X', 'x')))
                if not text.isspace():
                    flags = text[-7:]
                    text = text[:-7]
                    size_and_cycles = text.split()[-2][-1] + ' ' + text.split()[-1]
                    inst = ''.join(text.split()[:-2]) + ' ' + text.split()[-2][:-1]

                    print('\t// ' + inst)
                    print('\t// ' + size_and_cycles)
                    print('\t// ' + flags)
                print('\tbreak;')
                count += 1