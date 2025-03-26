from PIL import Image

with Image.open("Miaodigou_8.jpg") as im:
    im = im.convert("L")
    converted = im.point( lambda p: 255 if p > 100 else 0)
    converted = converted.resize([512,512])
    converted.save("gray_test.jpg")
