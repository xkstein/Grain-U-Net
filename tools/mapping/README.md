# Mapping

Nothing in this folder is working yet. Depending on what state it is, it may be part of the .gitignore. 

The current method of thresholding network outputs doesn't leverage the sequential nature of the 12/7/2021 images. 
With a provided ground truth tracing, one could create probability maps for the location of boundaries for the next
image. Creating this map must be fairly accurate and computationally cheap. The first attempt at this operation 
is included in this folder. 

Here, a simple convolution is performed, and then the histogram is matched to a uniform distribution. 
