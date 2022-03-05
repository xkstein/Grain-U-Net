'''
U-Net No Fill

This is a version of the Unet which doesn't fill after each convolutional layer, instead cropping each time
Hasn't shown clear benefits, so not adopted
'''

import numpy as np
from keras.models import *
from keras.layers import *
from tensorflow import keras
from keras.callbacks import ModelCheckpoint, LearningRateScheduler
#from keras import backend as keras
import tensorflow as tf
import pdb

def crop_in(img1, img2, ax=[1,2]):
    # This just finds the crop in from the edge to make the expansion and 
    # contration match
    diff1 = np.abs(img1.shape[ax[0]] - img2.shape[ax[0]])
    diff2 = np.abs(img1.shape[ax[1]] - img2.shape[ax[1]])
    crop = (diff1 // 2, int(np.ceil(diff1 / 2)), diff2 // 2, int(np.ceil(diff2 / 2)))
    print(img1.shape, img2.shape, crop)
    return crop

def unet_valid(pretrained_weights = None,input_size = (256,256,1)):
    # Here I'm trying the same thing as the normal unet(), but without padding
    inputs = Input(input_size)
    conv1 = Conv2D(64, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(inputs)
    conv1 = Conv2D(64, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv1)
    pool1 = MaxPooling2D(pool_size=(2, 2))(conv1)
    conv2 = Conv2D(128, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(pool1)
    conv2 = Conv2D(128, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv2)
    pool2 = MaxPooling2D(pool_size=(2, 2))(conv2)
    conv3 = Conv2D(256, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(pool2)
    conv3 = Conv2D(256, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv3)
    pool3 = MaxPooling2D(pool_size=(2, 2))(conv3)
    conv4 = Conv2D(512, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(pool3)
    conv4 = Conv2D(512, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv4)
    drop4 = Dropout(0.5)(conv4)
    pool4 = MaxPooling2D(pool_size=(2, 2))(drop4)

    conv5 = Conv2D(1024, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(pool4)
    conv5 = Conv2D(1024, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv5)
    drop5 = Dropout(0.5)(conv5)

    up6 = Conv2D(512, 1, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(drop5))
    crop4 = crop_in(drop4, up6)
    merge6 = concatenate([drop4[:,crop4[0]:-crop4[1],crop4[2]:-crop4[3],:],up6], axis = 3)

    conv6 = Conv2D(512, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(merge6)
    conv6 = Conv2D(512, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv6)

    up7 = Conv2D(256, 1, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv6))
    crop3 = crop_in(conv3, up7)
    merge7 = concatenate([conv3[:,crop3[0]:-crop3[1],crop3[2]:-crop3[3],:],up7], axis = 3)
    conv7 = Conv2D(256, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(merge7)
    conv7 = Conv2D(256, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv7)

    up8 = Conv2D(128, 1, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv7))
    crop2 = crop_in(conv2, up8)
    merge8 = concatenate([conv2[:,crop2[0]:-crop2[1],crop2[2]:-crop2[3],:],up8], axis = 3)
    conv8 = Conv2D(128, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(merge8)
    conv8 = Conv2D(128, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv8)

    up9 = Conv2D(64, 1, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv8))
    crop1 = crop_in(conv1, up9)
    merge9 = concatenate([conv1[:,crop1[0]:-crop1[1],crop1[2]:-crop1[3],:],up9], axis = 3)
    conv9 = Conv2D(64, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(merge9)
    conv9 = Conv2D(64, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv9)
    conv9 = Conv2D(2, 3, activation = 'relu', padding = 'valid', kernel_initializer = 'he_normal')(conv9)
    conv10 = Conv2D(1, 1, activation = 'sigmoid')(conv9)

    model = Model(inputs = inputs, outputs = conv10)

    opt = keras.optimizers.Adam(lr = 1e-4)
    model.compile(optimizer = opt, loss = 'binary_crossentropy', metrics = ['accuracy'])
    #model.summary()

    if(pretrained_weights):
    	model.load_weights(pretrained_weights)

    return model

