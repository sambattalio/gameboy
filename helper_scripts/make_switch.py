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

        print(tr[1])
        for i in tr[1][1:]:
            print(i.text_content())

        count = 0
        
        for x in tr[1:17]:
            for y in x[1:]:
                text = str(y.text_content()).replace('\xa0', ' ')
                print('case {}:'.format(hex(count).upper().replace('X', 'x')))
                if not text.isspace():
                    print('\t// ' + re.search(r'[a-zA-Z]+([\s]+[0-9A-Za-z]+(,[\s]+[0-9a-zA-Z]+)?)?', text)[0])
                    print('\t// ' + re.search(r'[0-9][ \s\t]+[0-9]+', text)[0])
                    print('\t// ' + re.search(r'[Z01-][\s][N10-][\s][H01-][\s][01C-]', text)[0])
                print('\tbreak;')
                count += 1
        '''
        for x in tr[1:]:
            for y in x[1:]:
                print(x.text_content())
                print('\n\n')
            break
        '''