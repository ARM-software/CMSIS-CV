from scripts.test_utils import *
from scripts.test_details import *

# Use image ID is counting from 1

allSuites = [
    {
        "name" : "Linear Filters",
        "define": "TESTGROUP1",
        "inputs": [Image([(16,16)
                         ,(32,32)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   Image([(16,16)
                         ,(32,32)],
                   format=Format.RGB,
                   path="Patterns/Mandrill.tiff")],
        "tests":[
           {"desc":"Copy gray8 image 16x16",
            "funcid": 1,
            "useimg": [0],
            "reference": CopyInput(),
            "check" : IdenticalImage()
           },
           {"desc":"Copy gray8 image 32x32",
            "funcid": 1,
            "useimg": [1],
            "reference": CopyInput(),
            "check" : IdenticalImage()
           },
           {"desc":"Copy RGB image 16x16",
            "funcid": 0,
            "useimg": [2],
            "reference": CopyInput(),
            "check" : IdenticalImage()
           },
           {"desc":"Copy RGB image 32x32",
            "funcid": 0,
            "useimg": [3],
            "reference": CopyInput(),
            "check" : IdenticalImage()
           },
        ]
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
        "inputs": [Image([(64,64)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff")],
        "reference": GaussianFilter(),
        "check" : SimilarTensorFixp()
}