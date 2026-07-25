import os

def generate_html_mapping_sheet(mapping_data, ttf_filename, html_filename):
    """
    Generates a beautifully styled standalone HTML layout map showcasing 
    all processed icons, their names, and hex codes.
    """
    html_content = f"""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Font Layout Mapping Sheet</title>
    <style>
        @font-face {{
            font-family: 'MaterialCustomIcons';
            src: url('{ttf_filename}') format('truetype');
        }}
        body {{
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
            background-color: #f4f5f7;
            color: #333;
            padding: 40px;
        }}
        h1 {{ text-align: center; margin-bottom: 5px; color: #111; }}
        .subtitle {{ text-align: center; color: #666; margin-bottom: 40px; }}
        .grid {{
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(150px, 1fr));
            gap: 20px;
            max-width: 1200px;
            margin: 0 auto;
        }}
        .card {{
            background: #fff;
            border-radius: 8px;
            padding: 20px;
            text-align: center;
            box-shadow: 0 2px 4px rgba(0,0,0,0.04);
            transition: transform 0.2s, box-shadow 0.2s;
        }}
        .card:hover {{
            transform: translateY(-2px);
            box-shadow: 0 4px 8px rgba(0,0,0,0.08);
        }}
        .icon {{
            font-family: 'MaterialCustomIcons';
            font-size: 48px;
            color: black;
            margin-bottom: 15px;
            height: 50px;
            line-height: 50px;
        }}
        .name,.hex {{
            font-size: 14px;
            font-weight: 600;
            word-break: break-word;
            margin-bottom: 5px;
            color: #222;
        }}
    </style>
</head>
<body>

    <h1>Font Layout Mapping Sheet</h1>
    <p class="subtitle">Generated dynamically from FontForge project</p>

    <div class="grid">
"""
    for name, hex_code in mapping_data:
        # Convert hex string safely to a browser-readable HTML entity reference
        html_entity = f"&#x{hex_code[2:].upper()};"
        html_content += f"""        <div class="card">
            <div class="icon">{html_entity}</div>
            <div class="name">{name}</div>
            <div class="hex">{hex_code.upper()}</div>
        </div>\n"""

    html_content += """    </div>
</body>
</html>"""

    with open(html_filename, 'w', encoding='utf-8') as f:
        f.write(html_content)
    print(f"Generated Layout Mapping Sheet at: {os.path.abspath(html_filename)}")

