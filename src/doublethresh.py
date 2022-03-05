from skimage import measure
import numpy as np

def double_thresh(conservative, liberal):
    '''
    Double Threshold

    This code takes two images of a grain boundary, one which only includes high
    certainty grain boundaries (called conservative) and another which includes more
    boundaries with lower certainty (called liberal). It combines them by adding to
    the conservative image boundaries in the liberal image which overlap with the
    conservative image
    '''
    output = conservative

    lib_lab = measure.label(liberal)
    overlap = np.unique(lib_lab[conservative])

    for label in overlap:
        if label == 0:
            continue
        output[lib_lab == label] = 1

    return output

if __name__ == '__main__':
    from skimage import io

    img = io.imread('../data/test/0_predict.png')
    img_thresh = double_thresh(img < 230, img < 253)
    io.imshow(img < 253)
    io.show()
    io.imshow(img < 230)
    io.show()
    io.imshow(img_thresh)
    io.show()
