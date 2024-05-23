from scripts.test_utils import *
from scripts.details import *

# Standard size for images that can be used to write the test
# description in a shorter way.
STANDARD_GRAY_IMGS = image_dims(np.int8)
STANDARD_RBG_IMGS = image_dims(np.int8)
#print(len(STANDARD_GRAY_IMGS))

# The tests are written using the fuction linear_copy to show how one can use
# a function to make it easier to define a list of tests in a shorter way
# The C code must of course be able to identify the image input
# from the testid which is a bit hidden when writing the Python with abstractions

allSuites = [
    {
        "name" : "Linear Filters",
        "define": "TESTGROUP1",
        "inputs": [ImageGen(STANDARD_GRAY_IMGS,format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen(STANDARD_RBG_IMGS,
                   format=Format.RGB,
                   path="Patterns/Mandrill.tiff")],
        "tests":
          ([linear_copy_test(imgid,imgdim,funcid=1) for imgid,imgdim in enumerate(STANDARD_GRAY_IMGS)] +
           [linear_copy_test(imgid+len(STANDARD_GRAY_IMGS),imgdim,funcid=0,img_type="rgb") for imgid,imgdim in enumerate(STANDARD_RBG_IMGS)])
    },
    {
        "name" : "Tensor tests",
        "define": "TESTGROUP2",
        "inputs": [GenTensor(20,10,4,np.int16)],
        "tests":[
           {"desc":"Test tensor",
            "funcid": 0,
            "useimg": [0],
            "reference": CopyInput(),
            "check" : IdenticalTensor()
           },
        ]
    }
]

# Only one test used for developping and debugging a new kernel
# If more than one image is passed, they are assumed to be all input of the test
# But generally the test will use only one input image
devTest = {
        "inputs": [ImageGen([(64,64)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff")],
        "reference": GaussianFilter(),
        "check" : SimilarTensorFixp()
}