from skimage import io, measure, morphology
import numpy as np

def find_mask(img=None, thresh=200, close_width=8, verbose=False):
    if img == None:
        img = io.imread("data/test/key/0.png")

    img_thresh = img > thresh 

    img_label = measure.label(img_thresh, background=0)
    bound = np.concatenate((img_label[0,:], img_label[-1,:], 
                            img_label[:,0], img_label[:,-1]))
    mask_grains = np.zeros((img.shape))
    grains = np.unique(bound)
    for grain in grains:
        if grain == 0:
            continue
        mask_grains[img_label == grain] = 1

    mask = morphology.closing(mask_grains, np.ones((close_width, close_width)))
    
    if verbose == True:
        img_masked = img.copy()
        img_masked[mask == 1] = 0
        io.imshow(img_masked)
        io.show()
#        fuse = np.zeros((img.shape + (3,)))
#        fuse[:,:,0] = mask
#        fuse[:,:,1] = img / np.max(img)
#        io.imshow(fuse)
#        io.show()

    return mask

if __name__ == "__main__":
    find_mask(verbose=True)
