'''
This is a temporary script for auto-generating slides 

'''
import os
from skimage import io
import numpy as np
from src.doublethresh import double_thresh
from src.utils.process import post_process
from src.utils.sub_plot import sub_plot
from src.utils.overlay import overlay
import matplotlib.pyplot as plt
from skimage import morphology
from skimage import img_as_ubyte as as_ubyte
from plantcv import plantcv as pcv

def thresh_then_stack(imgs, cons_thresh=180, lib_thresh=240):
    cons_imgs = np.zeros(imgs.shape)
    lib_imgs = np.zeros(imgs.shape)
    thresh_imgs = np.zeros(imgs.shape)

    for ind in range(imgs.shape[-1]):
        lib_img = imgs[:,:,ind] < lib_thresh
        cons_img = imgs[:,:,ind] < cons_thresh
        thresh_img = double_thresh(cons_img, lib_img)
        
        lib_imgs[:,:,ind] = lib_img
        cons_imgs[:,:,ind] = cons_img
        thresh_imgs[:,:,ind] = thresh_img
        
    thresh_min = 1 - np.amin(1 - thresh_imgs, axis=-1)
    return thresh_min
    skel = morphology.skeletonize(thresh_min)

def stack_then_thresh(imgs, cons_thresh=160, lib_thresh=200):
    if len(imgs.shape) > 2:
        img_min = np.amin(imgs, axis=-1)
    else:
        print('Only one image found!')
        img_min = imgs

    lib_img = img_min < lib_thresh
    cons_img = img_min < cons_thresh
    thresh_img = double_thresh(cons_img, lib_img)

    return img_min, thresh_img

def run_analysis(hour, fov, input_dir, slides_dir, save_to_dir=None):
    imgs = np.array([])
    img_paths = np.array([])
    for fname in np.sort(os.listdir(input_dir)):
        if fname.endswith('.png') and np.char.find(fname, 'trace') == -1:
            path = os.path.join(input_dir, fname)
            img_paths = np.append(img_paths, path)
            img = io.imread(path)
            if len(img.shape) > 2:
                img = img[:,:,0]
                
            if len(imgs) == 0:
                imgs = img
            else:
                imgs = np.dstack((imgs,img))

#    # Threshold First, Stack After
#    threshfirst_stack = thresh_then_stack(imgs)
#    threshfirst_skel = morphology.skeletonize(threshfirst_stack)

    # Stack First, Threshold After
    stackfirst_stack, stackfirst_thresh = stack_then_thresh(imgs)
    stackfirst_skel = morphology.skeletonize(stackfirst_thresh)

    pruned_skel, _, _ = pcv.morphology.prune(stackfirst_skel.astype('uint8'), 30)
    dilated_skel = morphology.binary_dilation(pruned_skel)

    # Generate Summary image
    if not os.path.isdir(slides_dir):
        os.mkdir(slides_dir)

    trace_dir = img_paths[0].split('/predict/')[0]
    trace_path = os.path.join(trace_dir, 'trace.png')

    trace = io.imread(trace_path)

    fig, axes = plt.subplots(2, 3, figsize=(15,8))

#    fig.text(-0.01, 0.75, 'Threshold First\nCombine After', ha='right', va='center', fontsize='x-large')
#    fig.text(-0.007, 0.75, '⇨', ha='left', va='center', fontsize='xx-large')
#    fig.text(-0.01, 0.25, 'Combine First\nThreshold After', ha='right', va='center', fontsize='x-large')
#    fig.text(-0.007, 0.25, '⇨', ha='left', va='center', fontsize='xx-large')

    for ax in axes.flatten():
        ax.set_axis_off()

    sub_plot(axes[0,0], stackfirst_stack, title='Min (logical or) Combined Image')
    sub_plot(axes[0,1], stackfirst_thresh, title='Double Thresholded Combined Image')
    sub_plot(axes[0,2], stackfirst_skel, title='Double Threshold Skeleton')

    sub_plot(axes[1,0], pruned_skel, title='Pruned Skeleton')
    sub_plot(axes[1,1], 1 - dilated_skel, title='Inverted Dilated Skeleton')
    sub_plot(axes[1,2], trace, title='Hand Tracing')

    fig.tight_layout()

    fig.savefig(os.path.join(slides_dir,'double_thresh_analysis.png'), 
                facecolor='white',
                bbox_inches='tight',
                dpi=300
               )
    plt.close(fig)

    if save_to_dir is not None:
        if not os.path.isdir(save_to_dir):
            os.mkdir(save_to_dir)
        io.imsave(os.path.join(save_dir,'1_combine.png'), stackfirst_stack)
        io.imsave(os.path.join(save_dir,'2_thresh.png'), as_ubyte(stackfirst_thresh))
        io.imsave(os.path.join(save_dir,'3_skel.png'), as_ubyte(stackfirst_skel))
        io.imsave(os.path.join(save_dir,'4_pruned_skel.png'), 255 * pruned_skel)
        io.imsave(os.path.join(save_dir,'5_dilated_skel.png'), as_ubyte(dilated_skel))

    return imgs, img_paths


