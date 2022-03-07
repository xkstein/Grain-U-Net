from skimage import morphology
import numpy as np

def post_process(image, thresh=255, verbose=False):
    threshold = np.zeros(image.shape)
    threshold[image < thresh] = 1
    skeleton = morphology.skeletonize(threshold)

    if verbose:
        fig, (a, b, c) = plt.subplots(ncols=3)

        imsubplot(a, image, title='UNET Output')
        imsubplot(b, threshold, title='Threshold')
        imsubplot(c, skeleton, title='Skeleton')
        plt.show()
        return skeleton
    else:
        return skeleton
