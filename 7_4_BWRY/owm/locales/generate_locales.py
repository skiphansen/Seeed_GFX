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

    for locale_file in locale_files:
        with open('locales/' + locale_file, "r") as file:
            print(f'File {locale_file}:')
            for line in file:
                language = re.search("OWM_LANG",line)
                if not language == None:
                    print(line)




generate_locales()

