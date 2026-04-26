import zipfile
import xml.etree.ElementTree as ET
import re
import sys

def col2num(col_str):
    num = 0
    for c in col_str.upper():
        num = num * 26 + (ord(c) - ord('A')) + 1
    return num

def convert_xlsx_to_csv(filepath, outpath):
    print(f"Reading {filepath}...")
    try:
        z = zipfile.ZipFile(filepath)
    except Exception as e:
        print(f"Error opening Excel file {filepath}: {e}")
        return
        
    shared_strings = []
    if 'xl/sharedStrings.xml' in z.namelist():
        sst = ET.fromstring(z.read('xl/sharedStrings.xml'))
        for si in sst:
            t = si.find('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}t')
            if t is not None and t.text:
                shared_strings.append(t.text)
            else:
                shared_strings.append("")

    sheet_xml = None
    try:
        sheet_xml = z.read('xl/worksheets/sheet1.xml')
    except:
        print("Could not find sheet1.xml in .xlsx file.")
        return

    sheet = ET.fromstring(sheet_xml)
    
    max_r = 0
    max_c = 0
    data = {}
    
    # Prefix for namespaces
    ns = {'main': 'http://schemas.openxmlformats.org/spreadsheetml/2006/main'}
    
    for row in sheet.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}row'):
        for c in row.iter('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}c'):
            r_attr = c.attrib.get('r')
            if not r_attr: continue
            
            match = re.match(r"([A-Z]+)([0-9]+)", r_attr)
            if not match: continue
            col_letters, row_num_str = match.groups()
            
            c_idx = col2num(col_letters) - 1
            r_idx = int(row_num_str) - 1
            
            max_c = max(max_c, c_idx)
            max_r = max(max_r, r_idx)
            
            v = c.find('{http://schemas.openxmlformats.org/spreadsheetml/2006/main}v')
            if v is not None and v.text is not None:
                val = v.text
                if c.attrib.get('t') == 's': 
                    val = shared_strings[int(val)]
                data[(r_idx, c_idx)] = val

    if max_r == 0 and max_c == 0 and len(data) == 0:
        print("Warning: Map data is empty or could not be parsed.")

    print(f"Detected map bounds: {max_c+1} columns by {max_r+1} rows.")
    
    with open(outpath, 'w', encoding='utf-8') as f:
        for r in range(max_r + 1):
            row_data = []
            for c in range(max_c + 1):
                val = data.get((r, c), "0")
                if val.strip() == "": val = "0"
                row_data.append(val)
            f.write(",".join(row_data) + "\n")
    print(f"Successfully wrote {outpath}")

if __name__ == "__main__":
    infile = sys.argv[1] if len(sys.argv) > 1 else "The-Jungle-Map.xlsx"
    outfile = sys.argv[2] if len(sys.argv) > 2 else "The-Jungle-Map.csv"
    convert_xlsx_to_csv(infile, outfile)
