'''This is an test/example to help verify the results of:
    - src/utils/perimeter_polygon.py
    - src/utils/mask.py
'''
import os
from skimage import io, img_as_ubyte
import matplotlib.pyplot as plt
import numpy as np
try:
    from src.utils import make_mask
    from src.utils import make_perimeter_polygon
except ModuleNotFoundError:
    import sys
    print('src isn\'t installed a package')
    sys.path.append('../utils')
    from mask import make_mask
    from perimeter_polygon import make_perimeter_polygon
import pdb

tracing = io.imread('trace.png')
interior_grain_mask = make_mask(tracing, close_width=4)
poly = make_perimeter_polygon(interior_grain_mask, 5)

if not os.path.isdir('data'):
    os.mkdir('data')

mask_overlay = 1.0 * np.dstack((tracing, tracing, tracing))
mask_overlay[:,:,0] += (1.0 * np.max(tracing)) * interior_grain_mask
mask_overlay[:,:,1] += (0 * np.max(tracing)) * interior_grain_mask
mask_overlay[:,:,2] += (0 * np.max(tracing)) * interior_grain_mask

io.imsave('data/mask_overlay.png', img_as_ubyte(1/np.max(mask_overlay) * mask_overlay))
io.imsave('data/mask.png', img_as_ubyte(interior_grain_mask))

fig, ax = plt.subplots(ncols=1)

ax.imshow(interior_grain_mask)
ax.plot(poly[:,1], poly[:,0], marker='o', color='r')

plt.gray()
fig.tight_layout()
plt.savefig('data/polygon_perimeter.png')
#plt.show()
