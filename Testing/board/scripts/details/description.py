from . import *

# Functions to make it easier to write tests

def yuv420_gray8_test(imgid,imgdim,funcid=0):
    return {"desc":f"YUV420 to GRAY8 {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": YUV420ToGray8(),
            "check" : SimilarTensorFixp()
           }

def bgr_8U3C_gray8_test(imgid,imgdim,funcid=0):
    return {"desc":f"BGR 8U3C to GRAY8 {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": BGR8U3CToGray8(),
            "check" : SimilarTensorFixp(1)
           }

def gray8_to_rgb_test(imgid,imgdim,funcid=0):
    return {"desc":f"GRAY8 to RGB24 {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": Gray8ToRGB(),
            "check" : SimilarTensorFixp()
           }

def bgr_8U3C_to_rgb_test(imgid,imgdim,funcid=0):
    return {"desc":f"BGR 8U3C to RGB24 {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": BGR8U3CToRGB(),
            "check" : SimilarTensorFixp()
           }

def yuv420_to_rgb_test(imgid,imgdim,funcid=0):
    return {"desc":f"YUV420 to RGB {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": YUV420ToRGB(),
            "check" : SimilarTensorFixp(1)
           }

def rgb_to_gray_test(imgid,imgdim,funcid=0):
    return {"desc":f"RGB to Gray {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": RGBToGray(),
            "check" : SimilarTensorFixp(1)
           }

def gray8_crop_test(imgdim,funcid,left,top,right,bottom):
    return {"desc":f"Gray 8 crop test {imgdim[0]}x{imgdim[1]} -> {right-left}x{bottom-top} ",
            "funcid": funcid,
            "useimg": [0],
            "reference": CropGray8((left,right),(top,bottom)),
            "check" : SimilarTensorFixp(0)
           }

def rgb_crop_test(imgdim,funcid,left,top,right,bottom):
    return {"desc":f"RGB crop test {imgdim[0]}x{imgdim[1]} -> {right-left}x{bottom-top} ",
            "funcid": funcid,
            "useimg": [1],
            "reference": CropRGB((left,right),(top,bottom)),
            "check" : SimilarTensorFixp(0)
           }
           
def linear_gaussian_test(imgid, imgdim, funcid=2, img_type="gray8"):
    return {"desc":f"Gauss {img_type} image {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": GaussianFilter(),
            "check" : SimilarTensorFixp(1)
            }
