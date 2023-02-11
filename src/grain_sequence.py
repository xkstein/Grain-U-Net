'''
Grain Sequence

This class reads images from a list of file names, augments them, and returns a
sequence (which keras can train with)

Parameters:
    batch_size      - Number of images per entry in the sequence
    img_size        - The size of the input data
    input_img_paths - List of paths for training images
    label_img_paths - List of paths of label images
                      (in the same order as input_img_paths)
    enable_augment  - If False, doesn't do random data augmentation

Author: Jamie (jamie.k.eckstein@gmail.com)
Credit: fchollet

'''



import random
from re import I
from tensorflow import keras
from skimage import io, exposure
import numpy as np

class GrainSequence(keras.utils.Sequence):
    '''Helper to iterate over the data (as Numpy arrays).'''
    def __init__(self, batch_size, img_size, input_img_paths, label_img_paths):
        self.batch_size = batch_size
        self.img_size = img_size
        self.input_img_paths = input_img_paths
        self.label_img_paths = label_img_paths
        print('augmenting with shift axis')

    def __len__(self):
        return len(self.label_img_paths) // self.batch_size

    def __getitem__(self, idx):
        """Returns tuple (input, target) correspond to batch #idx."""
        i = idx * self.batch_size
        batch_input_img_paths = self.input_img_paths[i : i + self.batch_size]
        batch_label_img_paths = self.label_img_paths[i : i + self.batch_size]

        input_imgs = np.zeros((self.batch_size,) + self.img_size, dtype="float32")
        for j, path in enumerate(batch_input_img_paths):
            img = io.imread(path)
            assert img.shape == self.img_size, f"Training images must be downscaled to {self.img_size} manually"
            if len(img.shape) > 2 and len(self.img_size) < 3:
                img = img[:,:,0]
                img = np.expand_dims(img, -1)

            img = img - img.min()
            img = img.astype('float32') / np.ptp(img)
            #code for contrast stretching
            '''p2, p98 = np.percentile(img, (2, 98))
            img = exposure.rescale_intensity(img, in_range=(p2, p98))'''

            input_imgs[j] = img

        label_imgs = np.zeros((self.batch_size,) + self.img_size[:2] + (1,), dtype="uint8")
        for j, path in enumerate(batch_label_img_paths):
            img = io.imread(path)
            assert img.shape[:2] == self.img_size[:2], f"Training images must be downscaled to {self.img_size} manually"
            if len(img.shape) > 2:
                img = img[:,:,0]

            img = img / 255
            label_imgs[j] = np.expand_dims(img, 2)
        
        for j in range(self.batch_size):
            input_imgs[j], label_imgs[j] = self.augment(input_imgs[j], label_imgs[j])

        return input_imgs, label_imgs

    def augment(self, image, label):
        '''Augments the input images'''
        if random.choice([True, False]):
            image = np.flipud(image)
            label = np.flipud(label)
        if random.choice([True, False]):
            image = np.fliplr(image)
            label = np.fliplr(label)
        if random.choice([True, False]):
            image = np.rot90(image)
            label = np.rot90(label)
        
        image = np.rollaxis(image, -1)
        np.random.shuffle(image)
        image = np.rollaxis(image, 0, 3)

        return image, label
