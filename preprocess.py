from PIL import Image
import os
import pathlib
from glob import glob

directory= "./images"
files = glob(f"{directory}/*.jpg") + glob(f"{directory}/*.png") +glob(f"{directory}/*.tiff") + glob(f"{directory}/*.jpeg")

for file in files:
    im = Image.open(file)
    grayscale = im.convert("L")
    
    bw = grayscale
    bw = bw.resize([800,800])
    filename = pathlib.Path(file).stem
    bw.save(f"./preprocessed/{filename}.jpg")

