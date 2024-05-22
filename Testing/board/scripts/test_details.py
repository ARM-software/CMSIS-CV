from .test_utils import *
import cv2 as cv

class GenTensor:
    def __init__(self,a,b,c,the_type):
        self._tensor = np.ones((a,b,c),dtype=the_type)

    def __call__(self):
        return [AlgoImage(self._tensor)]

    @property
    def nb_images(self):
        return 1


# Functions and classes used to describe details of the tests
# on the Python side like:
# How to generate reference for a given test
# How to validate a test
# 

class IdenticalTensor(Comparison):
    def __call__(self,ref,result):
        for s,d in zip(ref,result):
            if not np.array_equal(s.tensor,d.tensor):
                self.add_error("Different tensors")
                return False 
        return(True)


class IdenticalImage(Comparison):
    def __call__(self,ref,result):
        return same_images(self,ref,result)

class SimilarTensor(Comparison):
    def __init__(self,r=1e-05,a=1e-08):
        super().__init__()

        self._r = r
        self._a = a

    def __call__(self,ref,result):
        for s,d in zip(ref,result):
            if not np.isclose(s.tensor,d.tensor,rtol=self._r,atol=self._a).all():
                diff = np.abs(s.tensor-d.tensor)
                errorVal = np.max(diff)
                self.add_error(f"Different tensors. Max error = {errorVal}")
                return False 
        return(True)

class SimilarTensorFixp(Comparison):
    def __init__(self,t=0):
        super().__init__()

        self._t = t

    def __call__(self,ref,result):
        for s,d in zip(ref,result):
            diff = np.abs(s.tensor-d.tensor)
            errorVal = np.max(diff)
            if errorVal > self._t:
               self.add_error(f"Different tensors. Max error = {errorVal}")
               return False 
        return(True)

# Generation of references

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