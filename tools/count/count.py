from skimage import io, measure, morphology
import numpy as np
import os
import pdb

def count_grains(img=None, thresh=200, close_width=8, verbose=False):
    if img.all() == None:
        print('Using my image')
        img = io.imread("data/test/key/0.png")

    img_thresh = img > thresh 

    img_label = measure.label(img_thresh, background=0)
    img_label = morphology.remove_small_objects(img_label, min_size=128)
    bound = np.concatenate((img_label[0,:], img_label[-1,:], 
                            img_label[:,0], img_label[:,-1]))
    boundary_grains = np.unique(bound)

    n_grains = np.size(np.unique(img_label)) - np.size(boundary_grains)
    return n_grains 

if __name__ == "__main__":
    input_path = 'images/1-2hr_trace'
    
    img_names = np.sort(os.listdir(input_path))
    valid = np.core.defchararray.find(img_names, '.png')
    img_names = img_names[valid != -1]
    total_grains = 0

    for img_name in img_names:
        img_path = input_path + '/' + img_name
        img = io.imread(img_path)
        grains = count_grains(img)
        print(f"{img_name} - {grains}")
        total_grains += grains

    print(f"Total Number of grains are {total_grains}")
