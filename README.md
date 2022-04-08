# Notes
I'm still deciding if it's a good idea to have data in this code base. I tend to keep code and data on different platforms so if I mess with it, and change code it won't mess with your local version. So for now, the data I'm using can be found here: https://drive.google.com/drive/folders/1DqZTwz4W53z-whbqt3C8YNUVCFEnQFn0?usp=sharing

Please let me know how you want to set up data sharing. Maybe each time we expand upon the set dramatically we should add a folder containing that data to the group drive? The benefits of this is that we could be looking at the data modularly, the downside is that the code isn't well suited for that (though thats something we could change).

Original implementation: https://github.com/zhixuhao/unet

My current setup is to run this in a python virtual machine and install src as a package. To do the latter (which is required to access the files in src from `scripts/`), run `pip3 install -e .` in the main directory. This might be an overcomplication, but I'm trying it out for organization.
