import numpy as np 

##
## Classes for describing tests
## Each test check must inherit from Comparison class
##
class Comparison:

  def __init__(self):
     self._errors = ""

  @property
  def errors(self):
      return self._errors

  def add_error(self,err):
      self._errors += err + "\n";
  

  def check(self,src,dst):
      return True

# Assume the AlgoImage is containing an image
# and not a numpy
# 
def _same_img_type(self,src,dst):
    if (src.img.mode != dst.img.mode):
        self.add_error("Different image types")
        return False

    if (src.img.width != dst.img.width):
        self.add_error("Different image widths")
        return False

    if (src.img.height != dst.img.height):
        self.add_error("Different image heights")
        return False

    return(True)

# Assume AlgoImage aare containing PIL images
def same_images(self,src,dst):
    if len(src) != len(dst):
        self.add_error("Different image list lengths")
        return False

    for s,d in zip(src,dst):
        if not _same_img_type(self,s,d):
            return False
        if list(s.img.getdata()) != list(d.img.getdata()):
            self.add_error("Different image content")
            return False 

    return True

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
            st = s.tensor
            dt = d.tensor
            # Cast to signed so that the difference below is giving
            # the right value
            if st.dtype == np.uint8:
                st = st.astype(dtype=np.int16)
                dt = dt.astype(dtype=np.int16)
            if st.dtype == np.uint16:
                st = st.astype(dtype=np.int32)
                dt = dt.astype(dtype=np.int32)
            if st.dtype == np.uint32:
                st = st.astype(dtype=np.int64)
                dt = dt.astype(dtype=np.int64)

            diff = np.abs(st-dt)
            errorVal = np.max(diff)
            if errorVal > self._t:
               self.add_error(f"Different tensors. Max error = {errorVal}")
               return False 
        return(True)