from chi2test import *
from skimage import io
import numpy as np

img = io.imread('data/test/5_processed.png')
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