def make_pred_raw_images(hour, fov, imgs, img_paths, slides_dir):
    fig_size = (13,8) if (len(img_paths) == 4) else (10,8)
    ncols = 2 if len(img_paths) < 2 else len(img_paths)

    fig, axes = plt.subplots(2, ncols, figsize=fig_size)

    #fig.text(0, 1, f'{hour}{fov}', ha='right', va='top', fontsize='xx-large')
    fig.text(0, 0.75, 'Raw\nNegative', ha='right', va='center', fontsize='x-large')
    fig.text(0, 0.25, 'U-Net\nPrediction', ha='right', va='center', fontsize='x-large')

    for ax in axes.flatten():
        ax.set_axis_off()

    for ind, path in enumerate(img_paths):
        if len(img_paths) > 1:
            img = imgs[:,:,ind]
        else:
            img = imgs

        fname = path.split('predict/')[-1]
        raw_path = os.path.join(*path.split('/predict/'))
        raw_img = io.imread(raw_path)
        
        axes[0, ind].imshow(raw_img, cmap='gray')
        axes[0, ind].set_title(fname[:-4])
        axes[1, ind].imshow(img, cmap='gray')
        
    fig.tight_layout()
    fig.savefig(os.path.join(slides_dir,'slide1.png'), 
                facecolor='white',
                bbox_inches='tight',
                dpi=300
               )
    plt.close(fig)

def make_pred_trace_images(hour, fov, imgs, img_paths, slides_dir):
    trace_dir = img_paths[0].split('/predict/')[0]
    trace_path = os.path.join(trace_dir, 'trace.png')
    trace = io.imread(trace_path)

    fig_size = (13,8) if (len(img_paths) == 4) else (10,8)
    ncols = 2 if len(img_paths) < 2 else len(img_paths)

    fig, axes = plt.subplots(2, ncols, figsize=fig_size)

    #fig.text(0, 1, f'{hour}{fov}', ha='right', va='top', fontsize='xx-large')
    fig.text(0, 0.75, 'Hand\nTracing', ha='right', va='center', fontsize='x-large')
    fig.text(0, 0.25, 'U-Net\nPrediction', ha='right', va='center', fontsize='x-large')
    
    for ax in axes.flatten():
        ax.set_axis_off()

    for ind, path in enumerate(img_paths):
        fname = path.split('predict/')[-1]
        if len(img_paths) > 1:
            img = imgs[:,:,ind]
        else:
            img = imgs
        
        axes[0, ind].imshow(trace, cmap='gray')
        axes[0, ind].set_title(fname[:-4])
        axes[1, ind].imshow(img, cmap='gray')
        
    fig.tight_layout()
    fig.savefig(os.path.join(slides_dir,'slide2.png'), 
                facecolor='white',
                bbox_inches='tight',
                dpi=300
               )
    plt.close(fig)

def generate_markdown(hour, fov, slides, tag):
    header = f'## {hour.lower()}{fov}'
    img_tag = f'![bg height:600px]'

    #slides = ['slide1.png','slide2.png','stack2thresh.png','thresh2stack_double.png','thresh2stack_stack.png']
    #tag = ['','','### Combine First, Threshold After','### Threshold First - Double Threshold','### Threshold First - Combine']
    md_str = ''
    for ind, slide in enumerate(slides):
        md_str += header
        md_str += '\n'
        md_str += tag[ind]
        md_str += '\n\n'
        md_str += img_tag
        md_str += '('
        md_str += os.path.join(hour.lower(),fov,'slides',slide)
        md_str += ')'
        md_str += '\n\n---\n\n'

    return md_str

if __name__ == '__main__':
    hours = ['1hr2741',
            '1hr2745',
            '1hr2753',
            '1hr2761',
            '1hr2789',
            '2hr2312',
            '2hr2314',
            '2hr2317',
            '2hr2331',
            '2hr2349',
            '2hr2354',
            '2hr2356',
            '2hr2364',
            '4hr2413',
            '4hr2414',
            '4hr2434',
            '4hr2436',
            '4hr2465',
            '4hr2495',
            '4hr2507',
            '10hr2127',
            '10hr2136',
            '10hr2144',
            '10hr2153',
            '10hr2155',
            '10hr2159',
            '10hr2400',
            '10hr2405',
            '10hr2429']

    slides = ['slide1.png','slide2.png','double_thresh_analysis.png']
    tag = ['','','']

    md = ''
    for entry in hours:
        os.system('clear')
        for stupid in hours:
            if stupid == entry:
                print(f"* {stupid}")
            else:
                print(f"  {stupid}")

        hour = entry.split('hr')[0] + 'HR'
        fov = entry.split('hr')[-1]
        fov_dir = f'../data/test_all/{hour}/{fov}'
        input_dir = os.path.join(fov_dir, 'predict')
        save_dir = os.path.join(fov_dir, 'results')
        slides_dir = os.path.join(fov_dir, 'slides')

        imgs, img_paths = run_analysis(hour, fov, input_dir, slides_dir, save_to_dir=save_dir)
        make_pred_raw_images(hour, fov, imgs, img_paths, slides_dir)
        make_pred_trace_images(hour, fov, imgs, img_paths, slides_dir)
        md += generate_markdown(hour, fov, slides, tag)

    with open('../data/test_all/tmp.md', 'w') as f:
        f.write(md)
