'''This is to test to help verify the results of src/analysis/neighbors.py
'''
import os
from skimage import io, measure
import matplotlib.pyplot as plt
import json
import numpy as np
try:
    from src.analysis.neighbors import find_neighbors
except ModuleNotFoundError:
    import sys
    print('src isn\'t installed a package')
    sys.path.append('..')
    from neighbors import find_neighbors

tracing = io.imread('trace.png')

grain_map = find_neighbors(tracing, dilation_radius=6)

label = measure.label(tracing)
props = measure.regionprops(label)

if not os.path.isdir('data'):
    os.mkdir('data')

fig, axes = plt.subplots(1, 1, figsize=(10,10))

axes.imshow(tracing)
axes.axis('off')
font_dict = {
        'fontsize':'x-small',
        'verticalalignment': 'center',
        'multialignment': 'center'
}
for prop in props:
    if prop.label == 0:
        continue
    axes.text(prop.centroid[1], prop.centroid[0], prop.label, **font_dict)
fig.tight_layout()
fig.savefig('data/numbered_tracing.png')

with open('data/neighbors.json', 'w') as json_file:
    json.dump(grain_map, json_file, indent=4)
