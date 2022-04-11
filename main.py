from Model import *
from data import *
from pdb import set_trace

#os.environ["CUDA_VISIBLE_DEVICES"] = "0"

'''
data_gen_args = dict(rotation_range=0.2,
                    width_shift_range=0.05,
                    height_shift_range=0.05,
                    shear_range=0.05,
                    zoom_range=0.05,
                    horizontal_flip=True,
                    fill_mode='nearest')
'''
#data_gen_args = None
#
#myGene = trainGenerator(10,'data/train','image','label',data_gen_args,save_to_dir = None)
#
#model = unet()
#model_checkpoint = ModelCheckpoint('unet_membrane.hdf5', monitor='loss',verbose=1, save_best_only=False)
#model.fit(myGene, steps_per_epoch=30, epochs=1, callbacks=[model_checkpoint])

model = unet()

testGene = testGenerator("data/membrane/test")
results = model.predict_generator(testGene,30,verbose=1)
saveResult("data/membrane/test",results)
