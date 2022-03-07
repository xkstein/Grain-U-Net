from skimage import transform, morphology
import numpy as np

def overlay(raw_img, predict, trace, out_dims=(2048,2048), trace_dilation=None):
    '''
    Overlay:
    Helper function to overlay tracings and predicted tracings over images

    raw_img  -> Grain image
    predict  -> Predicted output
    trace    -> Hand traced image
    out_dims -> The dimensions of the returned image (impacts performance)
    trace_dilation -> Passed as an int if you want to dilate the tracing to make it more visible
    '''
    raw_img = transform.resize(raw_img, out_dims, preserve_range=True, anti_aliasing=False)
    predict = transform.resize(predict, out_dims, preserve_range=True, anti_aliasing=False)
    trace = transform.resize(trace, out_dims, preserve_range=True, anti_aliasing=False)

    if trace_dilation is not None:
        try:
            trace = 255 * morphology.binary_dilation(trace, morphology.square(trace_dilation))
        except IndexError:
            pass

    fusion_img = np.zeros((out_dims[0], out_dims[1], 3), dtype=int)
    fusion_img[:, :, 0] = raw_img
    fusion_img[:, :, 1] = raw_img
    fusion_img[:, :, 2] = raw_img
    fusion_img[predict > 100, 0] = 255
    fusion_img[trace > 100, 1] = 255

    return fusion_img
