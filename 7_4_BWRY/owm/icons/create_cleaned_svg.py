#!/usr/bin/env fontforge

# Written by Google's Gemini using just chrome

# https://share.google/aimode/8rBPJvcerVY1kexiw

# Conversation:
# Initial Issue: You stated that an SVG looks different when viewed in 
# FontForge compared to Inkscape.
# Clarification: You specified that you were importing the SVG form of a 
# Google Material font.
# Feature Request: You asked for a Python script for FontForge to handle the 
# import and correction of the SVG file automatically.
# Error 1: You reported receiving the error: TypeError: 'str' object cannot be 
# interpreted as an integer.
# Error 2: You reported receiving the error: TypeError: Bad PostScript 
# import flag list, must be a single string or a sequence (tuple/list) of strings.
# Information Request: You asked what the string versions of the legacy integer 
# import flags 16 and 64 are.
# Error 3: You reported receiving the error: ValueError: Unknown PostScript 
# import flag "clear".
# Error 4: You reported receiving the error: AttributeError: module 'fontforge' 
# has no attribute 'psMat'.
# List Request: You requested a list of your questions up to that point.
# Output Change: You requested to change the output to a compiled .ttf file 
# instead of an .sfd project file.
# Layout Issue: You noted that the imported SVG was not centering correctly.
# Visual Issue: You reported that there was a solid black box artifact 
# appearing on the icon.
# Error 5 / Correction: You noted that the manual string filter was skipped 
# because the string version variation "is not present" inside the file code.
# Error 6: You reported receiving the error: TypeError: 'fontforge.contour' 
# object cannot be interpreted as an integer.
# Visual Issue: You noted that the black box was still present after updating 
# the contour canvas layer loop.
# Bulk Request: You requested an expanded version of the script to batch-process 
# a complete folder of SVGs.
# Feature Mod: You requested to adjust the folder script so it will save the 
# cleaned SVG file iterations permanently rather than deleting them.
# Feature Request: You requested to expand the script to generate a layout 
# mapping sheet (HTML cheat sheet) for the compiled font.Current 

# Now that the script batch-processes the folder, fixes the centering, 
# strips the artifacts, exports a TTF, an HTML cheat sheet and 
# permanently saves the cleaned SVGs, 

import psMat
import os
import re
import generate_html_mapping_sheet

def clean_raw_svg_text(svg_path, output_folder):
    """
    Reads the raw SVG code, strips out the Google Material background box,
    and permanently saves the cleaned SVG file to the output folder.
    """
    if not os.path.exists(svg_path):
        return None

    with open(svg_path, 'r', encoding='utf-8') as f:
        content = f.read()

    patterns = [
        r'<path[^>]*fill=["\']none["\'][^>]*/>',
        r'<path[^>]*d=["\']M0\s+0h24v24H0z["\'][^>]*/>',
        r'<path[^>]*d=["\']M0\s+0h24v24h-24z["\'][^>]*/>',
        r'<rect[^>]*fill=["\']none["\'][^>]*/>',
        r'<rect[^>]*width=["\'](?:24|48)["\'][^>]*/>'
    ]

    cleaned_content = content
    for pattern in patterns:
        cleaned_content = re.sub(pattern, '', cleaned_content, flags=re.IGNORECASE)

    filename = os.path.basename(svg_path)
    cleaned_svg_path = os.path.join(output_folder, filename)
    
    with open(cleaned_svg_path, 'w', encoding='utf-8') as f:
        f.write(cleaned_content)
        
    return cleaned_svg_path


def batch_import_svg_folder(input_folder, cleaned_svg_folder="cleaned_svgs", output_ttf_name="material_icons_batch.ttf", start_unicode=0xE000):
    if not os.path.isdir(input_folder):
        print(f"Error: Input folder path not found: {input_folder}")
        return

    if not os.path.exists(cleaned_svg_folder):
        os.makedirs(cleaned_svg_folder)

    font = fontforge.font()
    font.encoding = "UnicodeFull"
    font.em = 1000 
    
    font.fontname = "MaterialCustomIcons"
    font.fullname = "Material Custom Icons Pack"
    font.familyname = "Material Custom Icons"

    current_unicode = start_unicode
    
    # This list will keep track of glyph data for our HTML report
    mapping_records = []

    svg_files = sorted([f for f in os.listdir(input_folder) if f.lower().endswith('.svg')])

    if not svg_files:
        print(f"No SVG files found inside target directory: {input_folder}")
        return

    print(f"Processing {len(svg_files)} files...")

    for filename in svg_files:
        svg_full_path = os.path.join(input_folder, filename)
        clean_glyph_name = os.path.splitext(filename)[0]

        processed_svg = clean_raw_svg_text(svg_full_path, cleaned_svg_folder)
        if not processed_svg:
            continue

        glyph = font.createChar(current_unicode, clean_glyph_name)
        glyph.importOutlines(processed_svg, clear=True, scale=False)

        glyph.removeOverlap()      
        glyph.correctDirection()    

        scale_matrix = psMat.scale(38.0)
        glyph.transform(scale_matrix)
        
        bbox = glyph.boundingBox()
        xmin, ymin, xmax, ymax = bbox[0], bbox[1], bbox[2], bbox[3]
        
        icon_width = xmax - xmin
        icon_height = ymax - ymin
        
        total_slot_width = 1000 
        font_ascender_height = 800  
        
        target_xmin = (total_slot_width - icon_width) / 2
        target_ymin = (font_ascender_height - icon_height) / 2
        
        shift_x = target_xmin - xmin
        shift_y = target_ymin - ymin
        
        translation_matrix = psMat.translate(shift_x, shift_y)
        glyph.transform(translation_matrix)
        glyph.width = total_slot_width

        # Record mapping details (name, hex value string like '0xe000')
        mapping_records.append((clean_glyph_name, hex(current_unicode)))
        
        current_unicode += 1

    # Save TTF Font file
    font.generate(cleaned_svg_folder + '/' + output_ttf_name)

    print(f"Successfully compiled icons into {output_ttf_name}.")
    
    # Generate the Layout Mapping sheet using our logged data
    generate_html_mapping_sheet.generate_html_mapping_sheet(mapping_records, output_ttf_name,cleaned_svg_folder + '/' + 'map.html')

# --- EXECUTION ---
if __name__ == "__main__":
    TARGET_INPUT_FOLDER = "./material_svg" # Folder with your original Google SVGs
    CLEANED_OUTPUT_FOLDER = "./cleaned_material_svg"   # Folder where you want to keep the fixed files
    
    batch_import_svg_folder(TARGET_INPUT_FOLDER, cleaned_svg_folder=CLEANED_OUTPUT_FOLDER)

