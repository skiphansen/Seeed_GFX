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

name2unicode = {
"wi_wind_beaufort_0": 0xf0b7,
"wi_wind_beaufort_1": 0xf0b8,
"wi_wind_beaufort_2": 0xf0b9,
"wi_wind_beaufort_3": 0xf0ba,
"wi_wind_beaufort_4": 0xf0bb,
"wi_wind_beaufort_5": 0xf0bc,
"wi_wind_beaufort_6": 0xf0bd,
"wi_wind_beaufort_7": 0xf0be,
"wi_wind_beaufort_8": 0xf0bf,
"wi_wind_beaufort_9": 0xf0c0,
"wi_wind_beaufort_10": 0xf0c1,
"wi_wind_beaufort_11": 0xf0c2,
"wi_wind_beaufort_12": 0xf0c3,
"wi_day_sunny": 0xf00d,
"wi_day_cloudy": 0xf002,
"wi_day_cloudy_gusts": 0xf000,
"wi_day_cloudy_windy": 0xf001,
"wi_day_fog": 0xf003,
"wi_day_hail": 0xf004,
"wi_day_haze": 0xf0b6,
"wi_day_lightning": 0xf005,
"wi_day_rain": 0xf008,
"wi_day_rain_mix": 0xf006,
"wi_day_rain_wind": 0xf007,
"wi_day_showers": 0xf009,
"wi_day_sleet": 0xf0b2,
"wi_day_sleet_storm": 0xf068,
"wi_day_snow": 0xf00a,
"wi_day_snow_thunderstorm": 0xf06b,
"wi_day_snow_wind": 0xf065,
"wi_day_sprinkle": 0xf00b,
"wi_day_storm_showers": 0xf00e,
"wi_day_sunny_overcast": 0xf00c,
"wi_day_thunderstorm": 0xf010,
"wi_day_windy": 0xf085,
"wi_solar_eclipse": 0xf06e,
"wi_hot": 0xf072,
"wi_day_cloudy_high": 0xf07d,
"wi_day_light_wind": 0xf0c4,
"wi_direction_up": 0xf058,
"wi_direction_up_right": 0xf057,
"wi_direction_right": 0xf04d,
"wi_direction_down_right": 0xf088,
"wi_direction_down": 0xf044,
"wi_direction_down_left": 0xf043,
"wi_direction_left": 0xf048,
"wi_direction_up_left": 0xf087,
"wi_alien": 0xf075,
"wi_celsius": 0xf03c,
"wi_fahrenheit": 0xf045,
"wi_degrees": 0xf042,
"wi_thermometer": 0xf055,
"wi_thermometer_exterior": 0xf053,
"wi_thermometer_internal": 0xf054,
"wi_cloud_down": 0xf03d,
"wi_cloud_up": 0xf040,
"wi_cloud_refresh": 0xf03e,
"wi_horizon": 0xf047,
"wi_horizon_alt": 0xf046,
"wi_sunrise": 0xf051,
"wi_sunset": 0xf052,
"wi_moonrise": 0xf0c9,
"wi_moonset": 0xf0ca,
"wi_refresh": 0xf04c,
"wi_refresh_alt": 0xf04b,
"wi_umbrella": 0xf084,
"wi_barometer": 0xf079,
"wi_humidity": 0xf07a,
"wi_na": 0xf07b,
"wi_train": 0xf0cb,
"wi_moon_new": 0xf095,
"wi_moon_waxing_crescent_1": 0xf096,
"wi_moon_waxing_crescent_2": 0xf097,
"wi_moon_waxing_crescent_3": 0xf098,
"wi_moon_waxing_crescent_4": 0xf099,
"wi_moon_waxing_crescent_5": 0xf09a,
"wi_moon_waxing_crescent_6": 0xf09b,
"wi_moon_first_quarter": 0xf09c,
"wi_moon_waxing_gibbous_1": 0xf09d,
"wi_moon_waxing_gibbous_2": 0xf09e,
"wi_moon_waxing_gibbous_3": 0xf09f,
"wi_moon_waxing_gibbous_4": 0xf0a0,
"wi_moon_waxing_gibbous_5": 0xf0a1,
"wi_moon_waxing_gibbous_6": 0xf0a2,
"wi_moon_full": 0xf0a3,
"wi_moon_waning_gibbous_1": 0xf0a4,
"wi_moon_waning_gibbous_2": 0xf0a5,
"wi_moon_waning_gibbous_3": 0xf0a6,
"wi_moon_waning_gibbous_4": 0xf0a7,
"wi_moon_waning_gibbous_5": 0xf0a8,
"wi_moon_waning_gibbous_6": 0xf0a9,
"wi_moon_third_quarter": 0xf0aa,
"wi_moon_waning_crescent_1": 0xf0ab,
"wi_moon_waning_crescent_2": 0xf0ac,
"wi_moon_waning_crescent_3": 0xf0ad,
"wi_moon_waning_crescent_4": 0xf0ae,
"wi_moon_waning_crescent_5": 0xf0af,
"wi_moon_waning_crescent_6": 0xf0b0,
"wi_moon_alt_new": 0xf0eb,
"wi_moon_alt_waxing_crescent_1": 0xf0d0,
"wi_moon_alt_waxing_crescent_2": 0xf0d1,
"wi_moon_alt_waxing_crescent_3": 0xf0d2,
"wi_moon_alt_waxing_crescent_4": 0xf0d3,
"wi_moon_alt_waxing_crescent_5": 0xf0d4,
"wi_moon_alt_waxing_crescent_6": 0xf0d5,
"wi_moon_alt_first_quarter": 0xf0d6,
"wi_moon_alt_waxing_gibbous_1": 0xf0d7,
"wi_moon_alt_waxing_gibbous_2": 0xf0d8,
"wi_moon_alt_waxing_gibbous_3": 0xf0d9,
"wi_moon_alt_waxing_gibbous_4": 0xf0da,
"wi_moon_alt_waxing_gibbous_5": 0xf0db,
"wi_moon_alt_waxing_gibbous_6": 0xf0dc,
"wi_moon_alt_full": 0xf0dd,
"wi_moon_alt_waning_gibbous_1": 0xf0de,
"wi_moon_alt_waning_gibbous_2": 0xf0df,
"wi_moon_alt_waning_gibbous_3": 0xf0e0,
"wi_moon_alt_waning_gibbous_4": 0xf0e1,
"wi_moon_alt_waning_gibbous_5": 0xf0e2,
"wi_moon_alt_waning_gibbous_6": 0xf0e3,
"wi_moon_alt_third_quarter": 0xf0e4,
"wi_moon_alt_waning_crescent_1": 0xf0e5,
"wi_moon_alt_waning_crescent_2": 0xf0e6,
"wi_moon_alt_waning_crescent_3": 0xf0e7,
"wi_moon_alt_waning_crescent_4": 0xf0e8,
"wi_moon_alt_waning_crescent_5": 0xf0e9,
"wi_moon_alt_waning_crescent_6": 0xf0ea,
"wi_cloud": 0xf041,
"wi_cloudy": 0xf013,
"wi_cloudy_gusts": 0xf011,
"wi_cloudy_windy": 0xf012,
"wi_fog": 0xf014,
"wi_hail": 0xf015,
"wi_rain": 0xf019,
"wi_rain_mix": 0xf017,
"wi_rain_wind": 0xf018,
"wi_showers": 0xf01a,
"wi_sleet": 0xf0b5,
"wi_snow": 0xf01b,
"wi_sprinkle": 0xf01c,
"wi_storm_showers": 0xf01d,
"wi_thunderstorm": 0xf01e,
"wi_snow_wind": 0xf064,
"wi_snow": 0xf01b,
"wi_smog": 0xf074,
"wi_smoke": 0xf062,
"wi_lightning": 0xf016,
"wi_raindrops": 0xf04e,
"wi_raindrop": 0xf078,
"wi_dust": 0xf063,
"wi_snowflake_cold": 0xf076,
"wi_windy": 0xf021,
"wi_strong_wind": 0xf050,
"wi_sandstorm": 0xf082,
"wi_earthquake": 0xf0c6,
"wi_fire": 0xf0c7,
"wi_flood": 0xf07c,
"wi_meteor": 0xf071,
"wi_tsunami": 0xf0c5,
"wi_volcano": 0xf0c8,
"wi_hurricane": 0xf073,
"wi_tornado": 0xf056,
"wi_small_craft_advisory": 0xf0cc,
"wi_gale_warning": 0xf0cd,
"wi_storm_warning": 0xf0ce,
"wi_hurricane_warning": 0xf0cf,
"wi_wind_direction": 0xf0b1,
"wi_night_clear": 0xf02e,
"wi_night_alt_cloudy": 0xf086,
"wi_night_alt_cloudy_gusts": 0xf022,
"wi_night_alt_cloudy_windy": 0xf023,
"wi_night_alt_hail": 0xf024,
"wi_night_alt_lightning": 0xf025,
"wi_night_alt_rain": 0xf028,
"wi_night_alt_rain_mix": 0xf026,
"wi_night_alt_rain_wind": 0xf027,
"wi_night_alt_showers": 0xf029,
"wi_night_alt_sleet": 0xf0b4,
"wi_night_alt_sleet_storm": 0xf06a,
"wi_night_alt_snow": 0xf02a,
"wi_night_alt_snow_thunderstorm": 0xf06d,
"wi_night_alt_snow_wind": 0xf067,
"wi_night_alt_sprinkle": 0xf02b,
"wi_night_alt_storm_showers": 0xf02c,
"wi_night_alt_thunderstorm": 0xf02d,
"wi_night_cloudy": 0xf031,
"wi_night_cloudy_gusts": 0xf02f,
"wi_night_cloudy_windy": 0xf030,
"wi_night_fog": 0xf04a,
"wi_night_hail": 0xf032,
"wi_night_lightning": 0xf033,
"wi_night_partly_cloudy": 0xf083,
"wi_night_rain": 0xf036,
"wi_night_rain_mix": 0xf034,
"wi_night_rain_wind": 0xf035,
"wi_night_showers": 0xf037,
"wi_night_sleet": 0xf0b3,
"wi_night_sleet_storm": 0xf069,
"wi_night_snow": 0xf038,
"wi_night_snow_thunderstorm": 0xf06c,
"wi_night_snow_wind": 0xf066,
"wi_night_sprinkle": 0xf039,
"wi_night_storm_showers": 0xf03a,
"wi_night_thunderstorm": 0xf03b,
"wi_lunar_eclipse": 0xf070,
"wi_stars": 0xf077,
"wi_storm_showers": 0xf01d,
"wi_thunderstorm": 0xf01e,
"wi_night_alt_cloudy_high": 0xf07e,
"wi_night_cloudy_high": 0xf080,
"wi_night_alt_partly_cloudy": 0xf081,
"wi_time_1": 0xf08a,
"wi_time_2": 0xf08b,
"wi_time_3": 0xf08c,
"wi_time_4": 0xf08d,
"wi_time_5": 0xf08e,
"wi_time_6": 0xf08f,
"wi_time_7": 0xf090,
"wi_time_8": 0xf091,
"wi_time_9": 0xf092,
"wi_time_10": 0xf093,
"wi_time_11": 0xf094,
"wi_time_12": 0xf089,
"wi_n": 0xf0b1,
"wi_nne": 0xf0b1,
"wi_ne": 0xf0b1,
"wi_ene": 0xf0b1,
"wi_e": 0xf0b1,
"wi_ese": 0xf0b1,
"wi_se": 0xf0b1,
"wi_sse": 0xf0b1,
"wi_s": 0xf0b1,
"wi_ssw": 0xf0b1,
"wi_sw": 0xf0b1,
"wi_wsw": 0xf0b1,
"wi_w": 0xf0b1,
"wi_wnw": 0xf0b1,
"wi_nw": 0xf0b1,
"wi_nnw": 0xf0b1,
"wi_n": 0xf0b1,
"wi_nne": 0xf0b1,
"wi_ne": 0xf0b1,
"wi_ene": 0xf0b1,
"wi_e": 0xf0b1,
"wi_ese": 0xf0b1,
"wi_se": 0xf0b1,
"wi_sse": 0xf0b1,
"wi_s": 0xf0b1,
"wi_ssw": 0xf0b1,
"wi_sw": 0xf0b1,
"wi_wsw": 0xf0b1,
"wi_w": 0xf0b1,
"wi_wnw": 0xf0b1,
"wi_nw": 0xf0b1,
"wi_nnw": 0xf0b1
}

