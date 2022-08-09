# Scripts

Note: a lot of these are pretty ad-hoc and poorly written. May need to be rewritten depending on your use case. 

- `AddNoise.ipynb`: For adding gaussian noise to an image.
- `FindOptimalResolution.ipynb`: Given a hand tracing, find the optimal U-Net output resolution. 
- `HistMatching.ipynb`: Given a reference histogram, match the intensity histogram of the images of a dataset to that image.
- `IntensityHists.ipynb`: Displays the intensity histograms of various images. 
- `MainHyperSearch.ipynb`: Uses Bayesian Optimization to search for the best performing hyperparameters for a given dataset. 
- `NetworkOutput.ipynb`: Outputs the first channel of an image ran through a U-Net at various layers into `/results/`
- `nlBayesTool.ipynb`: Tool for applying non-local Bayesian denoising to a dataset. 
- `NoiseProfile.ipynb`: Shows the histogram of an image. If used with a homogenous patch of an image, can show the noise profile of the image
- `PostProcessingDebugging.ipynb`: Used for debugging postprocessing and finding optimal postprocessing parameters, given a tracing
- `PtDataCutter.ipynb`: For cutting the platinum data into image patches for training. 
- `ResizeTrainingData.ipynb`: For safely resizing training images. 
- `TracingCleaning.ipynb`: For cleaning tracings. 