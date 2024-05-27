import cv2 as cv
from ..test_utils import *

#############################
# Generation of references
#
#
# Must return references for given inputs
# and number of references returned.
class CopyInput:
    def __call__(self,args,group_id,test_id,srcs):
        for image_id,img in enumerate(srcs):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)


class Gray8ToRGB:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            rgb = cv.cvtColor(i.tensor,cv.COLOR_GRAY2RGB)
            dims = rgb.shape
            img = PIL.Image.frombytes('RGB',(dims[1],dims[0]),rgb)
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class BGR8U3CToRGB:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            # Pack BGR components
            nimg = np.transpose(i.tensor,(1,2,0))
            rgb = cv.cvtColor(nimg,cv.COLOR_BGR2RGB)
            dims = rgb.shape
            img = PIL.Image.fromarray(rgb).convert('RGB')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class BGR8U3CToGray8:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            # Pack BGR componenets
            nimg = np.transpose(i.tensor,(1,2,0))
            gray = cv.cvtColor(nimg,cv.COLOR_BGR2GRAY)
            dims = gray.shape
            img = PIL.Image.frombytes('L',(dims[1],dims[0]),gray)
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class RGBToGray:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            gray = cv.cvtColor(i.tensor,cv.COLOR_RGB2GRAY)
            dims = gray.shape
            img = PIL.Image.frombytes('L',(dims[1],dims[0]),gray)
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class YUV420ToGray8:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            gray = cv.cvtColor(i.tensor,cv.COLOR_YUV2GRAY_420)
            dims = gray.shape
            img = PIL.Image.frombytes('L',(dims[1],dims[0]),gray)
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class YUV420ToRGB:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            rgb = cv.cvtColor(i.tensor,cv.COLOR_YUV2RGB_I420)
            dims = rgb.shape
            img = PIL.Image.fromarray(rgb).convert('RGB')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class CropGray8:
    def __init__(self,width=None,height=None):
        self._width = width # pair 
        self._height = height # pair 

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._width
            h = self._height

            cropped = i.tensor[h[0]:h[1],w[0]:w[1]]

            img = PIL.Image.fromarray(cropped).convert('L')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class CropRGB:
    def __init__(self,width=None,height=None):
        self._width = width # pair 
        self._height = height # pair 

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._width
            h = self._height

            cropped = i.tensor[h[0]:h[1],w[0]:w[1],:]

            img = PIL.Image.fromarray(cropped).convert('RGB')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class GaussianFilter:
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            # Extract the image from the AlgoImage and blur it
            # OpenCv can work with NumPy array but not with Pillow image
            blur = cv.GaussianBlur(i.tensor,(3,3),0,0,cv.BORDER_REPLICATE)
            # Pack the image in an AlgoImage and add it to the reference patterns
            # If we get the blur as it is, it will be recorded as an .npy file
            # It would be simpler with a gray8 as tiff image 
            # So we need to convert back to Pillow
            pil = PIL.Image.fromarray(blur)
            filtered.append(AlgoImage(pil))
            #
            # Our gaussian return a q15 so we can't use a Pillow picture.
            # We convert the result and write is as .npy
            #blur= blur.astype(np.int16)
            #filtered.append(AlgoImage(blur))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)