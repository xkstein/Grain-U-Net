# This crops images into overlapping sub images

import numpy as np
from skimage import io
import os
import pdb

key_path = 'Pt94kx20um_key_aligned.png'
#ref_path = ['Pt94kx20um_99_aligned.png']
ref_path = ['Pt94kx20um_key_aligned.png']
n_slices = 3    # The number of slices when not counting any overlapping
start = 0
name_suf = "_predicted.png"
    
def crop(img, coords, dims, name):
    if coords[0] + dims[0] > img.shape[0] or \
        coords[1] + dims[1] > img.shape[1]:
        return 0
#    pdb.set_trace()
    crop_img = img[coords[0]:coords[0]+dims[0], coords[1]:coords[1]+dims[1]]
    io.imsave(name, crop_img)
    return 1

def main():
    key_img = io.imread(key_path)
    width, height = key_img.shape[:2]

    ref_img = []
    for path in ref_path:
        img = io.imread(path)
        if img.shape != key_img.shape:
            print(f"Warning: {path} not a match, shape {img.shape} when {key_img.shape} was expected")
        ref_img.append(img)
    
    if width%n_slices or height%n_slices:
        print(f"Error: n_slices:{n_slices} doesn't cleanly divide the dimensions of this image")
        exit(1)

    sub_coords = [(0,0), (width // 6,0), (0,height // 6), (width // 6,height // 6)]
    dims = (width // n_slices, height // n_slices)
    name_ind = start
    for i in range(n_slices):
        for j in range(n_slices):
            for ref in ref_img:
                for sub in sub_coords:
                    coords = (i * (width // n_slices) + sub[0], j * (height // n_slices) + sub[1])
                    name = f"{name_ind}.png"
                    if crop(ref, coords, dims, name):
                        name_ind += 1
    
if __name__ == "__main__":
    main()
