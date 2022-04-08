'''
autothresh
This is a remade version of code written in C
Original Authors: Derrick T. Carpenter, Jeffrey M. Rickman, Katy Barmak
Author: James Eckstein (jamie.k.eckstein@gmail.com)

Original Comment:
Determines a reasonable threshold for a grayscale image based upon the
distribution of image intensities. Calculates an image histogram (number of
pixels as a function of intensity) and assumes that the function is near-
continuous after smoothing. Finds the inflection point after the
peak position and uses this point as the threshold value, binarizing
the image. Also requires that the derivative of the curve is not
changing excessively at the threshold point.

NOTE: this gives repeatable results, but may result in the loss of a large
fraction of the data...especially if the peak in the histogram is very
sharp.
'''
import numpy as np

def smooth_wave(arr, filter_width, flip_ends=True):
    '''
    smooth_wave

    This function averages out the value of each pixel within with the
    filter_width number of pixels surrounding it. With the flip_ends flag
    enabled, the last filter_width/2 values are flipped at the ends of
    arr so that the ends of the input arr don't need to be averaged with 0.
    '''
    bordered_width = len(arr) + 2 * (filter_width//2)
    temp_list = np.zeros(bordered_width)

    temp_list[filter_width//2:-(filter_width//2)] = arr
    if flip_ends:
        for i in range(1, filter_width//2 + 1):
            temp_list[filter_width//2 - i] = arr[i]
            temp_list[-(filter_width//2 - i + 1)] = arr[-(i + 1)]

    smooth_width = bordered_width + filter_width
    smooth = np.zeros(smooth_width)
    for offset in range(filter_width):
        smooth[offset:-(filter_width-offset)] += temp_list
    smooth = smooth / filter_width
    output = smooth[2*(filter_width//2):-filter_width]
    return output

def second_derivative(arr):
    '''
    second_derivative

    This should be standard laplacian code, I tried to make it
    equivalent to the original code
    '''
    output = np.zeros(len(arr) + 2)

    output[1:-1] = -2 * arr
    output[0:-2] += arr
    output[2:] += arr
    # This is what was in the original code (probably replace with an
    # actual endpoint second derivative formula
    output[1] = 0
    output[-2] = 0

    return output[1:-1]

def automatic_threshold(img):
    '''
    automatic_threshold

    This function takes a grayscale image of grain boundaries and returns a good
    thresholdvalue
    '''
    hist = np.zeros(256)
    for pixel in img.flatten():
        hist[pixel] += 1
#    import matplotlib.pyplot as plt
#    fig, ax = plt.subplots()
#    ax.hist(img.flatten(), bins=list(range(260)))
#    plt.show()

#    text_file = open('hist.txt', 'w')
#    for el in hist:
#        text_file.write(f"{el:.0f}, ")
#    text_file.close()

    hist = smooth_wave(hist, 5)
    dhist = np.gradient(hist)
    d2hist = second_derivative(hist)
    max_ind = np.argmax(hist[3:]) + 3

    print(f"Max hist value {max_ind}")
    if max_ind >= 255:
        raise Exception("Automatic thresholding has failed")

    for ind in range(max_ind, 256):
        if d2hist[ind] >= 0:
            max_ind = ind
            break
        if max_ind >= 255:
            raise Exception("Automatic thresholding has failed")

    print(f"Point of inflection hist value {max_ind}")
    for ind in range(max_ind, 256):
        if dhist[ind] >= 1.7 * dhist[ind + 1]:
            max_ind = ind
            break
        if ind == 255:
            raise Exception("Automatic thresholding has failed")

    return max_ind

if __name__=='__main__':
    from skimage import io

    img_in = io.imread('images/0.png')
    img_in = img_in
    # img_in = io.imread('images/cat.png')
    # img_in = np.int32((255/np.max(img_in)) * img_in)
    thresh = automatic_threshold(img_in)
    print(f"The thresh was: {thresh}")
#    img = img < thresh
#    io.imshow(img)
#    io.show()
