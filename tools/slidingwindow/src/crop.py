"""Crop
Returns image cropped with dims dimensions at coords from the upper left
"""

def crop(img, coord, dims):
    """Crop
    """
    if coord[0] + dims[0] > img.shape[0] or \
        coord[1] + dims[1] > img.shape[1]:
        print("Error: Crop dims went out of bounds, skipping image")
        return None

    crop_img = img[coord[0]:coord[0]+dims[0], coord[1]:coord[1]+dims[1]]
    return crop_img
