import cv2 as cv
from ..test_utils import *
import scipy
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

# Himax is not using the same algorithm as OpenCV
# To be able to do regression testibng and generate test patterns
# here is a Python function behaving as the Himax C function
def _Himax_resize(input,output_w,output_h):
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

    #let the latest row be exactly the same with lastest tmp buffer
    result[row,:] = (tmp[0,:] + np.float32(0.5))


    return(result)

# Resizing using Himax algorithm
class HimaxResizeGray8:
    def __init__(self,w,h):
        self._dst_width = w  
        self._dst_height = h  

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._dst_width
            h = self._dst_height

            resized = _Himax_resize(i.tensor,w,h)

            img = PIL.Image.fromarray(resized).convert('L')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class ResizeGray8:
    def __init__(self,w,h):
        self._dst_width = w  
        self._dst_height = h  

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._dst_width
            h = self._dst_height

            resized = cv.resize(i.tensor,(w,h),interpolation=cv.INTER_LINEAR)

            img = PIL.Image.fromarray(resized).convert('L')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class HimaxResizeBGR_8U3C:
    def __init__(self,w,h):
        self._dst_width = w  
        self._dst_height = h  

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._dst_width
            h = self._dst_height

            resized_b = _Himax_resize(i.tensor[0],w,h)
            resized_g = _Himax_resize(i.tensor[1],w,h)
            resized_r = _Himax_resize(i.tensor[2],w,h)
            resized = np.stack((resized_b,resized_g,resized_r))

            filtered.append(AlgoImage(resized))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class ResizeBGR_8U3C:
    def __init__(self,w,h):
        self._dst_width = w  
        self._dst_height = h  

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._dst_width
            h = self._dst_height

            resized_b = cv.resize(i.tensor[0],(w,h),interpolation=cv.INTER_LINEAR)
            resized_g = cv.resize(i.tensor[1],(w,h),interpolation=cv.INTER_LINEAR)
            resized_r = cv.resize(i.tensor[2],(w,h),interpolation=cv.INTER_LINEAR)
            resized = np.stack((resized_b,resized_g,resized_r))

            filtered.append(AlgoImage(resized))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)


class HimaxResizeBGR_8U3C_to_RGB24:
    def __init__(self,w,h):
        self._dst_width = w  
        self._dst_height = h  

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            #print(i.tensor.shape)
            w = self._dst_width
            h = self._dst_height

            resized_b = _Himax_resize(i.tensor[0],w,h)
            resized_g = _Himax_resize(i.tensor[1],w,h)
            resized_r = _Himax_resize(i.tensor[2],w,h)
            resized = np.stack((resized_b,resized_g,resized_r))

            # Pack BGR components
            nimg = np.transpose(resized,(1,2,0))
            rgb = cv.cvtColor(nimg,cv.COLOR_BGR2RGB)
            dims = rgb.shape
            img = PIL.Image.fromarray(rgb).convert('RGB')
            filtered.append(AlgoImage(img))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)
        
def custom_filter(image):
    return((image[0]+(image[1])*2+image[2]+(image[3])*2+(image[4])*4+(image[5])*2+image[6]+(image[7])*2+image[8])/16)    

def custom_filter5(image):
    return((image[ 0]   + image[ 1]*4 + image[ 2]*6 + image[ 3]*4 + image[ 4]   + 
            image[ 5]*4 + image[ 6]*16+ image[ 7]*24+ image[ 8]*16+ image[ 9]*4 + 
            image[10]*6 + image[11]*24+ image[12]*36+ image[13]*24+ image[14]*6 + 
            image[15]*4 + image[16]*16+ image[17]*24+ image[18]*16+ image[19]*4 + 
            image[20]   + image[21]*4 + image[22]*6 + image[23]*4 + image[24]  )/256)  

def custom_filter7(image):
    return((image[ 0]* 4+ image[ 1]* 14+ image[ 2]* 28+ image[ 3]* 36+ image[ 4]* 28+ image[ 5]* 14+image[ 6]* 4 +
            image[ 7]*14+ image[ 8]* 49+ image[ 9]* 98+ image[10]*126+ image[11]* 98+ image[12]* 49+image[13]*14 +
            image[14]*28+ image[15]* 98+ image[16]*196+ image[17]*252+ image[18]*196+ image[19]* 98+image[20]*28 +
            image[21]*36+ image[22]*126+ image[23]*252+ image[24]*324+ image[25]*252+ image[26]*126+image[27]*36 +
            image[28]*28+ image[29]* 98+ image[30]*196+ image[31]*252+ image[32]*196+ image[33]* 98+image[34]*28 +
            image[35]*14+ image[36]* 49+ image[37]* 98+ image[38]*126+ image[39]* 98+ image[40]* 49+image[41]*14 +
            image[42]* 4+ image[43]* 14+ image[44]* 28+ image[45]* 36+ image[46]* 28+ image[47]* 14+image[48]* 4 )/4096) 

class GaussianFilter:
    def __init__(self, mode_select, kernel_size):
        self._mode = mode_select
        self._kernel_size = kernel_size

    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            # Extract the image from the AlgoImage and blur it
            if(self._kernel_size == 3):
                blur = scipy.ndimage.generic_filter(i.tensor,custom_filter, [self._kernel_size,self._kernel_size],mode = self._mode)
            elif(self._kernel_size == 5):
                blur = scipy.ndimage.generic_filter(i.tensor,custom_filter5, [self._kernel_size,self._kernel_size],mode = self._mode)
            elif(self._kernel_size == 7):
                blur = scipy.ndimage.generic_filter(i.tensor,custom_filter7, [self._kernel_size,self._kernel_size],mode = self._mode)                
            else:
                print("error kernel size not supported")
            # Pack the image in an AlgoImage and add it to the reference patterns
            # If we get the blur as it is, it will be recorded as an .npy file
            # So we need to convert back to Pillow
            pil = PIL.Image.fromarray(blur)
            filtered.append(AlgoImage(pil))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)

class SobelFilter:
    def __init__(self, mode_select, axis_select):
        self._mode = mode_select
        self.axis = axis_select
    
    def __call__(self,args,group_id,test_id,srcs):
        filtered = []
        for i in srcs:
            # Extract the image from the AlgoImage and apply sobel on it, in the direction difined by axis
            #0 for vertical and 1 for horizontal
            sobel = scipy.ndimage.sobel(i.tensor.astype('int16'),self.axis,mode = self._mode)
            # Pack the image in an AlgoImage and add it to the reference patterns
            # Our gaussian return a q15 so we can't use a Pillow picture.
            # We convert the result and write is as .npy
            filtered.append(AlgoImage(sobel))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)
    
class CannyEdge:
    def __call__(self,args,group_id,test_id,srcs):
        procesed = []
        for i in srcs:
            canny = cv.Canny(i.tensor, 95,78)
            # Pack the image in an AlgoImage and add it to the reference patterns
            # If we get the blur as it is, it will be recorded as an .npy file
            # It would be simpler with a gray8 as tiff image 
            # So we need to convert back to Pillow
            pil = PIL.Image.fromarray(canny)
            procesed.append(AlgoImage(pil))

        # Record the filtered images
        for image_id,img in enumerate(procesed):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)
    
class CannyEdgeAutoRef:

    def __call__(self,args,group_id,test_id,srcs):
        procesed = []
        pil = AlgoImage.open(f"RefPatterns/test_{test_id}_img_0.tiff")
        procesed.append(pil)

        # Record the filtered images
        for image_id,img in enumerate(procesed):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)