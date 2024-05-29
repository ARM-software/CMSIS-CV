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
input =   AlgoImage.open("inputs/dev/img_0.tiff").tensor
ref = AlgoImage.open("references/dev/test_0_img_0.tiff").tensor
out = AlgoImage.open("results/img/AC6/VHT-Corstone-300/dev/test_0_img_0.tiff").tensor
 
print(ref-out)
 
print("out", out[0,1])
print("ref", ref[0,1])
c = 0
t = []
ref = ref.astype(dtype=np.int16)
out = out.astype(dtype=np.int16)
for i in range(0, 64):
    for j in range(0, 64):
        if(abs(ref[i,j]- out[i,j]) >1):
            if(c == 0):
                print(i,j)
            c+=1
            t.append(abs(ref[i,j]- out[i,j]))
print(c)
print(t)
i = 1
j = 1
print(input[i-1,j-1], input[i-1,j],input[i-1,j+1],input[i,j-1],input[i,j],input[i,j+1],input[i+1,j-1],input[i+1,j],input[i+1,j+1])
print(input[i-1,j-1]+2* input[i-1,j]+input[i-1,j+1]+2*input[i,j-1]+4*input[i,j]+2*input[i,j+1]+input[i+1,j-1]+2*input[i+1,j]+input[i+1,j+1])
print((input[i-1,j-1]+2* input[i-1,j]+input[i-1,j+1]+2*input[i,j-1]+4*input[i,j]+2*input[i,j+1]+input[i+1,j-1]+2*input[i+1,j]+input[i+1,j+1])/16)
print(0.0625*input[i-1,j-1]+0.125* input[i-1,j]+0.0625*input[i-1,j+1]+0.125*input[i,j-1]+0.25*input[i,j]+0.125*input[i,j+1]+0.0625*input[i+1,j-1]+0.125*input[i+1,j]+0.0625*input[i+1,j+1])
kernel = cv.getGaussianKernel(3,0)
print("kernel", kernel)
print(input[0,0], input[0,1], input[1,0], input[1,1])
print(ref[0,0])