"""Grain Pixels
**UNTESTED**

This is an attempt at making some kind of metric to figure out what resolution to run
a given grain image at
"""
import os
from skimage import io, measure, morphology, transform
import numpy as np
import pdb

def pixels_per_grain(trace, dims = None, thresh=0.6):
    """Pixels Per Grain
    Counts how many pixels are in each grain and returns statistical info about that
    Accepts 2 2d arrays, img and trace, and an int (thresh)
    """
    if dims is not None:
        if trace.shape[0] / dims[0] > 1:
            trace = morphology.binary_erosion(trace, morphology.square(trace.shape[0] // dims[0] + 1))
        trace = transform.resize(trace, dims, preserve_range = True)

    #trace_thresh = (trace.astype('float') > thresh).astype('float')
    #trace_label = measure.label(trace_thresh, background=0)

    trace_label = measure.label(trace, background=0, connectivity=1)
    trace_label = morphology.remove_small_objects(trace_label, min_size=128)
    bound = np.concatenate((trace_label[0,:], trace_label[-1,:],
                            trace_label[:,0], trace_label[:,-1]))
    boundary_grains = np.unique(bound)
    for grain in boundary_grains:
        trace_label[trace_label == grain] = 0

    grains = np.unique(trace_label)
    grain_sizes = np.zeros(np.size(grains))
    for ind, grain in enumerate(grains):
        if grain == 0:
            continue
        grain_sizes[ind] = np.sum(trace_label == grain)

    grain_sizes = grain_sizes[grain_sizes > 4]
    
    n_grains = len(grains)
    avg_grain_size = np.mean(grain_sizes)

    grain_radius = np.sqrt(grain_sizes / np.pi)
    avg_radius = np.mean(grain_radius)
    std_radius = np.std(grain_radius)

    return (n_grains, avg_grain_size, avg_radius, std_radius)

if __name__ == "__main__":
    INPUT_PATH = 'data/train_preaug/label'
    #INPUT_PATH = 'images/trace'

    img_names = np.sort(os.listdir(INPUT_PATH))
    valid = np.core.defchararray.find(img_names, '.png')
    img_names = img_names[valid != -1]
    data = np.array([0,0,0])

    for ind, img_name in enumerate(img_names):
        trace_path = INPUT_PATH + '/' + img_name
        trace = io.imread(trace_path)
        trace = (trace / 255)
        if len(trace.shape) > 2:
            trace = trace[:,:,0]

        (n_grains, avg, r_avg, r_std) = pixels_per_grain(trace, dims=(336,336), thresh = 0.6)
        if np.isnan(np.array([avg, r_avg, r_std])).any():
            continue
        if ind == 0:
            data = np.array([avg, r_avg, r_std])
        else:
            data = np.vstack((data,[avg, r_avg, r_std]))

        print(n_grains)
        print(f"{img_name}")
        #print(f"N Grains: {n_grains}\nAvg Pixels per Grain: {avg}\nAvg Ideal Grain Radius: {r_avg}\nIdeal Grain Radius STD: {r_std}")
    if os.path.isdir(INPUT_PATH):
        print(f"Avg Pixels per Grain: {np.mean(data[:,0])}\n \
                Avg Ideal Grain Radius: {np.mean(data[:,1])}\n \
                Ideal Grain Radius STD: {np.mean(data[:,2])}")
    else:
        print(f"Avg Pixels per Grain: {np.mean(data[0])}\n \
                Avg Ideal Grain Radius: {np.mean(data[1])}\n \
                Ideal Grain Radius STD: {np.mean(data[2])}")
