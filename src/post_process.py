'''
kwargs:
    'compilation': (default 'min') defines the image compilation technique
    'liberal_thresh': (default 200) liberal threshold for double threshold
    'conservative_thresh': (default 160) conservative threshold for double threshold
    'invert_double_threshold': (default True) Changes < to > in double threshold
    'n_dilations': (default 3) Number of dilations to apply in closing
    'min_area': (default 100) Max size of a hole to close
    'prune_size': Size to prune with plantcv
'''
from skimage import morphology
from plantcv import plantcv as pcv
import numpy as np
try:
    from src.utils import compile_imgs, double_thresh
except ModuleNotFoundError:
    print('utils mod not found')
    from src.utils.post_processing_utils import compile_imgs, double_thresh

def post_process(imgs, n_dilations=3, min_area=100, prune_size=30, debug=False,\
        save_to_dir=None, out_dict=False, **kwargs):
    '''This tries to make clean skeletons with N Unet output image(s) from an FOV
    '''
    if len(imgs.shape) > 2:
        img_compiled = compile_imgs(imgs, **kwargs)
    else:
        img_compiled = imgs

    img_double_thresh = double_thresh(img_compiled, **kwargs)

    img_dilated = np.copy(img_double_thresh)
    for _ in range(n_dilations):
        img_dilated = morphology.binary_dilation(img_dilated)
    img_closed = morphology.remove_small_holes(img_dilated, area_threshold=min_area)

    skeleton = morphology.skeletonize(img_closed)
    pruned_skeleton, _, _ = pcv.morphology.prune(skeleton.astype('uint8'), prune_size)

    if out_dict:
        return {'compiled': img_compiled,
                'double_thresh': img_double_thresh,
                'dilated': img_dilated,
                'closed': img_closed,
                'skeleton': skeleton,
                'pruned_skeleton': pruned_skeleton
                }

    return pruned_skeleton

if __name__ == '__main__':
    from skimage import io

    fnames = ['../data/test_all/10HR/2400/predict/10hr2400_1.png',
              '../data/test_all/10HR/2400/predict/10hr2401_2.png',
              '../data/test_all/10HR/2400/predict/10hr2402_3.png']

    for ind, fname in enumerate(fnames):
        if ind == 0:
            predictions = io.imread(fname)
        else:
            img = io.imread(fname)
            predictions = np.dstack((predictions, img))

    args = {
            'compilation': 'min',
            'n_dilations': 3,
            'liberal_thresh': 200,
            'conservative_thresh': 160,
            'invert_double_thresh': True,
    }

    post_process(predictions, debug=True, **args)
