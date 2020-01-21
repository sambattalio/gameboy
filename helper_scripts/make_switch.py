#!/usr/bin/env python3

'''
Script to pull opcodes from url below to a switch statement.
made because its like over 200 of them...
'''

import lxml.html as lh
import requests

def get_table_elements(url):
    ''' Return tr elements from page as list '''
    response = requests.get(url)

    if response.status_code == 200:
        doc = lh.fromstring(response.content)
        return doc.xpath('//tr')
    
    return None

if __name__ == '__main__':
        tr = get_table_elements('https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html')