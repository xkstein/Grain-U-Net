# This function is supposed to find the translational best match of two images
# by minimizing chi2 values (dist squared)
# Based on 1998 Carpenter, Rickman, Barmak Section IV.B.5
import numpy as np

# This should probably be replaced at some point with like a knnsearch type
# function for speed. This is probably the slowest way to do this
def find_nearest(array, value):
    array = np.asarray(array)
    ind = ((array[0, :] - value[0])**2 + (array[1, :] - value[1])**2).argmin()
    return ind

def find_chi2(A_id, B_id, offset_x=0, offset_y=0):
    A_id_offset[0] = A_id[0] + offset_x
    A_id_offset[1] = A_id[1] + offset_y

    chi2 = 0
    for indA in range(len(A_id)):
        indB = find_nearest(B_id, A_id_offset[:, A_ind])
        chi2 += (A_id_offset[0, indA] - B_id[0, indB]) ** 2 \
                    + (A_id_offset[1, indA] - B_id[1, indB]) ** 2
    return chi2

def chi2test(A, B):
    A_id = np.where(A == 1)
    B_id = np.where(B == 1)

    offset_range = [50, -50, 50, -50]
    offset_n = 10

    offset_map = np.zeros((2, offset_n))
    offset_map[0] = np.linspace(offset_range[1], offset_range[0], offset_n)
    offset_map[1] = np.linspace(offset_range[3], offset_range[2], offset_n)

    chi2s = np.zeros((offset_n, offset_n))
    A_id_offset = np.zeros(A_id)
    for indx, offset_x in enumerate(offset_map[0]):
        for indy, offset_y in enumerate(offset_map[1]):
            chi2s(indx, indy) = find_chi2(A_id, B_id, offset_x, offset_y)

    chi2_min = np.min(chi2s)
    [x, y] = np.where(chi2s == chi2_min)
    offset_x = offset_map[0, x]
    offset_y = offset_map[1, y]

    while True:
        chi2s = np.zeros((3,3))
        chi2s(2, 2) = chi2_min
        for i in range(-1, 2):
            for j in range(-1, 2):
                if (i ==0 and j == 0):
                    continue
                chi2s(i + 2, j + 2) = find_chi2(A_id, B_id, offset_x + i, offset_y + j)
        chi2_min = np.min(chi2s)
        [x, y] = np.where(chi2s == chi2_min)
        if (x == 2 and y == 2):
            return [offset_x, offset_y]
        else:
            offset_x += x - 2
            offset_y += y - 2

'''
def chi2test(A, B):
    A_id = np.where(A == 1)
    B_id = np.where(B == 1)

    offset_range = [50, -50, 50, -50]
    offset_n = 10

    offset_map = np.zeros((2, offset_n))
    offset_map[0] = np.linspace(offset_range[1], offset_range[0], offset_n)
    offset_map[1] = np.linspace(offset_range[3], offset_range[2], offset_n)

    chi2s = np.zeros((offset_n, offset_n))
    A_id_offset = np.zeros(A_id)
    for indx, offset_x in enumerate(offset_map[0]):
        for indy, offset_y in enumerate(offset_map[1]):
            A_id_offset[0] = A_id[0] + offset_x
            A_id_offset[1] = A_id[1] + offset_y

            for indA in range(len(A_id)):
                indB = find_nearest(B_id, A_id_offset[:, A_ind])
                chi2s(indx, indy) += (A_id_offset[0, indA] - B_id[0, indB]) ** 2 \
                                    + (A_id_offset[1, indA] - B_id[1, indB]) ** 2

    chi2_min = np.min(chi2s)
    [x, y] = np.where(chi2s == chi2_min)
    offset_x = offset_map[0, x]
    offset_y = offset_map[1, y]

    while True:
        chi2s = np.zeros((3,3))
        chi2s(2, 2) = chi2_min
'''
