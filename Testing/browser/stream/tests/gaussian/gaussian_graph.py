# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

from ...appnodes import * 

# Define the datatype we are using for all the IOs in this
# example
# 

def mk_gaussian(W=640,H=480,OPENCV=True):

    the_graph = Graph()
    the_graph.defaultFIFOClass  = ImageFIFO

    image_t = CImageType(W,H,t=CImageType.RGBA32)
    
    camera=WebCamera("camera",W,H)
    
    
    # Canny edge
    to_gray8 = RGBA32ToGray8("to_gray8",W,H)
    gaussian = GaussianFilter("gaussian",W,H)
    to_rgba_cv1 = Gray8ToRGBA32("to_rgba_cv1",W,H)
    display1=WebDisplay1("display1",W,H)

    if OPENCV:
       gaussianCV = OpenCVGaussian("gaussian_cv",W,H)
       to_rgba_cv2 = Gray8ToRGBA32("to_rgba_cv2",W,H)
       display2=WebDisplay2("display2",W,H)
        
    gray=(to_gray8(camera(the_graph)))
    display1(to_rgba_cv1(gaussian(gray)))
   
    if OPENCV:
       display2(to_rgba_cv2(gaussianCV(gray)))

    return(the_graph)


