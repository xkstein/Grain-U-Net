import os
import numpy as np
from skimage import io
from src.pixels_per_grain import pixels_per_grain

INPUT_DIR = 'imgs/'

test_dir_names = np.sort(os.listdir(INPUT_DIR))
is_img = np.char.find(test_dir_names, '.png')
isnt_test = np.char.find(test_dir_names, '__')
dir_names = test_dir_names[np.logical_and([is_img != -1], [isnt_test == -1])[0]]

for name in test_dir_names:
    fname = f"{INPUT_DIR}{name}"
    trace = 255 - io.imread(fname)
    print(f"Reading {fname}")
    if len(trace.shape) > 2:
        trace = trace[:,:,0]
    grain_sizes = pixels_per_grain(trace)
    print(grain_sizes)
