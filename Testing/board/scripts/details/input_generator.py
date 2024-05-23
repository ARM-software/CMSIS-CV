from PIL import ImageOps
import PIL
import PIL.Image
import numpy as np
import itertools

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
    return (la + [(16,16),(32,32),(64,64),(1,1),(2,2),(3,3)])


class Format:
    GRAY8 = 0
    RGB = 1

class ImageGen:
    def __init__(self,dims,format=Format.GRAY8,path=None):
        self._path = path
        self._dims = dims
        self._format = format


    def __call__(self):
        res = []
        img = PIL.Image.open(self._path)
        if self._format == Format.GRAY8:
            img = img.convert("L")
        elif self._format == Format.RGB:
            img = img.convert("RGB")

        for d in self._dims:
            img = ImageOps.pad(img, d, color="#fff")

            res.append(AlgoImage(img))

        return res


    @property
    def nb_images(self):
         return len(self._dims)




class GenTensor:
    def __init__(self,a,b,c,the_type):
        self._tensor = np.ones((a,b,c),dtype=the_type)

    def __call__(self):
        return [AlgoImage(self._tensor)]

    @property
    def nb_images(self):
        return 1

