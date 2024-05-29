from scripts.test_utils import *
from scripts.details import *

# Standard size for images that can be used to write the test
# description in a shorter way.
# Those lengths must be coherent with the ones defined in
# src/common.h otherwise the C test wrapper won't be able
# to find the right image for the test
STANDARD_IMG_SIZES = image_dims(np.int8)
STANDARD_YUV_IMG_SIZES = yuv_image_dims(np.int8)
STANDARD_GRAY_IMG_SIZES = image_dims(np.int8)
STANDARD_RGB_IMG_SIZES = image_dims(np.int8)

#print(len(STANDARD_IMG_SIZES))
#print(len(STANDARD_YUV_IMG_SIZES))
#print(len(STANDARD_GRAY_IMG_SIZES))
#print(len(STANDARD_RGB_IMG_SIZES))


# The tests are written using the fuction linear_copy to show how one can use
# a function to make it easier to define a list of tests in a shorter way
# The C code must of course be able to identify the image input
# from the testid which is a bit hidden when writing the Python with abstractions

allSuites = [
    {
        "name" : "Linear Filters",
        "define": "TESTGROUP0",
        "inputs": [ImageGen(STANDARD_GRAY_IMGS,
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen(STANDARD_RBG_IMGS,
                   format=Format.RGB,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(64,64)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(128,128)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(65,65)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(127,127)],
                   format=Format.GRAY8,
                   path="Patterns/House.tiff"),
                   ImageGen(STANDARD_GRAY_IMGS,
                   format=Format.GRAY8,
                   path="Patterns/House.tiff"),
                   ],
        "tests":
          ([linear_copy_test(imgid,imgdim,funcid=1) for imgid,imgdim in enumerate(STANDARD_GRAY_IMGS)] +
           [linear_copy_test(imgid+len(STANDARD_GRAY_IMGS),imgdim,funcid=0,img_type="rgb") for imgid,imgdim in enumerate(STANDARD_RBG_IMGS)])+
            [
                {
                "desc":"Test linear filter 64",
                "funcid": 2,
                "useimg": [len(STANDARD_GRAY_IMGS)+len(STANDARD_RBG_IMGS)],
                "reference": GaussianFilter(),
                "check" : SimilarTensorFixp(1)
                },
                {
                "desc":"Test linear filter 128",
                "funcid": 2,
                "useimg": [len(STANDARD_GRAY_IMGS)+len(STANDARD_RBG_IMGS)+1],
                "reference": GaussianFilter(),
                "check" : SimilarTensorFixp(1)
                },
                {
                "desc":"Test linear filter 65",
                "funcid": 2,
                "useimg": [len(STANDARD_GRAY_IMGS)+len(STANDARD_RBG_IMGS)+2],
                "reference": GaussianFilter(),
                "check" : SimilarTensorFixp(1)
                },
                {
                "desc":"Test linear filter 127",
                "funcid": 2,
                "useimg": [len(STANDARD_GRAY_IMGS)+len(STANDARD_RBG_IMGS)+3],
                "reference": GaussianFilter(),
                "check" : SimilarTensorFixp(1)
                }
            ]+
            [linear_gaussian_test(imgid+len(STANDARD_GRAY_IMGS)+len(STANDARD_RBG_IMGS)+4, imgdim, funcid=2) for imgid,imgdim in enumerate(STANDARD_GRAY_IMGS)]
    },
    {
        "name" : "Color conversions",
        "define": "TESTGROUP1",
        "inputs": [ImageGen(STANDARD_YUV_IMG_SIZES,
                            format=Format.YUV420,
                            path="Patterns/Mandrill.tiff"),
                   ImageGen(STANDARD_RGB_IMG_SIZES,
                            format=Format.BGR8U3C,
                            path="Patterns/Mandrill.tiff"),
                   ImageGen(STANDARD_GRAY_IMG_SIZES,
                            format=Format.GRAY8,
                            path="Patterns/Mandrill.tiff"),
                   ImageGen(STANDARD_RGB_IMG_SIZES,
                            format=Format.RGB24,
                            path="Patterns/Mandrill.tiff")
                   ],
        "tests":
          ([yuv420_gray8_test(imgid,imgdim,funcid=0) for imgid,imgdim in enumerate(STANDARD_YUV_IMG_SIZES)] +
           [bgr_8U3C_gray8_test(imgid+len(STANDARD_YUV_IMG_SIZES),imgdim,funcid=1) for imgid,imgdim in enumerate(STANDARD_RGB_IMG_SIZES)]+
           [gray8_to_rgb_test(imgid+len(STANDARD_YUV_IMG_SIZES)+len(STANDARD_RGB_IMG_SIZES),imgdim,funcid=2) for imgid,imgdim in enumerate(STANDARD_GRAY_IMG_SIZES)] +
           [bgr_8U3C_to_rgb_test(imgid+len(STANDARD_YUV_IMG_SIZES),imgdim,funcid=3) for imgid,imgdim in enumerate(STANDARD_RGB_IMG_SIZES)]+
           [yuv420_to_rgb_test(imgid,imgdim,funcid=4) for imgid,imgdim in enumerate(STANDARD_YUV_IMG_SIZES)] +
           [rgb_to_gray_test(imgid+len(STANDARD_YUV_IMG_SIZES)+len(STANDARD_RGB_IMG_SIZES)+len(STANDARD_GRAY_IMG_SIZES),imgdim,funcid=5) for imgid,imgdim in enumerate(STANDARD_RGB_IMG_SIZES)]
           )
           
    },
]

#print(allSuites[1]["tests"][78])
#exit(0)
#
# Only one test used for developping and debugging a new kernel
# If more than one image is passed, they are assumed to be all input of the test
# But generally the test will use only one input image
devTest = {
<<<<<<< HEAD
        "inputs": [ImageGen([(128,128)],
                   format=Format.YUV420,
                   path="Patterns/JellyBeans.tiff")],
        #"inputs": [UniformYUV420ImageGen([(32,32)],
        #           yuv=(161,99,183))],
        #
        "reference": YUV420ToRGB(),
=======
        "inputs": [ImageGen([(64,64)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff")],
        "reference": GaussianFilter(),
>>>>>>> 814543b (bug issue white image)
        "check" : SimilarTensorFixp(1)
}