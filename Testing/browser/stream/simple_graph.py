# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsis_stream.cg.scheduler import *
import sys 

sys.path.append("C:/Users/chrfav01/benchresults/Seeed_Grove_Vision_AI_Module_V2/EPII_CM55M_APP_S/app/scenario_app/cmsis_stream_app")

from appnodes import * 

# Define the datatype we are using for all the IOs in this
# example

W=640 
H=480

image_t = CImageType(W,H,t=CImageType.RGBA)

camera=WebCamera("camera",W,H)
to_gray8 = RGBAToGray8("to_gray8",W,H)
#sobel=WebSobel("sobel",W,H)
to_rgba = Gray8ToRGBA("to_rgba",W,H)
display=WebDisplay("display",W,H)

# Create a Graph object
the_graph = Graph()

the_graph.connect(camera.o,to_gray8.i)
the_graph.connect(to_gray8.o,to_rgba.i)
the_graph.connect(to_rgba.o,display.i)
