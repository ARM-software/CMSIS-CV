import cv2 as cv
import numpy as np 
from PIL import ImageOps
import PIL
import PIL.Image
import io 
import scripts.export
from scripts.export import YUV420,AlgoImage,serialize_tensors,read_tensors
from scripts.details.input_generator import *

#with open("results/output_0.dat","rb") as f:
#     t = read_tensors(f)
#alg = t[0].tensor
#print(alg)

output = AlgoImage.open("results/img/AC6/VHT-Corstone-300/dev/test_0_img_0.npy").tensor

#print(output)
#exit(0)

ref = AlgoImage.open("references/dev/test_0_img_0.npy").tensor


input = AlgoImage.open("inputs/dev/img_0.npy").tensor


#print(ref.shape)
#img = PIL.Image.fromarray(ref).convert('L')
#img.show()
#
#print(input.shape)
#img = PIL.Image.fromarray(input).convert('L')
#img.show()
#

def Himax_resize(input,output_w,output_h):
    tmp = np.zeros((2,output_w),dtype=np.uint8)
    result = np.zeros((output_h,output_w),dtype=np.uint8)
    input_h,input_w = input.shape 
    w_scale = (input_w - 1) / (output_w - 1)
    h_scale = (input_h - 1) / (output_h - 1)


    # DEBUG
    dy,iy = np.modf(h_scale*np.array(range(output_h),dtype=np.float32))
    iy = iy.astype(dtype=np.int32)
    pre_iy = np.hstack((-1,iy))
    #print(dy)
    #exit(0)


    dx,ix=np.modf(w_scale*np.array(range(output_w),dtype=np.float32))
    ix = ix.astype(dtype=np.int32)

    for row in range(output_h-1):
        if iy[row] != pre_iy[row-1]:
           tmp[:,0:output_w-1] = ((1-dx[:-1])*input[iy[row]:iy[row]+2,ix[:-1]] + dx[:-1] * input[iy[row]:iy[row]+2,ix[:-1]+1] + 0.5)
           tmp[:,output_w-1] = input[iy[row]:iy[row]+2,input_w-1]

        result[row,:] = np.floor(((1-dy[row])*tmp[0,:]).astype(np.uint8) + dy[row]*tmp[1,:] + np.float32(0.5))


    row = output_h - 1

    if iy[row] != pre_iy[row-1]:
           tmp[0,0:output_w-1] = ((1-dx[:-1])*input[iy[row],ix[:-1]] + dx[:-1] * input[iy[row],ix[:-1]+1] + 0.5)
           tmp[0,output_w-1] = input[iy[row],input_w-1]

    result[row,:] = ((1-dy[row])*tmp[0,:] + np.float32(0.5))


    return(result)


print(output.shape)
print(ref.shape)
print(input[0].shape)
#print(ref[0])
#result=Himax_resize(input[0],32,16)
#print(result[0])
#
img = PIL.Image.fromarray(output[0]).convert('L')
img.show()

exit(0)

err = np.abs(ref.astype(float)-output.astype(float))

err[np.abs(err) < 1] = 0

#print(err)

#print(ref[-1,:])
#print(result[-1,:])


#img = PIL.Image.fromarray(ref[2]).convert('L')
#img.show()

#img = PIL.Image.fromarray(ref).convert('L')
#img.show()
#yuv=ImageGen([(128,128)],
#                   format=Format.YUV420,
#                   path="Patterns/JellyBeans.tiff")()[0].img
#yuv = AlgoImage.open("inputs/dev/img_0_yuv420.npy").img
#
#print(yuv.planes[0][15,21])
#print(yuv.planes[1][15//2,21//2])
#print(yuv.planes[2][15//2,21//2])
#
#x = yuv.tensor
#rgb = cv.cvtColor(x, cv.COLOR_YUV2RGB_I420)
#print(rgb[0,0])
#
#yuv2 = cv.cvtColor(rgb, cv.COLOR_RGB2YUV_I420)
#print(yuv2)

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