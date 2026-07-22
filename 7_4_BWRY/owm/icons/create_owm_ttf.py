#!/usr/bin/env python3

import fontforge
import psMat

name2unicode = {
}

def create_ttf_from_svg():
    # 1. Create a new font container
    font = fontforge.font()
    
    # 2. Set font metadata
    font.fontname = "MyCustomFont"
    font.fullname = "My Custom Font"
    font.familyname = "MyCustomFont"
    
    # 3. Map your SVGs to specific character codes
    glyphs_to_map = {
        0xf0ec: "wifi.svg",
        0xf0ed: "wifi_1_bar.svg",
        0xf0ee: "wifi_2_bar.svg",
        0xf0ef: "wifi_3_bar.svg",
        0xf0f0: "wifi_x.svg",
        0xf0f1: "air_filter.svg",
        0xf0f2: "battery_0_bar_90deg.svg",
        0xf0f3: "battery_1_bar_90deg.svg",
        0xf0f4: "battery_2_bar_90deg.svg",
        0xf0f5: "battery_3_bar_90deg.svg",
        0xf0f6: "battery_4_bar_90deg.svg",
        0xf0f7: "battery_5_bar_90deg.svg",
        0xf0f8: "battery_6_bar_90deg.svg",
        0xf0f9: "battery_full_90deg.svg",
        0xf0fa: "error_icon.svg",
        0xf0fb: "house_humidity.svg",
        0xf0fc: "house_thermometer.svg",
        0xf0fd: "visibility_icon.svg",
        0xf0fe: "wind_direction_meteorological_0deg.svg",
        0xf0ff: "wind_direction_meteorological_112_5deg.svg",
        0xf100: "wind_direction_meteorological_135deg.svg",
        0xf101: "wind_direction_meteorological_157_5deg.svg",
        0xf102: "wind_direction_meteorological_180deg.svg",
        0xf103: "wind_direction_meteorological_202_5deg.svg",
        0xf104: "wind_direction_meteorological_225deg.svg",
        0xf105: "wind_direction_meteorological_22_5deg.svg",
        0xf106: "wind_direction_meteorological_247_5deg.svg",
        0xf107: "wind_direction_meteorological_270deg.svg",
        0xf108: "wind_direction_meteorological_292_5deg.svg",
        0xf109: "wind_direction_meteorological_315deg.svg",
        0xf10a: "wind_direction_meteorological_337_5deg.svg",
        0xf10b: "wind_direction_meteorological_45deg.svg",
        0xf10c: "wind_direction_meteorological_67_5deg.svg",
        0xf10d: "wind_direction_meteorological_90deg.svg",
        0xf10e: "biological_hazard_symbol.svg",
        0xf10f: "ionizing_radiation_symbol.svg",
        0xf110: "warning_icon.svg",
        0xf111: "battery_alert_0deg.svg"
    }
    
    # Select all glyphs in the font
    #font.selection.all()

    offset_matrix = psMat.translate(-500,0)
    # AutoWidth takes a spacing value (in em-units)
    # Setting to 0 or negative em-size defaults to a standard built-in spacing
    #font.autoWidth(0) 
    for unicode_dec, svg_path in glyphs_to_map.items():
        # Create a glyph slot using the Unicode value
        glyph = font.createChar(unicode_dec)
        # Import the SVG vector outlines into the slot
        glyph.importOutlines('svg/' + svg_path)
        #glyph.left_side_bearing = 0
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
        offset_matrix = psMat.translate(-500,0)
        glyph.transform((1, 0, 0, 1, shift_x, shift_y))
        glyph.removeOverlap()
        glyph.correctDirection()

    # 4. Generate and save the final TTF file
    font.generate("owm_icons.ttf")
    print("TTF font generated successfully!")

if __name__ == "__main__":
    create_ttf_from_svg()

