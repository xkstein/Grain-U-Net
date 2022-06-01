import numpy as np 
import os
from numpy.lib.function_base import append
import skimage.io as io
import skimage.transform as trans
import numpy as np
from keras.models import *
from keras.layers import *
# from keras.optimizers import *
from tensorflow import keras
from keras.callbacks import ModelCheckpoint, LearningRateScheduler
#from keras import backend as keras
import tensorflow as tf
import pdb

def get_unet(pretrained_weights = None,input_size = (256,256,1)):
    inputs = Input(input_size)
    ConvWrap = lambda filters, size : Conv2D(filters, size, activation = 'relu',\
            padding = 'same', kernel_initializer = 'he_normal', dilation_rate=1)

    conv1 = ConvWrap(64, 3)(inputs)
    conv1 = BatchNormalization()(conv1)
    conv1 = ConvWrap(64, 3)(conv1)
    conv1 = BatchNormalization()(conv1)
    pool1 = MaxPooling2D(pool_size=(2, 2))(conv1)

    conv2 = ConvWrap(128, 3)(pool1)
    conv2 = BatchNormalization()(conv2)
    conv2 = ConvWrap(128, 3)(conv2)
    conv2 = BatchNormalization()(conv2)
    pool2 = MaxPooling2D(pool_size=(2, 2))(conv2)

    conv3 = ConvWrap(256, 3)(pool2)
    conv3 = BatchNormalization()(conv3)
    conv3 = ConvWrap(256, 3)(conv3)
    conv3 = BatchNormalization()(conv3)
    pool3 = MaxPooling2D(pool_size=(2, 2))(conv3)

    conv4 = ConvWrap(512, 3)(pool3)
    conv4 = BatchNormalization()(conv4)
    conv4 = ConvWrap(512, 3)(conv4)
    conv4 = BatchNormalization()(conv4)

    drop4 = Dropout(0.5)(conv4)
    pool4 = MaxPooling2D(pool_size=(2, 2))(drop4)

    conv5 = ConvWrap(1024, 3)(pool4)
    conv5 = BatchNormalization()(conv5)
    conv5 = ConvWrap(1024, 3)(conv5)
    conv5 = BatchNormalization()(conv5)
    drop5 = Dropout(0.5)(conv5)

    up6 = Conv2D(512, 2, activation = 'relu', padding = 'same', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(drop5))
    merge6 = concatenate([drop4,up6], axis = 3)
    conv6 = ConvWrap(512, 3)(merge6)
    conv6 = BatchNormalization()(conv6)
    conv6 = ConvWrap(512, 3)(conv6)
    conv6 = BatchNormalization()(conv6)

    up7 = Conv2D(256, 2, activation = 'relu', padding = 'same', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv6))
    merge7 = concatenate([conv3,up7], axis = 3)
    conv7 = ConvWrap(256, 3)(merge7)
    conv7 = BatchNormalization()(conv7)
    conv7 = ConvWrap(256, 3)(conv7)
    conv7 = BatchNormalization()(conv7)

    up8 = Conv2D(128, 2, activation = 'relu', padding = 'same', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv7))
    merge8 = concatenate([conv2,up8], axis = 3)
    conv8 = ConvWrap(128, 3)(merge8)
    conv8 = BatchNormalization()(conv8)
    conv8 = ConvWrap(128, 3)(conv8)
    conv8 = BatchNormalization()(conv8)

    up9 = Conv2D(64, 2, activation = 'relu', padding = 'same', kernel_initializer = 'he_normal')(UpSampling2D(size = (2,2))(conv8))
    merge9 = concatenate([conv1,up9], axis = 3)
    conv9 = ConvWrap(64, 3)(merge9)
    conv9 = BatchNormalization()(conv9)
    conv9 = ConvWrap(64, 3)(conv9)
    conv9 = BatchNormalization()(conv9)

    conv10 = Conv2D(1, 1, activation = 'sigmoid')(conv9)

    model = Model(inputs = inputs, outputs = conv10)

    opt = keras.optimizers.Adam(lr = 1e-4)
    model.compile(optimizer = opt, loss='binary_crossentropy', metrics = ['accuracy'])

    #model.summary()

    if(pretrained_weights):
    	model.load_weights(pretrained_weights)

    return model
