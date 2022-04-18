'''
This function is used to find the number of neighbors and neighbor's neighbors 
touching each grain.

It starts by masking out edge grains. Then, iterating over all the internal
grains, it dilates each grain and finds the grains which the dilation intersects
with to find the neighbors.
'''
from skimage import io, measure, morphology
import numpy as np

def find_neighbors(img, invert=False, dilation_radius=5):
    '''Finds Every Grain's Neighbor Grains
    img     - 2D, black and white (not grayscale), normalized image
    invert  - False if the insides of the grains are already white
    dilation_radius - How length the dilation should readh to find neighbors

    returns a pretty useless dict containing the indicies of each grain and
    their neighbors and their neighbors neighbors
    '''

    assert (len(img.shape) == 2), 'This only works on 2D images!'
    if invert:
        img = 1 - img

    label = measure.label(img)
    boundary = np.unique(np.concatenate((label[0,:], label[-1,:],\
                                        label[:,0], label[:, -1])))
    for ind in boundary:
        label[label == ind] = 0

    grains = np.unique(label)
    grains = grains[grains != 0]

    grain_neighbors = {}
    for ind in grains:
        mask = np.zeros(label.shape)
        mask[label == ind] = 1

        mask = morphology.binary_dilation(mask, morphology.disk(dilation_radius))

        neighbors = np.unique(label[mask])
        neighbors = neighbors[(neighbors != 0) & (neighbors != ind)]

        grain_neighbors[ind] = neighbors

    grain_neighbors_neighbors = {}
    for ind in grains:
        gnn = np.array([])
        for neighbor in grain_neighbors[ind]:
            gnn = np.concatenate((gnn, grain_neighbors[neighbor]))
        gnn = np.unique(gnn)
        not_valid = [any(nn == grain_neighbors[ind]) for nn in gnn] | (gnn == ind)
        grain_neighbors_neighbors[ind] = gnn[~not_valid]

    total_neighbors = 0
    for key in grain_neighbors:
        total_neighbors += len(grain_neighbors[key])

    print(len(grains))
    print(f'Total Number of neighbors: {total_neighbors}')
    print(f'Average number of neighbors: {total_neighbors / len(grains)}')

    grain_map = {}
    for grain in grains:
        grain_map[str(grain)] = {'neighbors': grain_neighbors[grain].tolist(),\
                'neighbors neighbors': grain_neighbors_neighbors[grain].tolist()}

    return grain_map

if __name__ == '__main__':
    tracing = io.imread('tests/trace.png')
    find_neighbors(tracing, dilation_radius=6)
