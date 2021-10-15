import numpy as np
from skimage import io
import os
import pdb

n_slices = 3    # The number of slices when not counting any overlapping
start = 0
img_dir = 'predicted/'
fname_suf = '_predict.png'

def main():
    ref_img = io.imread(f"{img_dir}{start}{fname_suf}")
    width, height = ref_img.shape[:2]
    
    width_full = n_slices * width
    height_full = n_slices * height

    com_img = np.zeros((width_full, height_full))

    n_images = n_slices ** 2 + (2 * (n_slices * (n_slices - 1))) + (n_slices - 1) ** 2      # I haven't really thought about this tbh, just guessing

    sub_coords = [(0,0), (width // 2,0), (0,height // 2), (width // 2,height // 2)]
    dims = (width, height)

    coord_map = []
    for i in range(n_slices):
        for j in range(n_slices):
            for sub in sub_coords:
                coords = (i * width + sub[0], j * height + sub[1])
                if coords[0] + width > width_full or coords[1] + height > height_full:
                    continue
                coord_map.append(coords)

    print(coord_map)

    for i in range(n_images):
        fname = f"{img_dir}{start + i}{fname_suf}"
        sub_img = io.imread(fname)
        coords = coord_map[i]
#        pdb.set_trace()
        print(coords)
        com_img[coords[0]:coords[0] + width,coords[1]:coords[1] + height] += sub_img
        
    # average normalizing
    com_img[width // 2:-width // 2,:] /= 2
    com_img[:,height // 2:-height// 2] /= 2
    io.imsave("compilation.png", com_img)
    
        
if __name__ == "__main__":
    main()
