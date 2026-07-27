#!/usr/bin/env python3

import fontforge
import psMat
import generate_html_mapping_sheet
import os
from fontTools.ttLib import TTFont

name2unicode = {
}

def print_bounds(name,bounds,new_bounds):
    orig_width = round(bounds[2] - bounds[0])
    new_width = round(new_bounds[2] - new_bounds[0])
    orig_height = round(bounds[3] - bounds[1])
    new_height = round(new_bounds[3] - new_bounds[1])
    orig_xmin = round(bounds[0])
    orig_xmax = round(bounds[2])
    orig_ymin = round(bounds[1])
    orig_ymax = round(bounds[3])
    new_xmin = round(new_bounds[0])
    new_xmax = round(new_bounds[2])
    new_ymin = round(new_bounds[1])
    new_ymax = round(new_bounds[3])
    center_x = round(new_xmin + (new_xmax - new_xmin) / 2)
    center_y = round(new_ymin + (new_ymax - new_ymin) / 2)

    print(f'size {new_width}x{new_height} center {center_x},{center_y} {name}:')
    if orig_width != new_width or orig_height != new_height:
        print(f'  {orig_width}x{orig_height} -> {new_width}x{new_height}')
    if orig_xmin != new_xmin:
        print(f'  xmin {orig_xmin} -> {new_xmin}')
    if orig_xmax != new_xmax:
        print(f'  xmax {orig_xmax} -> {new_xmax}')
    if orig_ymin != new_ymin:
        print(f'  ymin {orig_ymin} -> {new_ymin}')
    if orig_ymax != new_ymax:
        print(f'  ymax {orig_ymax} -> {new_ymax}')
    print('')

def create_ttf_from_svg():
    # 1. Create a new font container
    font = fontforge.font()
    
    # 2. Set font metadata
    font.fontname = "OwmIcons"
    font.fullname = font.fontname
    font.familyname = font.fontname
    
    # 3. Map your SVGs to specific character codes
    glyphs_to_map = {
        0xe63e: "wifi",
        0xe4ca: "wifi_1_bar",
        0xe4d9: "wifi_2_bar",
        0xebe1: "wifi_3_bar",
        0xf0f0: "wifi_x",
        0xf0f1: "air_filter",
        0xf0fa: "error_icon",
        0xf0fb: "house_humidity",
        0xf0fc: "house_thermometer",
        0xf0fe: "wind_direction_meteorological_0deg",
        0xf0ff: "wind_direction_meteorological_112_5deg",
        0xf100: "wind_direction_meteorological_135deg",
        0xf101: "wind_direction_meteorological_157_5deg",
        0xf102: "wind_direction_meteorological_180deg",
        0xf103: "wind_direction_meteorological_202_5deg",
        0xf104: "wind_direction_meteorological_225deg",
        0xf105: "wind_direction_meteorological_22_5deg",
        0xf106: "wind_direction_meteorological_247_5deg",
        0xf107: "wind_direction_meteorological_270deg",
        0xf108: "wind_direction_meteorological_292_5deg",
        0xf109: "wind_direction_meteorological_315deg",
        0xf10a: "wind_direction_meteorological_337_5deg",
        0xf10b: "wind_direction_meteorological_45deg",
        0xf10c: "wind_direction_meteorological_67_5deg",
        0xf10d: "wind_direction_meteorological_90deg",
        0xf10e: "biological_hazard_symbol",
        0xf10f: "ionizing_radiation_symbol",
        0xf110: "warning_icon",
    }

    battery_glyphs_to_map = {
        0xebdc: "battery_0_bar_90deg",
        0xebd9: "battery_1_bar_90deg",
        0xebe0: "battery_2_bar_90deg",
        0xebdd: "battery_3_bar_90deg",
        0xebe2: "battery_4_bar_90deg",
        0xebd4: "battery_5_bar_90deg",
        0xebd2: "battery_6_bar_90deg",
        0xe1a4: "battery_full_90deg",
        0xe8f4: "visibility_icon"
    }

    max_width = 0
    max_height = 0

    mapping_records = []

    glyphs_to_add = glyphs_to_map | battery_glyphs_to_map
    for unicode_dec, name in glyphs_to_add.items():
        # Create a glyph slot using the Unicode value
        mapping_records.append((name, hex(unicode_dec)))
        glyph = font.createChar(unicode_dec)
        glyph.glyphname = name
        # Import the SVG vector outlines into the slot
        svg_path = 'svg/' + name + '.svg'
        if not os.path.exists(svg_path):
            svg_path = 'cleaned_material_svg/' + name + '.svg'

        glyph.importOutlines(svg_path)
        #glyph.transform(offset_matrix)
        # 3. Determine the bounding box of the imported SVG 
        # (returns (xmin, ymin, xmax, ymax))
        bounds = glyph.boundingBox()

        # 4. Calculate the SVG's current center
        width = bounds[2] - bounds[0]
        height = bounds[3] - bounds[1]
        center_x = bounds[0] + (width / 2.0)
        center_y = bounds[1] + (height / 2.0)

        # 5. Determine where you want the shape centered in your em-square
        target_center_x = 500  # Horizontal center (assuming 1000 upem)
        target_center_y = 500  # Vertical center/baseline target

        # 6. Calculate the X/Y shift needed
        shift_x = target_center_x - center_x
        shift_y = target_center_y - center_y

        # 7. Apply the transformation and clean up overlaps
        glyph.transform((1, 0, 0, 1, shift_x, shift_y))
        glyph.removeOverlap()

        new_bounds = glyph.boundingBox()
        #print_bounds(name,bounds,new_bounds)
        #print(f'advancewidth {glyph.width}')
        width = round(bounds[2] - bounds[0])
        height = round(bounds[3] - bounds[1])
        left_side_bearing = 1000.0 - ((bounds[2] - bounds[0]) // 2.0)
        if 'wind_direction_meteorological' in name:
            glyph.left_side_bearing = 250
        else:
            glyph.left_side_bearing = 0

        # must reset width (advancewidth) since setting left_side_bearing changes it
        glyph.width = 2000

        if max_width < width:
            max_width = width
        if max_height < height:
            max_height = height

        glyph.correctDirection()

    print(f'max bounding box size {max_width}x{max_height}')
    # 4. Generate and save the final TTF file
    output_ttf_name = "owm_icons.ttf"
    font.generate(output_ttf_name)
    print("TTF font generated successfully!")
    generate_html_mapping_sheet.generate_html_mapping_sheet(mapping_records, output_ttf_name,'owm_icons_map.html')

if __name__ == "__main__":
    create_ttf_from_svg()

