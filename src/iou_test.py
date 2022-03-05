'''
IOU Test

This is a little set of functions to calculate the mean IOU per-grain (just
called IOU in the unet paper). 

Usage: To use the algorithm outlined in the paper, use find_iou()

Source: http://celltrackingchallenge.net/evaluation-methodology/ (under SEG)
Author: jamie.k.eckstein@gmail.com
'''

from skimage import io, measure, morphology
import os
import numpy as np
import time
def show_imgs(img1, img2, overlap=False, wait=True):
    if overlap:
        img = np.zeros((img1.shape + (3,)))
        img[:,:,0] = img1
        img[:,:,1] = img2
        io.imshow(img)
        io.show()
    else:
        io.imshow(img1)
        io.show()
        io.imshow(img2)
        io.show()
    if wait:
        input()

def find_jaccard(pred, test):
    intersect = np.logical_and(pred, test)
    union = np.logical_or(pred, test)
    jaccard = np.sum(intersect) / np.sum(union)
    return jaccard


def find_iou(pred, ref, verbose=False, ret_arr=False):
    pred_blob = measure.label(pred, background=0, connectivity=1, return_num=True)
    ref_blob = measure.label(ref, background=0, return_num=True)
    jaccard = np.zeros(ref_blob[1])
    for i in range(1, ref_blob[1]):
        ref_obj = (ref_blob[0] == i)
        
        pred_ref_mask = np.zeros(ref_blob[0].shape)
        pred_ref_mask[(ref_obj == 1)] = (pred_blob[0])[(ref_obj == 1)]

        pred_labels = np.unique(pred_ref_mask)
        pred_labels = pred_labels[pred_labels != 0]
        for j in pred_labels:
            pred_obj = (pred_blob[0] == j)
            n_intersection = np.sum(np.logical_and(pred_obj, ref_obj))
            if n_intersection > 0.5 * np.sum(ref_obj):
                jaccard[i] = find_jaccard(pred_obj, ref_obj)
                if verbose: 
                    show_imgs(pred_obj, ref_obj, overlap=True, wait=False)
                    print(jaccard[i])
                break
        if verbose: print(np.mean(jaccard))
    if ret_arr:
        return jaccard
    else:
        return np.mean(jaccard)

