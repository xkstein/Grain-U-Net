'''
This file contains the necessary functions to find the rand err
between two binarized skeletonized images of the same size.
Simply perform:

err = randerr(first,second)
where first and second are just the images in question.
'''

from skimage import data, io, filters, morphology
from skimage.util import invert
from skimage.measure import label
from skimage.color import label2rgb
from skimage.metrics import adapted_rand_error
import numpy as np
import matplotlib.image as mpimg
import matplotlib.pyplot as plt

def labelmake(S):
    inverted = invert(S)

    # Remove the border (1% on each side), which contains disconnects
    s = S.shape[0]
    b = round(s*1/100)
    cropped = inverted[b:s-b,b:s-b]

    label_image = label(cropped, connectivity = 1)
    return label_image

def find_randerr(first,second):
    first_label = labelmake(first) 
    second_label = labelmake(second)
    randerr,p,r = adapted_rand_error(image_true=first_label, image_test=second_label, ignore_labels=[0])
    return randerr
