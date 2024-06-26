from PIL import ImageOps
import PIL
import PIL.Image
import numpy as np
import itertools
import cv2 as cv

from ..export import *

########################################
#
# Classes used to generate input patterns
#
#
#

# Typical number of iterations to use to test different cases of a 
# loop unrolling
LOOP_UNROLL_ITERATIONS = [3,4,8,11]


def _nb_lanes(dt):
    if dt == np.uint8 or dt == np.int8:
        return(16)
    if dt == np.uint16 or dt == np.int16 or dt == np.float16:
        return(8)
    if dt == np.uint32 or dt == np.int32 or dt == float:
        return(4)
    if dt == np.uint64 or dt == np.int46 or dt == double:
        return(2)
    return(0)

# Typical number of iterations to use to test different cases of vectorized
# loop
def loop_vec_iterations(dt):
    nb = _nb_lanes(dt)
    return [nb-1, nb, 2*nb, 3*nb-1]

# Typical image dimensions for tests
# It use combinations of unroll and vector loop iterations
# plus some square images and some corner cases that may be useful to
# test padding errors
def image_dims(dt):
    la = list(itertools.product(loop_vec_iterations(dt),LOOP_UNROLL_ITERATIONS))
    # Add some square images and some corner cases for padding checks
    return (la + [(16,16),(32,32),(64,64),(2,2),(3,3)])

def is_ok_for_yuv(x):
    width,height = x
    return(height % 4 == 0 and width % 2 == 0)

def yuv_image_dims(dt):
    la = image_dims(dt)
    la = la + [(4,4),(8,8)]
    la = [x for x in la if is_ok_for_yuv(x)]
    return la

##
## This class describes a format.
##
##

def RGBtoYUV420(src):
    # OpenCV is BGR 
    d = src.shape 
    width = d[1]
    height = d[0]

    yuv = cv.cvtColor(src, cv.COLOR_RGB2YUV_I420)
    yline = 0 
    uline = height 
    vline = height + height // 4 


    y = yuv[yline:yline+height].reshape(height,width)
    u = yuv[uline:uline+(height//4),:].reshape(height//2,width//2)
    v = yuv[vline:vline+(height//4),:].reshape(height//2,width//2)

    return(YUV420(y,u,v))



def YUV420toRGB(src):    
    y = src.planes[0]
    ysize = y.shape[0]
    width = y.shape[1]


    u = src.planes[1]
    v = src.planes[2]
    u = np.frombuffer(u, dtype=np.uint8).reshape((ysize//4, width))  # Read U color channel and reshape to height x width numpy array
    v = np.frombuffer(v, dtype=np.uint8).reshape((ysize//4, width))  # Read V color channel and reshape to height x width numpy array


    yuv = np.vstack((y, u,v)) # Stack planes to 3D matrix (use Y,V,U ordering)

    rgb = cv.cvtColor(yuv, cv.COLOR_YUV2RGB_I420)

    return(rgb)


##
## This class describes a format.
##

class Format:
    GRAY8 = 0
    RGB24 = 1 # RGB888 / RGB24 (packed)
    YUV420 = 2 # Planar format 
    BGR8U3C = 3 # Unpacked. 8 bits per channel

class _ImageGenerator:
    def __init__(self,dims,format=Format.GRAY8):
        self._dims = dims
        self._format = format

    def _gen_img(self,imgs):
        res = []

        for nimg in imgs:
            if self._format == Format.GRAY8:
               nimg = nimg.convert("L")
            elif self._format == Format.RGB24:
                nimg = nimg.convert("RGB")
            elif self._format == Format.YUV420:
                # Unpacked YUV420 object
                nimg = RGBtoYUV420(np.asarray(nimg))
            elif self._format == Format.BGR8U3C:
                nimg = cv.cvtColor(np.asarray(nimg), cv.COLOR_RGB2BGR)
                nimg = np.asarray(nimg,dtype=np.uint8)
                # Unpack components
                nimg = np.transpose(nimg,(2,0,1))

            res.append(AlgoImage(nimg))

        return res


    @property
    def nb_images(self):
         return len(self._dims)

class UniformColorImageGen(_ImageGenerator):
    def __init__(self,dims,format=Format.GRAY8,rgb_color=None):
        self._rgb_color = rgb_color
        super().__init__(dims,format)

    def __call__(self):
        imgs=[]
        for d in self._dims:
            img = PIL.Image.new("RGB",d,self._rgb_color)
            imgs.append(img)
        return self._gen_img(imgs)

class UniformYUV420ImageGen(_ImageGenerator):
    def __init__(self,dims,yuv=None):
        self._yuv = yuv
        super().__init__(dims,format)

    def __call__(self):
        imgs=[]
        for d in self._dims:
            width,height = d
            y = np.ones((width,height),dtype=np.uint8)*self._yuv[0]
            u = np.ones((width//2,height//2),dtype=np.uint8)*self._yuv[1]
            v = np.ones((width//2,height//2),dtype=np.uint8)*self._yuv[2]
            yuv = YUV420(y,u,v)
            imgs.append(AlgoImage(yuv))
        return imgs

class ImageGen(_ImageGenerator):
    def __init__(self,dims,format=Format.GRAY8,path=None):
        self._path = path
        super().__init__(dims,format)

    def __call__(self):
        imgs = []
        img = PIL.Image.open(self._path)

        for d in self._dims:
            nimg = ImageOps.pad(img, d, color="#fff")
            imgs.append(nimg)
        
        return self._gen_img(imgs)

class GenTensor:
    def __init__(self,a,b,c,the_type):
        self._tensor = np.ones((a,b,c),dtype=the_type)

    def __call__(self):
        return [AlgoImage(self._tensor)]

    @property
    def nb_images(self):
        return 1

