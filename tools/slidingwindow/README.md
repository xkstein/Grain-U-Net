# Sliding Window
![Lil graphic](docs/graphic.gif)

This graphic showes an example of a sliding window cropping where `slices = 2` (aka `grid_slices = 2`)  and `sub_slices = 2`. 

Notice how the first 4 crops happen along the edge, and look like a standard cropping. Also notice the following 5 croppings are offset by half the width of the first cropping area. If `sub_slice` was set to 3, the croppings following the first 4 would be offset by a third the width of the original croppings.

# File Contents
- `sliding_crop.py` -> Crops an image into a sequential set of images which are set by `slice` and `sub_slice` parameters
- `sliding_compile.py` -> Compiles images which are cropped by sliding_crop.py
- `Sliding Window.ipynb` -> jupyter file which crops and compiles
- `src/` -> Got helper functions