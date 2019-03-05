#!/usr/bin/env python

import subprocess
import sys
from xml.dom import minidom

input_filename = 'light-guide.scad'
output_filename = 'light-guide.svg'

command = [
    'openscad',
    '-o', output_filename,
    input_filename
]

# Run OpenSCAD
print('=== Running OpenSCAD to create {} from {}...\n'.format(output_filename, input_filename))
proc = subprocess.Popen(command)
proc.communicate()

if proc.returncode != 0:
    sys.exit('Error running openscad')

print()
print('=== Tuning SVG file {} for laser cutting...\n'.format(output_filename))

svg_dom = minidom.parse(output_filename)
svg_node = svg_dom.documentElement

# Source: https://github.com/scottbez1/splitflap/blob/master/3d/svg_processor.py

# Add mm units to the document dimensions
svg_node.attributes['width'].value += 'mm'
svg_node.attributes['height'].value += 'mm'

# Set fill and stroke for laser cutting
attributes = {
    'fill': 'none',
    'stroke': '#0000ff',
    'stroke-width': '0.01',
}
for path in svg_node.getElementsByTagName('path'):
    for (k, v) in attributes.items():
        path.attributes[k] = v

# Write SVG file
with open(output_filename, 'w') as output_file:
    svg_node.writexml(output_file)

