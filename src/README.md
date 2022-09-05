# src

- `analysis`: includes the source code for different tests used. Main ones are IOU and chi squared. These tests can be ran through `main.ipynb`. Used to compare the quality of segmentation, given a label. 
- `nl-bayes_20130617`: Compiled C++ code for non-local Bayesian denoising. Easiest way to run is through the notebook in the "scripts" folder. 
- `tests`: tests for evaluating functions in `utils`
- `utils`: `auto_thresh.py`, `mask.py`, `perimeter_polygon.py` won't be used in normal operation. `post_processing_utils.py` used for post processing network outputs into usable skeletons. 
- `visual_tools`: `overlay.py` used for overlaying the network output over the original image. If a tracing is supplied, than the overlay will also indicate whether the output is correct. `sub_plot` used to make some matplotlib operations easier. 
- `data_manager.py`: used for preprocessing and formatting the data for testing the network. 
- `grain_sequence.py`: used for preprocessing and formatting the data for training the network
- `post_process.py`: used for applying post processing to network outputs. 
- `unet.py`: defines the network
- `unet_diceloss.py`: network with custom loss function implemented
- `unet_hypersearch.py`: network which accepts hyperparameters as inputs during initialization, allowing it to be used for hyperparameter searching (see `scripts`)
- `unet_transfer.py`: no longer in use, but framework for implementing transfer learning