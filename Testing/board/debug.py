import cv2 as cv
import numpy as np 
from PIL import ImageOps
import PIL
import PIL.Image
import io 
import scripts.export
from scripts.export import YUV420,AlgoImage,serialize_tensors,read_tensors
from scripts.details.input_generator import *


#yuv=ImageGen([(128,128)],
#                   format=Format.YUV420,
#                   path="Patterns/JellyBeans.tiff")()[0].img
yuv = AlgoImage.open("inputs/dev/img_0_yuv420.npy").img

print(yuv.planes[0][15,21])
print(yuv.planes[1][15//2,21//2])
print(yuv.planes[2][15//2,21//2])

x = yuv.tensor
rgb = cv.cvtColor(x, cv.COLOR_YUV2RGB_I420)
print(rgb[0,0])

yuv2 = cv.cvtColor(rgb, cv.COLOR_RGB2YUV_I420)
print(yuv2)

#with open("test.dat","rb") as f:
#    r = read_tensors(f)

#for n in r:
#    print(n.tensor.shape)
#yuv=UniformColorImageGen([(32,8)],
#                   format=Format.YUV420,
#                   rgb_color=(0,0,0))()[0].img
#width = 32
#height = 8
#yline = 0 
#uline = height 
#vline = height + height // 4 
#
#y = yuv[yline:yline+height].reshape(height,width)
#u = yuv[uline:uline+(height//4),:].reshape(height//2,width//2)
#v = yuv[vline:vline+(height//4),:].reshape(height//2,width//2)
#
#print(y.shape)
#print(u.shape)
#print(v.shape)
#
#i = AlgoImage.open("inputs/dev/img_0_yuv420.npy").img
#print("")
#print("")
#
#print(yuv.planes[0])
#print(yuv.planes[1])
#print(yuv.planes[2])

#print(yuv.planes[0][12, 15])
#print(yuv.planes[1][6, 14])
#print(yuv.planes[2][6, 14])

#i = np.asarray(AlgoImage.open("references/dev/test_0_img_0.tiff").img)
#
#i = np.array(range(4*5*3)).reshape((4,5,3))
#d = np.array(i.shape)
#fd = np.prod(d[:-1])
#rd = d[-1]
#print(fd,rd)
#
#print(d)
#r = i.reshape(20,3)
#print(r)
#



#16 128 29
#ref = AlgoImage.open("references/dev/test_0_img_0.tiff").tensor
#out = AlgoImage.open("results/img/AC6/VHT-Corstone-300/dev/test_0_img_0.tiff").tensor
#
#ref = ref.astype(dtype=np.int16)
#out = out.astype(dtype=np.int16)
#
#print(out[0,0])
#print(ref[0,0])
#
#diff = np.abs(out-ref)
#errorVal = np.max(diff)
#
#print(diff)#

#ref = AlgoImage.open("inputs/group_1/img_6.npy").tensor
#print(ref.shape)
#
#rgb = np.array(PIL.Image.new("RGB",(256,256),(0,0,255)))
#img.show()


#gray = cv.cvtColor(rgb, cv.COLOR_RGB2GRAY)
#print(gray)
 
#print(r[0].dim)
#input =   AlgoImage.open("inputs/dev/img_0.tiff").tensor
#ref = AlgoImage.open("references/dev/test_0_img_0.tiff").tensor
#out = AlgoImage.open("results/img/AC6/VHT-Corstone-300/dev/test_0_img_0.tiff").tensor
# 
#print(ref-out)
# 
#print("out", out[0,1])
#print("ref", ref[0,1])
#c = 0
#t = []
#ref = ref.astype(dtype=np.int16)
#out = out.astype(dtype=np.int16)
#for i in range(0, 64):
#    for j in range(0, 64):
#        if(abs(ref[i,j]- out[i,j]) >1):
#            if(c == 0):
#                print(i,j)
#            c+=1
#            t.append(abs(ref[i,j]- out[i,j]))
#print(c)
#print(t)
#i = 1
#j = 1
#print(input[i-1,j-1], input[i-1,j],input[i-1,j+1],input[i,j-1],input[i,j],input[i,j+1],input[i+1,j-1],input[i+1,j],input[i+1,j+1])
#print(input[i-1,j-1]+2* input[i-1,j]+input[i-1,j+1]+2*input[i,j-1]+4*input[i,j]+2*input[i,j+1]+input[i+1,j-1]+2*input[i+1,j]+input[i+1,j+1])
#print((input[i-1,j-1]+2* input[i-1,j]+input[i-1,j+1]+2*input[i,j-1]+4*input[i,j]+2*input[i,j+1]+input[i+1,j-1]+2*input[i+1,j]+input[i+1,j+1])/16)
#print(0.0625*input[i-1,j-1]+0.125* input[i-1,j]+0.0625*input[i-1,j+1]+0.125*input[i,j-1]+0.25*input[i,j]+0.125*input[i,j+1]+0.0625*input[i+1,j-1]+0.125*input[i+1,j]+0.0625*input[i+1,j+1])
#kernel = cv.getGaussianKernel(3,0)
#print("kernel", kernel)
#print(input[0,0], input[0,1], input[1,0], input[1,1])
#print(ref[0,0])
