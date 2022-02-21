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
            trace = morphology.binary_erosion(trace, morphology.square(trace.shape[0] // dims[0]))
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
    
    return grain_sizes
