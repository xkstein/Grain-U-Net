# Instructions

These instructions assume no prior knowledge of Python, Jupyter Notebooks, etc. You may be better served simply following the README. These instructions are current as of 9/5/2022. 

Running `main.ipynb` (no transfer learning)

1. Create a folder named "Lab" in the "My Drive" folder of your Google Drive. Then, clone this repo into that folder. The easiest way to do this is to download Google Drive for Desktop, then download the zip from Github and unzip into "My Drive". 
2. The code is ONLY set up to run on Google Colab. Open `main.ipynb` in Colab by opening the file from Google Drive in your browser. Note that you can run code cells using shift + enter. 
3. Run all code under the Setup Google Colab heading
4. Run all code under the train heading to train your network. Change the `train_dir` variable if you wish to use different training data, and the `save_name` variable to set a name for your trained network. 
5. You now have a trained network. Go to the n-channel testing section. Set `target_size` to the desired dimension (1216x1216 works well for most datasets I've ran), set `test_dir` to point to your directory of images. Make sure the parameter passed through `model.load_weights()` is the name of your trained network from step 4 (`save_name`). 
6. Run the code under the Post Processing section. You may need to change some of the parameters under `process_args`, depending on your dataset. This will just take some trial and error. 
7. Done! Your outputs should be named `unet_skel.png` and can be found in every FOV folder for your test directory. 


Running `main_transfer.ipynb` (transfer learning). These instructions assume you already have a trained (non-transfer learning) network. Run steps 1-4 above if you do not. 

1. Open `main_transfer.ipynb` similar to (2) above. 
2. Run all code under Setup Google Colab. 
3. Skip all cells under Train on Aluminum
4. Go to Transfer onto Pt Data. Set `train_dir` to point to your desired training dataset. Then run all code under Importing Pt Data as Sequence. You have now imported and formatted your data for transfer learning. 
5. Make sure the parameter passed through `model.load_weights()` is your desired trained network. Set the `save_name` to your desired name for your new network. Then run all the code under Loading trained Al model and replacing the expanding blocks. 
6. Run the rest of your notebook. 
7. Done! You now have a trained transfer learning network. To test it, follow the instructions from 5-7 for `main.ipynb`, but make sure the parameter passed through `model.load_weights()` is your transfer learning network. 


Running `scripts/nlBayesTol.ipynb` for denoising

1. Open the notebook from your broswer. 
2. If your images are pngs, you can skip the Converting images to png section. If not, set `test_dir` to point to your dataset, and `out_dir` to the name of your desired png directory. 
3. Run all code in this section. If your files are not properly converted, check the file extension passed through the `.glob()` function to make sure it matches (see section below for more details). 
4. Go to Running Denoising. Set `noisy_dir` to your png directory, and set `clean_dir` to the output directory of denoised images. Run the rest of the code. 


**PROBLEMS YOU MAY RUN INTO**

- Files are recursively found using `.glob()`. If the code cannot find your files, make sure that the file extension passed through `.glob()` matches the file extension of your images. For example, if your images are jpg, change `for image in FOV.glob('raw/*.tif'):` to `for image in FOV.glob('raw/*.jpg'):`