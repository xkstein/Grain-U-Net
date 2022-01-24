from skimage import io, measure, morphology
import numpy as np
import os
import pdb

def find_mask(img=None, thresh=200, close_width=8, verbose=False):
    if img.all() == None:
        print('Using my image')
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
    input_path = 'data/train/label'
    mask_path = 'data/train/mask'
    
    img_names = np.sort(os.listdir(input_path))
    valid = np.core.defchararray.find(img_names, '.png')
    img_names = img_names[valid != -1]

    for img_name in img_names:
        img_path = input_path + '/' + img_name
        img = io.imread(img_path)
        mask_img = find_mask(img)
        pdb.set_trace()
        mask_img_path = mask_path + '/' + img_name
        io.imsave(mask_img_path, mask_img)

#    find_mask(verbose=True)

