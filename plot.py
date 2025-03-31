from PIL import Image, ImageDraw
import sys
import pathlib
if len(sys.argv) != 2:
    print(sys.argv)
    print("Please pass a .txt file with coordinates from the /data directory.\n")
    exit

with Image.new("1", (800,800), 255) as im:
    draw = ImageDraw.Draw(im)
    
    data = open(sys.argv[1], "r")

    for line in data:
        line.strip()
        p = line.split()
        draw.circle((int(p[0]),int(p[1])), 1, fill="Black")

    # need to rotate image because of different coordinate
    # systems used between my poisson distribution and Pillow
    im = im.rotate(angle=270)
    filename = pathlib.Path(sys.argv[1]).stem
    im.save(f"./plot_output/{filename}.jpg", "JPEG")
    


"""
import turtle
import tkinter as tk
from PIL import Image
import os

turtle.screensize(2000,2000)
turtle.setup(800,800)
wn = turtle.Screen()
pen = turtle.Turtle()
pen.penup()

turtle.setworldcoordinates(100,100,20,20)
#root = tk.Tk()
#canvas = tk.Canvas(root, width=500, height=500)
#canvas.pack()
#pen = turtle.RawTurtle(canvas)


pen.penup()
pen.speed(0)
turtle.tracer(0,0)

data = open("data.txt", "r")

for line in data:
    line.strip()
    p = line.split()
    pen.goto(int(p[0]), int(p[1]))
    pen.dot()

turtle.update()
canvas = turtle.getscreen().getcanvas()
canvas.postscript(file="canvas.ps", height=10000, width=10000)
psimage = Image.open("canvas.ps")
psimage.save("canvas.jpg")
os.remove("canvas.ps")
turtle.done()
"""
