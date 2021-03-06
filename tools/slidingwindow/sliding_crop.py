'''Sliding Window Crop
Command Line Usage

    % python3 autocrop.py <input img> <slices> <sub slices> <starting index> <output file dir>

Or you can come in here and edit the values and run it without arguments

See readme for a better explanation about these things
'''

import sys
from skimage import io
from src.crop import crop
from src.generate_coords import generate_coords
from time import time

IMG_PATH = 'img/01.png'
SLICES = 2
SUB_SLICES = 2
START_IND = 0

def sliding_crop(img_path, slices, sub_slices, fname_ind = 0, out_dir = ''):
    """Sliding Crop
        Crops at the coordinates generated by `src.generate_coord`
        The dimensions of these crops are determined by the (image width / slices)
    """
    slices = int(slices)
    sub_slices = int(sub_slices)
    fname_ind = int(fname_ind)

    print(f"Reading image from {img_path}")
    img = io.imread(img_path)
    img_dims = img.shape[:2]
    win_dims = (img_dims[0] // slices, img_dims[1] // slices)

    coords = generate_coords(slices, sub_slices, img_dims = img_dims)

    for ind, coord in enumerate(coords):
        cropped_img = crop(img, coord, win_dims)
        if cropped_img is not None:
            fname = f"{out_dir}{fname_ind + ind}.png"
            print(f"Saving crop to {fname}")
            io.imsave(fname, cropped_img)

if __name__ == "__main__":
    if len(sys.argv) >= 3:
        sliding_crop(*sys.argv[1:])
    else:
        sliding_crop(IMG_PATH, SLICES, SUB_SLICES, START_IND, 'img/croppings/')
