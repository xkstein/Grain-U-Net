'''Generate Coords
You need to specify either the img_dims or win_dims, to return the coords to crop or recombine from
'''
import numpy as np

def generate_coords(grid_slices, sub_slices, img_dims = None, win_dims = None):
    '''Generate Coords
    '''
    if img_dims is not None:
        win_dims = (img_dims[0] // grid_slices, img_dims[1] // grid_slices)
    elif win_dims is not None:
        img_dims = (win_dims[0] * grid_slices, win_dims[1] * grid_slices)
    else:
        raise ValueError("Either img_dims or win_dims must be specified")

    sub_coords = []
    for y_offset in np.linspace(0, win_dims[0], sub_slices, endpoint=False, dtype=int):
        for x_offset in np.linspace(0, win_dims[1], sub_slices, endpoint=False, dtype=int):
            sub_coords.append((x_offset, y_offset))

    coords = []
    for y in np.linspace(0, img_dims[1], grid_slices, endpoint=False, dtype=int):
        for x in np.linspace(0, img_dims[0], grid_slices, endpoint=False, dtype=int):
            for sub in sub_coords:
                point = (x + sub[0], y + sub[1])
                if point[0] + win_dims[0] > img_dims[0] or point[1] + win_dims[1] > img_dims[1]:
                    continue
                coords.append(point)

    return coords
