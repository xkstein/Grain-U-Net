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
    for offset_x in offset_map[0]:
        for offset_y in offset_map[1]:
            A_id_offset[0] = A_id[0] + offset_x
            A_id_offset[1] = A_id[1] + offset_y

            for i in range(len(A_id)):
                ind = find_nearest(B_id, A_id_offset[:, i])
                