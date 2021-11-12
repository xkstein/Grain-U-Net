# A set of helper functions for align.py

import numpy as np
import cv2
from pymicro.view.vol_utils import compute_affine_transform

def transform_5pt(img, ref_pts, trans_pts, out_size):
    translation, transformation = compute_affine_transform(ref_pts, trans_pts)
    ref_centroid = np.mean(ref_pts, axis=0)
    trans_centroid = np.mean(trans_pts, axis=0)
    T = np.array([[1, 0, -trans_centroid[0]],
                  [0, 1, -trans_centroid[1]],
                  [0, 0, 1]], dtype=np.float32)
    R = np.array([[1, 0, ref_centroid[0]],
                  [0, 1, ref_centroid[1]],
                  [0, 0, 1]], dtype=np.float32)
    M = np.zeros((3,3), dtype=np.float32)
    M[:-1,:-1] = transformation
    M[2,2] = 1

    A = np.dot(R, np.dot(M, T))

    aligned = cv2.warpPerspective(img, A, out_size)
    return aligned

def transform_2pt(img, ref_pts, trans_pts, out_size):
    r0 = np.sqrt((ref_pts[1,0] - ref_pts[0,0]) ** 2 + (ref_pts[1,1] - ref_pts[0,1]) ** 2)
    r1 = np.sqrt((trans_pts[1,0] - trans_pts[0,0]) ** 2 + (trans_pts[1,1] - trans_pts[0,1]) ** 2)
    scale = r0 / r1

    theta_0 = np.arccos((ref_pts[1, 0] - ref_pts[0, 0]) / r0) * (ref_pts[1, 1] - ref_pts[0, 1]) \
                            / np.abs(ref_pts[1, 1] - ref_pts[0, 1])
    theta_1 = np.arccos((trans_pts[1, 0] - trans_pts[0, 0]) / r1) * (trans_pts[1, 1] - trans_pts[0, 1]) \
                            / np.abs(trans_pts[1, 1] - trans_pts[0, 1])

    dtheta = theta_0 - theta_1

    R = np.array([[np.cos(dtheta), -np.sin(dtheta), 0],
                  [np.sin(dtheta), np.cos(dtheta), 0],
                  [0,0,1]])

    # taking translation data from the first selected point pair
    pt0 = np.dot(R[:-1,:-1], np.transpose(trans_pts[0,:]))
    translate_x = ref_pts[0, 0] - scale * pt0[0]
    translate_y = ref_pts[0, 1] - scale * pt0[1]

    M = np.array([[scale, 0, translate_x],
                  [0, scale, translate_y],
                  [0,0,1]])

    aligned = cv2.warpPerspective(img, np.dot(M,R), out_size)
    return aligned
