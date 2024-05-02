# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

from ...appnodes import * 

# Define the datatype we are using for all the IOs in this
# example
# 

def mk_template(W=640,H=480,OPENCV=True):

    the_graph = Graph()

    W=640 
    H=480
    
    image_t = CImageType(W,H,t=CImageType.RGBA)
    
    camera=WebCamera("camera",W,H)
    
    
    # Canny edge
    to_gray8 = RGBAToGray8("to_gray8",W,H)
    gaussian = GaussianFilter("gaussian",W,H)
    
    to_rgba = Gray16ToRGBA("to_rgba",W,H)
    display1=WebDisplay1("display1",W,H)

    if OPENCV:
       gaussianCV = OpenCVGaussian("gaussian_cv",W,H)
       to_rgba_cv = Gray8ToRGBA("to_rgba_cv",W,H)
       display2=WebDisplay2("display2",W,H)
        
    gray = to_gray8(camera(the_graph))
    gauss = gaussian(gray)
    display1(to_rgba(gauss))

    if OPENCV:
       gausscv = gaussianCV(gray)
       display2(to_rgba_cv(gausscv))

    return(the_graph)


