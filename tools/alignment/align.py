#!/usr/bin/env python3

# Author: Jamie (jamie.k.eckstein@gmail.com)
#
# Dependancies:
# matplotlib, numpy, skimage, transformations (file), cv2, pymicro
#
# Warning: 
# This ended up being really weird because it uses matplotlib which really isn't
# designed with this stuff in mind. 
#
# Usage:
# Select points by double-left clicking on them in both images, then press ctrl+2
# to select the second pair (this goes up to the ctrl+5). You can select anywhere
# between 2 and 5 pairs of points, the higher the better. You can also re-select
# after rendering the alignment (which you do by pressing enter).
# 
# Right now cropping is done by choosing upper-left corner of your future crop
# and inputting the width and the height of the final image. Needs work.
#
# Keymap:
# mouse1 - Double click to select a point
# mouse2 - Double click cycles through the point selection index
# Enter  - Renders the current alignment with the current point selection
# S      - Saves aligned image (it'll ask you for crop width and height rn)
# ctrl+# - Selects the point selection index

# TODO:
# Fix the blit
# Move to pyqt or pyqtgraph
# Maybe make compatible with Qt4-5Agg

import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
from skimage import io
from transformations import *
#import pdb

raw = io.imread('images/10hr2429_8_raw.tif', as_gray=True)
if raw.dtype != np.uint8:
    raw = np.uint8(raw * 255/np.max(raw))
trace = io.imread('images/10hr2429_trace.gif', as_gray=True)
if trace.dtype != np.uint8:
    trace = np.uint8(trace * 255/np.max(trace))

#trace = cv2.cvtColor(trace[:,:,:-1], cv2.COLOR_RGB2GRAY)/255
align = []
verbose = False

pts = np.zeros((2, 5, 2))
pti = 0

crop_pts = np.zeros(2)
width = 4096
height = 4096

if verbose:
    print('Traced:', trace.shape)
    print('Raw:', raw.shape)

def onclick(event):
    # make point at x-y and send it somewhere to be labeled
    if event.dblclick:
        if event.button == 3:
            global pti
            pti = (pti + 1) % 3
            if verbose:
                print(f"Pti {pti}")
            return 0

        try:
            axi = [fig.axes[i] == event.inaxes for i in range(3)].index(1)
        except ValueError:
            print('Selection: not in axes\n')
        else:
            if axi != 2:
                ptx = event.xdata
                pty = event.ydata

                pts[axi, pti, 0] = ptx
                pts[axi, pti, 1] = pty

                for circ in ax[axi].patches:
                    if circ.get_label() == ['r','g','b','k','y'][pti]:
                        circ.remove()

                circ = plt.Circle((ptx, pty), radius=15, color=['r','g','b','k','y'][pti], \
                                    fill=False, label=['r','g','b','k','y'][pti]) 
                
                fig.canvas.restore_region(bg)
                ax[axi].add_patch(circ)
                ax[axi].draw_artist(circ)
                fig.canvas.blit(fig.bbox)
                fig.canvas.flush_events()

                if verbose:
                    print(f'Selection: axes {axi}\n({ptx:.2f}, {pty:.2f})\n')
                    print(pts)

                return 0
            elif axi == 2:
                ptx = event.xdata
                pty = event.ydata
                
                crop_pts[0] = ptx
                crop_pts[1] = pty

                for patch in ax[axi].patches:
                    patch.remove()
                
#                width = int(input('Crop width:'))
#                height = int(input('Crop height:'))
                
                crop = plt.Rectangle([ptx,pty], width, height, fill=False, lw=2.0, ls='--', \
                                     color='r')
                fig.canvas.restore_region(bg)
                ax[axi].add_patch(crop)
                ax[axi].draw_artist(crop)
                fig.canvas.blit(fig.bbox)
                fig.canvas.flush_events()

                return 0
            else:
                if verbose:
                    print(f'Selection: axes {axi}\n')
                return 0

def onpress(event):
    global align

    if np.char.find(event.key, 'ctrl') != -1:
        if event.key[5:].isdigit():
            if int(event.key[5:]) <= 5:
                global pti
                pti = int(event.key[5:]) - 1

    elif event.key == 'S':
#        width = int(input('Crop width:'))
#        height = int(input('Crop height:'))
        
        x = int(round(crop_pts[0]))
        y = int(round(crop_pts[1]))
        io.imsave("aligned_trace.png", trace[x:x+width,y:y+height])
        io.imsave("aligned_raw.png", align[x:x+width,y:y+height])
        
    elif event.key == 'enter':
        # A check to make sure that overlapping pairs of points were selected and find them
        non0_pts = (pts != 0)
        selected_pts = np.logical_or(non0_pts[:,:,0], non0_pts[:,:,1])
        overlapping = np.logical_and(selected_pts[0], selected_pts[1])
        
        ref_pts = pts[0, overlapping]
        trans_pts = pts[1, overlapping]

        if np.sum(overlapping) > 2:
            print(f"Using {np.sum(overlapping)} point alignment...")
            align = transform_5pt(raw, ref_pts, trans_pts, (trace.shape[1], trace.shape[0]))
        elif np.sum(overlapping) == 2:
            print("Warning: Using 2 point alignment (suboptimal)...")
            align = transform_2pt(raw, ref_pts, trans_pts, (trace.shape[1], trace.shape[0]))
        elif np.sum(overlapping) < 2:
            print("Not enough valid points selected")
            return 0

        fuse = np.copy(align)
        fuse[trace == 0] = 1
        ax[2].imshow(fuse)
        ax[2].figure.canvas.draw()
        return 0

plt.style.use('dark_background')
fig, ax = plt.subplots(1, 3, figsize=(14, 6))

ax[0].axis('off')
ax[1].axis('off')
ax[2].axis('off')
ax[0].imshow(trace/np.max(trace))
ax[1].imshow(raw/np.max(raw))

fig.canvas.mpl_connect('button_press_event', onclick)
fig.canvas.mpl_connect('key_press_event', onpress)

plt.tight_layout()

bg = fig.canvas.copy_from_bbox(fig.bbox)

plt.show()
