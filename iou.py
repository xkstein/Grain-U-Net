# This is a little set of functions to calculate the mean IOU per-grain (just
# called IOU in the unet paper). 
#
# Usage: To use the algorithm outlined in the paper, use find_iou()
#
# Source: http://celltrackingchallenge.net/evaluation-methodology/ (under SEG)
# Author: jamie.k.eckstein@gmail.com

from skimage import io, transform, measure, morphology
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


def find_iou(test_dir = 'data/test/', traced_dir = 'data/test/key/'):
    traced_names = np.sort(os.listdir(traced_dir))
    traced = np.char.strip(traced_names, '.png')

    results = np.array([], dtype=np.float64)

    for key in traced:
        ml_img = io.imread(test_dir + key + '_processed.png') / 255
        ml_img = 1 - ml_img
    #    ml_img = io.imread(test_dir + key + '_predict.png') / 255
        key_img = transform.resize(io.imread(traced_dir + key + '.png'), ml_img.shape)
        key_img = key_img > 0.9

        ml_blob = measure.label(ml_img, background=0, connectivity=1, return_num=True)
        key_blob = measure.label(key_img, background=0, return_num=True)
        jaccard = np.zeros(key_blob[1])
    #    show_imgs(ml_blob[0], key_blob[0], wait=False)
        for i in range(1, key_blob[1]):
            ref_obj = (key_blob[0] == i)

            pred_ref_mask = np.zeros(key_blob[0].shape)
            pred_ref_mask[(ref_obj == 1)] = (ml_blob[0])[(ref_obj == 1)]
    #        show_imgs(ref_obj, pred_ref_mask, wait=False)
            pred_labels = np.unique(pred_ref_mask)
            pred_labels = pred_labels[pred_labels != 0]
            for j in pred_labels:
                pred_obj = (ml_blob[0] == j)
                n_intersection = np.sum(np.logical_and(pred_obj, ref_obj))
                if n_intersection > 0.5 * np.sum(ref_obj):
                    jaccard[i] = find_jaccard(pred_obj, ref_obj)
    #                print(jaccard[i])
    #                show_imgs(pred_obj, ref_obj, overlap=True, wait=False)
        print(f'img {key} \t Mean IOU: {np.mean(jaccard)}')
        results = np.append(results, np.mean(jaccard))

    print(f'\nMean Mean IOU = {np.mean(results)}\nMedian Mean IOU = {np.median(results)}\n\n')
    print(f'Max Mean IOU = {np.max(results)}, Min Mean IOU = {np.min(results)}')

# These next two functions are basically useless, I just don't have the heart
# to delete them
def mean_iou(pred, test):
    return (find_jaccard(pred, test) + find_jaccard(1 - pred, 1 - test))/2

def mean_iou_by_intensity(pred, test, in_range=(0.1,1), n_steps=5, verbose=False):
    intensity = np.linspace(in_range[0], in_range[1], n_steps)
    meaniou = 0
    for thresh in intensity:
        pred_thresh = pred >= thresh
        meaniou += find_jaccard(pred_thresh, test)
        if verbose:
            show_imgs(pred_thresh, test, overlap=True, wait=False)
    return meaniou / n_steps
