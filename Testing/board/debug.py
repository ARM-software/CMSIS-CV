#from scripts.export import *
#
#with open("results/output_0.dat","rb") as f:
#         r = read_tensors(f)
#
#for i in r:
#    print(i.tensor)


import cv2 as cv
import numpy as np 
from PIL import ImageOps
import PIL
import PIL.Image
import io 
import scripts.export
from scripts.export import AlgoImage,serialize_tensors,read_tensors
 
 
 
#with open("results/output_0.dat","rb") as f:
#    r = read_tensors(f)
 
#print(r[0].dim)
 
ref = AlgoImage.open("references/dev/test_0_img_0.tiff").tensor
out = AlgoImage.open("results/img/AC6/VHT-Corstone-300/dev/test_0_img_0.tiff").tensor
 
print(ref-out)
 
print(out[0,0])
print(ref[0,0])