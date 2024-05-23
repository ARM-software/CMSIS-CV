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
            #pil = PIL.Image.fromarray(blur)
            #filtered.append(AlgoImage(pil))
            #
            # Our gaussian return a q15 so we can't use a Pillow picture.
            # We convert the result and write is as .npy
            blur= blur.astype(np.int16)*127
            filtered.append(AlgoImage(blur))

        # Record the filtered images
        for image_id,img in enumerate(filtered):
            record_reference_img(args,group_id,test_id,image_id,img)

    def nb_references(self,srcs):
        return len(srcs)