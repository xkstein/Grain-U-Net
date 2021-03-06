'''Chi2

This is supposed to find the translational best match of two images by minimizing
chi2 values (dist squared)

Based on 1998 Carpenter, Rickman, Barmak Section IV.B.5
Author: Jamie (jamie.k.eckstein@gmail.com)
'''
import numpy as np
import pdb

# This should probably be replaced at some point with like a knnsearch type
# function for speed. This is probably the slowest way to do this
def find_nearest(array, value):
    array = np.asarray(array)
    ind = ((array[0, :] - value[0])**2 + (array[1, :] - value[1])**2).argmin()
    return ind

def find_chi2(A, B, offset_x=0, offset_y=0, out_dict=False):
    A_id = np.array(np.where(A > 0))
    B_id = np.array(np.where(B > 0))

    A_id_offset = np.copy(A_id)
    A_id_offset[0] = A_id[0] + offset_x
    A_id_offset[1] = A_id[1] + offset_y

    chi2 = np.array([0] * A_id.shape[1], dtype=np.float64)
    for indA in range(A_id.shape[1]):
        indB = np.argmin((B_id[0, :] - A_id_offset[0, indA]) ** 2 \
                          + (B_id[1, :] - A_id_offset[1, indA]) ** 2)
        #indB = find_nearest(B_id, A_id_offset[:, indA])
        chi2[indA] = np.min((B_id[0, indB] - A_id_offset[0, indA]) ** 2 \
                        + (B_id[1, indB] - A_id_offset[1, indA]) ** 2)

    if out_dict:
        results = {}
        results["chi2"] = np.sum(chi2)
        results["median_r2"] = np.median(chi2)
        results["avg_chi2"] = np.average(chi2)
        return results
    else:
        return chi2

def find_chi2_matrix(A, B, offset_x=0, offset_y=0, out_dict=False):
    # This is slower for the 256x256 images, which I didn't expect, I'll keep it around 
    # incase we try much bigger images
    A_id = np.array(np.where(A > 0))
    B_id = np.array(np.where(B > 0))

    A_id_offset = np.copy(A_id)
    A_id_offset[0] = A_id[0] + offset_x
    A_id_offset[1] = A_id[1] + offset_y

    AAt = np.diagonal(np.dot(np.transpose(A_id_offset), A_id_offset))
    BBt = np.diagonal(np.dot(np.transpose(B_id), B_id))
    ABt = np.dot(np.transpose(A_id_offset), B_id)
    dists = np.transpose([AAt]) + (-2 * ABt) + BBt
    chi2 = np.sum(np.amin(dists, axis=1))

    if out_dict:
        results = {}
        results["chi2"] = chi2
        results["avg_chi2"] = chi2 / A_id.shape[1]
        return results
    else:
        return chi2

def chi2_align(A, B, offset_range = [50, -50, 50, -50], offset_n=13):
    offset_map = np.zeros((2, offset_n))
    offset_map[0] = np.round(np.linspace(offset_range[1], offset_range[0], offset_n))
    offset_map[1] = np.round(np.linspace(offset_range[3], offset_range[2], offset_n))

    chi2s = np.zeros((offset_n, offset_n))
    for indx, offset_x in enumerate(offset_map[0]):
        for indy, offset_y in enumerate(offset_map[1]):
            chi2s[indx, indy] = find_chi2(A, B, offset_x, offset_y)

    chi2_min = np.min(chi2s)
    [x, y] = np.where(chi2s == chi2_min)

    # if there are multiple chi2s with the min value, choose the one with the least offset
    if (len(x) > 1 or len(y) > 1):
        center_dist = (x - offset_n/2) ** 2 + (y - offset_n/2) ** 2
        center_id = np.argmin(center_dist)
        x = x[center_id]
        y = y[center_id]

    offset_x = offset_map[0, x]
    offset_y = offset_map[1, y]

    while True:
        chi2s = np.zeros((3,3))
        chi2s[1, 1] = chi2_min
        for i in range(-1, 2):
            for j in range(-1, 2):
                if (i ==0 and j == 0):
                    continue
                chi2s[i + 1, j + 1] = find_chi2(A, B, offset_x + i, offset_y + j)
        chi2_min = np.min(chi2s)
        [x, y] = np.where(chi2s == chi2_min)
        if (x.any() == 1 and y.any() == 1):
            results = {}
            results["x"] = offset_x
            results["y"] = offset_y 
            results["chi2"] = chi2_min
            A_n = len(np.array(np.where(A == np.max(A))))
            results["avg_chi2"] = chi2_min / A_n
            return results
        else:
            offset_x += x[0] - 1
            offset_y += y[0] - 1

if __name__ == '__main__':
    img = io.imread('../../data/test/5_processed.png')
    img2 = np.zeros((img.shape[0] + 25, img.shape[1] + 25))
    img2[25:, 25:] = img

    results = chi2_align(img, img2)

    print(results)

    offset_x = int(results["x"][0])
    offset_y = int(results["y"][0])
    fusionimg = np.zeros((img2.shape[0], img2.shape[1], 3))
    fusionimg[offset_x:, offset_y:, 0] = img
    fusionimg[:, :, 1] = img2

    io.imshow(fusionimg)
    io.show()
