#!/usr/bin/env python3
# Script to generate icons.h header file for esp32-weather-epd.
# Copyright (C) 2025  Luke Marzen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
import os
import re
import json
from compact_json import Formatter, EolStyle

"""
The following function was written by google AI, command:
"write a python function to convert octal encoded utf8 characters in a 
string to utf8 characters in a new string"

Then:
"modify function to accept single byte sequences as well"
"""
def decode_any_octal_to_utf8(text: str) -> str:
    """
    Converts literal octal escape sequences (e.g., '\\304\\201' or a single '\\304')
    inside a text string into characters, handling both multi-byte UTF-8 
    and fallback single-byte interpretations safely.
    """
    # Match one or more consecutive 3-digit octal sequences
    octal_pattern = re.compile(r'(?:\\[0-7]{3})+')
    
    def replace_octal_block(match):
        octal_block = match.group(0)
        # Extract just the 3-digit numbers
        octal_digits = re.findall(r'\\([0-7]{3})', octal_block)
        # Convert each octal string to an integer byte
        byte_values = [int(num, 8) for num in octal_digits]
        byte_data = bytes(byte_values)
        try:
            # First try: Decode as multi-byte UTF-8
            return byte_data.decode('utf-8')
        except UnicodeDecodeError:
            # Second try: Decode as Latin-1/Windows-1252 for single bytes
            # 'surrogateescape' protects any completely unmappable bytes from crashing
            return byte_data.decode('latin-1', errors='surrogateescape')
    return octal_pattern.sub(replace_octal_block, text)

def clean_value(value):
    if '{' in value:
        value = value.replace("{","[")
        value = value.replace("}","]")
        value = eval(value)
        for i in range(0,len(value)):
            value[i] = decode_any_octal_to_utf8(value[i])
    else:
        value = decode_any_octal_to_utf8(value)
    return value

def generate_locales():
    # Create a list of TXT_* variables referenced by DrawOwm source files
    Owm2Oepl = {'LC_DAY': "days", 'LC_ABDAY': 'daysShort','LC_MON': 'months',
    'LC_D_FMT': '' }
    ignore_variables = {"LC_DAY","LC_ABDAY","LC_MON"}
    LC_ABMON_re = r'const char \*(LC_ABMON).*\s*=(.+);'
    LC_re = r'const char \*(LC_\w++).*\s*=\s*"(.+)";'

    variables = {}
    source_files = sorted(os.listdir('../lib/DrawOwm/'))
    for source_file in source_files:
        if source_file.endswith('.cpp'):
            with open('../lib/DrawOwm/' + source_file, "r") as file:
                for line in file:
                    match = re.search(r'.*(TXT_\w++).*',line)
                    if match == None:
                        match = re.search(r'.*(LC_\w++).*',line)
                    if not match == None:
                        variable = match.group(1)
                        if variable in ignore_variables:
                            continue
                        if not variable in variables:
                            variables[variable] = ''
    #print(f'Variables found in source files:')
    #for variable in variables:
    #    print(variable)

    # get the english versions of the variables that were found
    with open('locales/locale_en_US.inc', "r") as file:
        for line in file:
            #print(line)
            match = re.search(r'const char \*(TXT_\w++).*\s*"(.+)"',line)
            if match == None:
                match = re.search(LC_ABMON_re,line)
                if match != None:
                # LC_ABMON is an array
                    variable = match.group(1)
                    variables[variable] = clean_value(match.group(2))
                    print(f'Set {variable} to {variables[variable]}')
                    continue

            if match == None:
                match = re.search(LC_re,line)
                if not match == None:
                    print(f"matched {match.group(1)} = '{match.group(2)}'")
            if not match == None:
                variable = match.group(1)
                if not variable in variables:
                    #print(f'Ignoring unreferenced variable {variable}')
                    continue
                if variable in ignore_variables:
                    continue
                value = match.group(2)
                value = clean_value(value)
                print(f"'{variable}' = '{value}'")
                if not variable in variables:
                    print(f'Error: "%s" not found in variables')
                    break
                variables[variable] = clean_value(value)
                continue

    # create list of keyworks

    print(f'Variables with english values:')
    for variable in variables:
        print(f'{variable} = {variables[variable]}')
        #print(f'{variable}')

    en_vars = variables
    with open('languages.json', 'r', encoding='utf-8') as file:
        lang_json = json.load(file)
    #print('initial json data:')
    #print(lang_json)

    languages = []

    # read the rest of the locale files and find the variables that are
    # different than the english values
    locale_files = sorted(os.listdir('locales'))
    for file_name in locale_files:
        variables = {}
        language = ''
        with open('locales/' + file_name, "r") as file:
            for line in file:
                #print(line)
                match = re.search(r'^.*OWM_LANG.*"([^"]+)"',line)
                if not match == None:
                    language = match.group(1)
                    print(f'language: {language}')
                    if language in languages:
                        print(f'Ignoring {file_name}, already have {language}')
                        break;
                    else:
                        languages.append(language)
                    continue
                match = re.search(r'const char \*(TXT_\w++).*\s*"(.+)"',line)
                if match == None:
                    match = re.search(LC_ABMON_re,line)
                    if match != None:
                    # LC_ABMON is an array
                        variable = match.group(1)
                        variables[variable] = clean_value(match.group(2))
                        print(f'Set {variable} to {variables[variable]}')
                        continue
                if match == None:
                    match = re.search(LC_re,line)
                if not match == None:
                    variable = match.group(1)
                    if not variable in en_vars:
                    #   print(f'Ignoring unreferenced variable {variable}')
                        continue
                    if variable in ignore_variables:
                        continue
                    value = match.group(2)
                    #print(f'{variable} = {value}')
                    if not variable in en_vars:
                        print(f'Error: "%s" not found in variables')
                        break
                    if value == en_vars[variable]:
                        #print(f'{variable}, same as english value')
                        pass
                    else:
                        variables[variable] = clean_value(value)
            if len(variables):
                #print(f'Variables in {file_name} with different values than english:')
                #for variable in variables:
                #    print(f'{variable}, {en_vars[variable]} -> {variables[variable]}')
                for index in lang_json.keys():
                    #print(f'testing index {index} "{lang_json[index]["language"]} == {language} ?')
                    if lang_json[index]['language'] == language:
                        #print(f'Found {language} at index {index}')
                        for variable in variables:
                            lang_json[index][variable] = variables[variable]

    formatter = Formatter()
    formatter.max_inline_complexity = 10
    formatter.max_inline_length = 75
    formatter.json_eol_style = EolStyle.LF
    formatter.indent_spaces = 2
    formatter.dump(lang_json, output_file="new.json", newline_at_eof=True)
    #with open('new.json', 'w') as file:
        #json.dump(lang_json, file, ensure_ascii = False,indent=2)

generate_locales()

