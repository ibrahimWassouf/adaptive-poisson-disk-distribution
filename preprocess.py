from PIL import Image
import os
import pathlib
from glob import glob

directory= "./images"
files = glob(f"{directory}/*.jpg") + glob(f"{directory}/*.png") +glob(f"{directory}/*.tiff") + glob(f"{directory}/*.jpeg")

for file in files:
    im = Image.open(file)
    grayscale = im.convert("L")
    
    #lowers the bit depth to 1 by expressing
    #image as either black or white pixels
    #bw = grayscale.convert("P;4", colors=3)
    
    bw = grayscale
    bw = bw.resize([800,800])
    filename = pathlib.Path(file).stem
    bw.save(f"./preprocessed/{filename}.jpg")
"""   
with Image.open("Banshan_98.jpg") as im:
    im = im.convert("L")
    converted = im.point( lambda p: 255 if p > 100 else 0)
    #converted = converted.resize([800,800])
    converted.save("gray_test.jpg")
"""
