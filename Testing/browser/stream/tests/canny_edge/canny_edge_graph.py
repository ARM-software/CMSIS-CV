# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *

from ...appnodes import * 

# Define the datatype we are using for all the IOs in this
# example
# 

def mk_canny_edge(W=640,H=480,OPENCV=True):

    the_graph = Graph()
    the_graph.defaultFIFOClass  = ImageFIFO

    
    image_t = CImageType(W,H,t=CImageType.RGBA32)
    
    camera=WebCamera("camera",W,H)
    
    
    # Canny edge
    to_gray8 = RGBA32ToGray8("to_gray8",W,H)
    gaussian = GaussianFilter("gaussian",W,H)
    
    cv_to_rgba1 = Gray8ToRGBA32("cv_to_rgba",W,H)
    cv_to_rgba2 = Gray8ToRGBA32("u8_to_rgba",W,H)
    canny_edge = CannyEdge("canny",W,H,config=True)
    to_rgba = Gray16ToRGBA32("to_rgba",W,H)
    
    display1=WebDisplay1("display1",W,H)

    if OPENCV:
       g16_to_g8 = Gray16ToGray8("g16_to_g8",W,H)
       canny_edge_cv = OpenCVCanny("canny_cv",W,H,firstAlgo=False,config=True)
       
       display2=WebDisplay2("display2",W,H)
        
    gauss = gaussian(to_gray8(camera(the_graph)))
    
    display1(cv_to_rgba2(canny_edge(gauss)))

    if OPENCV:
       display2(cv_to_rgba1(canny_edge_cv(gauss)))

    return(the_graph)