def generate_icons_h(directory):
    icons_h_content = []
    
    # Start the header file
    icons_h_content.append('// DO NOT MODIFY -- THIS FILE WAS GENERATED BY `python3 final_generate_icons_h.py`')
    icons_h_content.append('')
    icons_h_content.append('#ifndef __ICONS_H__')
    icons_h_content.append('#define __ICONS_H__')
    icons_h_content.append('')
    icons_h_content.append('#include <cstddef>')
    icons_h_content.append('#include "icons/icons.inc"')
    icons_h_content.append('')

    sizes = []
    icon_names = []
    full_names = []

    # Loop through all directories in 'icons'
    for size_dir in sorted(os.listdir(directory)):
        size_path = os.path.join(directory, size_dir)

        if os.path.isdir(size_path):
            size = size_dir.split('x')[0]  # Assume format is SIZExSIZE (e.g., 16x16)
            sizes.append(int(size))

    for size_dir in sorted(os.listdir(directory)):
        size_path = os.path.join(directory, size_dir)
        if not os.path.isdir(size_path):
            print(f'skipping {size_path}')
            continue

        size = size_dir.split('x')[0]  # Assume format is SIZExSIZE (e.g., 16x16)
        icons = sorted(os.listdir(size_path))
        for icon in icons:
            if icon.endswith(f"_{size}x{size}.h"):
                full_name = icon.split(f".h")[0]
                full_names.append(full_name)

                icon_name = icon.split(f"_{size}x{size}.h")[0]
                if not icon_name in icon_names:
                    icon_names.append(icon_name)

    sizes.sort()

    f_inc = open('icons/icons.inc',"w")

    # make externs 

    # make enums

    AllIcons = {}
    last_codepoint = 0
    for icon_name in icon_names:
        if icon_name in name2unicode:
            codepoint = name2unicode[icon_name]
            AllIcons[icon_name] = f'0x{codepoint:04x}'
            if codepoint > last_codepoint:
                last_codepoint = codepoint
        else:
            AllIcons[icon_name] = ''


    print(f'last_codepoint 0x{last_codepoint:04x}')
    # assign codepoint to remaining icons
    for icon_name in icon_names:
        if icon_name in name2unicode:
            pass
        else:
            last_codepoint += 1
            AllIcons[icon_name] = f'0x{last_codepoint:04x}'

    all_icons_sorted =  dict(sorted(AllIcons.items(), key=lambda item: item[1]))

    print(f'AllIcons {all_icons_sorted}')

    icons_h_content.append('typedef enum icon_name {')
    for icon_name in all_icons_sorted:
        icons_h_content.append(f'  {icon_name} = {all_icons_sorted[icon_name]},')
    icons_h_content.append('} icon_name_t;')
    icons_h_content.append('')

    # define the constexpr function
    icons_h_content.append('constexpr const unsigned char* getBitmap(icon_name_t icon, size_t size)')
    icons_h_content.append('{')
    icons_h_content.append('  switch (icon) {')
    for icon_name in icon_names:
        icons_h_content.append(f'  case {icon_name}:')
        icons_h_content.append('    switch (size) {')
        for size in sizes:
            test_name = f'{icon_name}_{size}x{size}'
            if test_name in full_names:
                icons_h_content.append(f'    case {size}: return {test_name};')
                test_path = f'icons/{size}x{size}/{test_name}.h'
                with open(test_path, "r") as f_header:
                    content = f_header.read()
                    f_inc.write(content)

        icons_h_content.append('    default:')
        icons_h_content.append('      return nullptr;')
        icons_h_content.append('    }')
    
    # Close the function and the header guard
    icons_h_content.append('  default:')
    icons_h_content.append('    return nullptr;')
    icons_h_content.append('  }')
    icons_h_content.append('}')
    icons_h_content.append('')
    icons_h_content.append('#endif')
    
    # Write the result to the icons.h file
    with open("icons/icons.h", "w") as f:
        f.write("\n".join(icons_h_content))
    print("icons.h and icons.inc files have been generated")
    f_inc.close()


generate_icons_h('icons')
