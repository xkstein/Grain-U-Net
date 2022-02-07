''' Sliding Window Compile
This script is expecting to find folder with sequentially numbered files, 
like 0.png, 1.png... or 0_predict.png, 1_predict.png...

Command line Usage:

    % python3 autocompile.py <in_dir> <slices> <sub_slices> <fname_out> <in_start_ind> <in_suf>

    in_dir          - Directory where the images you want to compile are
    slices          - See readme
    sub_slices      - see readme
    fname_out       - (OPTIONAL) filename of outputted compiled image
    in_start_ind    - (OPTIONAL) The starting index of the images you want to compile
    in_suf          - (OPTIONAL) a suffix of the filenames you're reading. 
                        like in 0_predict.png, "_predict.png" would be the suffix
'''
import sys
import numpy as np
from skimage import io
from src.generate_coords import generate_coords

DIR = 'img/croppings/'
SLICES = 2
SUB_SLICES = 2
FNAME = "compilation.png"
START_IND = 0
SUF = '.png'

def sliding_compile(in_dir, slices, sub_slices, fname_out = 'compilation.png', \
                     in_start_ind = 0, fname_suf = '.png'):
    '''Sliding Compilation
    '''
    slices = int(slices)
    sub_slices = int(sub_slices)
    in_start_ind = int(in_start_ind)

    img = io.imread(f"{in_dir}{in_start_ind}{fname_suf}")
    if len(img.shape) > 2:
        img = img[:,:,0]

    coords = generate_coords(slices, sub_slices, win_dims = img.shape[:2])

    win_dims = img.shape[:2]

    # This is where we're building the final image
    compiled_img = np.zeros((win_dims[0] * slices, win_dims[1] * slices))

    for i, coord in enumerate(coords):
        fname = f"{in_dir}{in_start_ind + i}{fname_suf}"
        print(f"Reading: {fname}")
        sub_img = io.imread(fname)
        if len(sub_img.shape) > 2:
            sub_img = sub_img[:,:,0]
        compiled_img[coord[0]:coord[0] + win_dims[0],coord[1]:coord[1] + win_dims[1]] += sub_img

    # average normalizing
    compiled_img[win_dims[0] // 2:-win_dims[0] // 2,:] /= 2
    compiled_img[:,win_dims[1] // 2:-win_dims[1] // 2] /= 2
    print(f"Writing to: {fname_out}")
    io.imsave(fname_out, compiled_img.astype(np.uint8))
        
if __name__ == "__main__":
    if len(sys.argv) >= 4:
        sliding_compile(*sys.argv[1:])
    else:
        sliding_compile(DIR, SLICES, SUB_SLICES, FNAME, START_IND, SUF)
