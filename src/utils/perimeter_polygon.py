from skimage import morphology, measure
import numpy as np

def make_perimeter_polygon(mask, tolerance=5):
    '''Make Perimeter Polygon

    This iffy function leverages skimage measure.approximate_polygon to make a
    polygonal representation of a mask of interior grains

    mask - Mask of interior grains, the mask cannot touch the edge of the image
    tolterance - Max distance that a leg of the polygon can be from mask

    returns (points, perimeter)
    points - list of ordered verticies which make up the polygon
    perimeter - the calculated perimeter of the mask
    '''
    assert np.sum((mask[0,:],mask[-1,:],mask[:,0],mask[:,-1])) == 0,\
                   'There cannot be any touching the edge of the image'

    perimeter = morphology.dilation(mask, morphology.disk(2)) ^ mask
    perimeter = morphology.skeletonize(perimeter)
    perimeter_coords = np.where(perimeter == 1)
    perimeter_coords = np.column_stack(perimeter_coords).astype(int)

    smooth = np.zeros(perimeter_coords.shape, dtype=int)
    smooth[0] = perimeter_coords[0].astype(int)
    for i in range(len(perimeter_coords) - 1):
        perimeter[smooth[i][0], smooth[i][1]] = 0
        hood = perimeter[smooth[i][0] - 1:smooth[i][0] + 2,\
                         smooth[i][1] - 1:smooth[i][1] + 2]
        smooth[i + 1] = smooth[i] + np.argwhere(hood)[0] + [-1, -1]

    poly_points = measure.approximate_polygon(smooth, tolerance=tolerance)
    return poly_points
