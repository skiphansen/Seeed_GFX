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

def generate_locales():
    
    # Start the header file

    locale_files = sorted(os.listdir('locales'))

    # create list of languages
    languages = {}
    variables = []
    bFirst = True
    for locale_file in locale_files:
        with open('locales/' + locale_file, "r") as file:
            print(f'Parsing {locale_file}')
            language = ""
            for line in file:
                match = re.search(r'^.*OWM_LANG.*"([^"]+)"',line)
                if not match == None:
                    language = match.group(1)
                    if language in languages.keys():
                        print(f'Ignoring {locale_file}, already have {language}')
                    else:
                        languages[language] = {}
                    break;

            file.seek(0)
            for line in file:
                match = re.search(r'HTTP|_WL_|WIFI|DESERIALIZATION',line)
                if not match == None:
                #ignore HTTP status, we don't need them
                    continue
                match = re.search(r'const char \*(TXT_[^\s]+).*\s*"(.+);',line)
                
                if not match == None:
                    variable = match.group(1)
                    value = '"' + match.group(2)
                    print(f'{variable} = {value}')
                    if not variable in variables:
                        if not bFirst:
                            print(f'Found new variable "{variable}" in {locale_file}')
                        variables.append(variable)
                    print(f'language: {language}')
                    languages[language][variable] = value
        bFirst = False
    print(f'Languages: {languages}')
    # create list of keyworks


generate_locales()

