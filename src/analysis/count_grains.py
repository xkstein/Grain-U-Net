"""Count

An implementation of the masking code which counts grains in traced grain images
"""
import os
from skimage import io, measure, morphology
import numpy as np

def count_grains(img, thresh=200):
    """Count Grains
    Counts how many grains are in the input image
    """
    img_thresh = img > thresh

    img_label = measure.label(img_thresh, background=0)
    img_label = morphology.remove_small_objects(img_label, min_size=128)
    bound = np.concatenate((img_label[0,:], img_label[-1,:],
                            img_label[:,0], img_label[:,-1]))
    boundary_grains = np.unique(bound)

    n_grains = np.size(np.unique(img_label)) - np.size(boundary_grains)
    return n_grains

if __name__ == "__main__":
    INPUT_PATH = 'images/10hr_trace'

    img_names = np.sort(os.listdir(INPUT_PATH))
    valid = np.core.defchararray.find(img_names, '.png')
    img_names = img_names[valid != -1]

    total_grains = 0
    for img_name in img_names:
        img_path = INPUT_PATH + '/' + img_name
        trace_img = io.imread(img_path)
        grains = count_grains(trace_img)
        print(f"{img_name} - {grains}")
        total_grains += grains

    print(f"Total Number of grains are {total_grains}")
