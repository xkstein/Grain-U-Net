# Setup:

This version is current as of 8/9/2022. 

We are still running code on Google Colab. To get this code working, make a folder called "Lab" in the root folder of your Google Drive, and clone this repo (Grain-U-Net) into that folder. 

`main.ipynb` includes everything you need to train a standard U-Net (no transfer learning). It also includes all of the post-processing and tests. 

`main_transfer.ipynb` includes the code for training a transfer learning network. 

Each folder contains a more in depth `README.md` concerning the contents of the folder. Code is fairly well documented as well. If you have any questions, please feel free to reach out at ajm2337@columbia.edu. `instructions.md` gives a (much) more in depth explanation of how to run code, if you are unfamiliar with Colab/Python. 

Please email me for the Google Drive link to the data folder. I'd also be happy to supply any trained networks you'd like. 

# Instructions for achieving current best results: 

For aluminum: Open `main.ipynb`. If you already have a network, skip the train section, go to "n-channel testing", put in your desired directory for `test_dir`, and load your network. Then run the code for creating the U-Net outputs and postprocessing. Keep note of the desired `target_size` of your data: this can substantially change results. 1216x1216 seems to work well for most datasets. Depending on the image, you may also want to change some of the postprocessing parameters. If you do not have a network, go to the "train" section, set `train_dir` to `data/train_nouveaux_256`, set a `save_name` for the resulting hdf5 file, and run. 

For platinum (non-Axon/clean): Open `main_transfer.ipynb`. These steps assume you already have a trained aluminum network (if you don't, follow steps above). Go to the section "Transfer onto pt data". Load in your trained Al network, change the `train_dir` to point to your platinum training data, change `save_name` to name the hdf5 file, and run the rest of the notebook. Output hdf5 file is your transfer learning network. To test, follow the same instructions as for aluminum, but with your new network. 

For Axon/noisy platinum: Open `scripts/nlBayesTool.ipynb`. Use this notebook to create a new directory with your denoised platinum images. For Axon, I've been using sigma=30 to denoise. Then folow the steps for platinum. 

# Things to take note of: 

Once the server is up and running almost none of this code will run out of the box anymore. We've been using Colab's Google Drive plugin to do a lot of our file IO, and this won't work once we move off Colab. This should be a simple enough fix. 

I'd like to move off of non-local Bayesian denoising, so once the server is running try training the DnCNN for a full 50 epochs and see how that does on the Axon data. 