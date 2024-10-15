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
STANDART_GRAY_SIZES = [(15, 8), (15, 11), (16, 8), (16, 11), (32, 8), (32, 11), (47, 8), (47, 11), (16, 16), (32, 32), (64, 64)]

VERTICAL = 0
HORIZONTAL = 1
#print(len(STANDARD_IMG_SIZES))
#print(len(STANDARD_YUV_IMG_SIZES))
#print(len(STANDARD_GRAY_IMG_SIZES))
#print(len(STANDARD_RGB_IMG_SIZES))


# The tests are written using the function linear_copy to show how one can use
# a function to make it easier to define a list of tests in a shorter way
# The C code must of course be able to identify the image input
# from the testid which is a bit hidden when writing the Python with abstractions

allSuites = [
    {
        "name" : "Linear Filters",
        "define": "TESTGROUP0",
        "inputs": [ImageGen(STANDART_GRAY_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff")
                   ],
        "tests":
           [gaussian_test(imgid, imgdim, funcid=0, border_type='nearest') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=1, border_type='mirror')for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=2, border_type='wrap') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=3, axis=VERTICAL, border_type='nearest') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=4, axis=VERTICAL, border_type='mirror') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=5, axis=VERTICAL, border_type='wrap') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=6, axis=HORIZONTAL, border_type='nearest') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=7, axis=HORIZONTAL, border_type='mirror') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [sobel_test(imgid, imgdim, funcid=8, axis=HORIZONTAL, border_type='wrap') for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=9 , border_type='nearest', kernel_size = 5) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=10, border_type='mirror', kernel_size = 5)for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=11, border_type='wrap', kernel_size = 5) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=12 , border_type='nearest', kernel_size = 7, threshold = 1) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=13, border_type='mirror', kernel_size = 7, threshold = 1) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test(imgid, imgdim, funcid=14, border_type='wrap', kernel_size = 7, threshold = 1) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)]+
           [gaussian_test_32(imgid, imgdim, funcid=15 , border_type='nearest', kernel_size = 7, threshold = 0) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test_32(imgid, imgdim, funcid=16, border_type='mirror', kernel_size = 7, threshold = 0) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)] +
           [gaussian_test_32(imgid, imgdim, funcid=17, border_type='wrap', kernel_size = 7, threshold = 0) for imgid,imgdim in enumerate(STANDART_GRAY_SIZES)]
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
    {
        "name" : "Image Transforms",
        "define": "TESTGROUP2",
        "inputs": [ImageGen([(128,128)],
                   format=Format.GRAY8,
                   path="Patterns/JellyBeans.tiff"),
                   ImageGen([(128,128)],
                   format=Format.RGB24,
                   path="Patterns/JellyBeans.tiff"),
                   ImageGen([(128,128)],
                   format=Format.BGR8U3C,
                   path="Patterns/JellyBeans.tiff")
        ],
        "tests":
          [gray8_crop_test((128,128),0,15,15,113,113),
           gray8_crop_test((128,128),1,16,16,112,112),
           gray8_crop_test((128,128),2,32,32,96,96),
           gray8_crop_test((128,128),3,47,47,81,81),
           gray8_crop_test((128,128),4,60,60,68,68),
           gray8_crop_test((128,128),5,16,60,112,68),
           rgb_crop_test  ((128,128),6,15,15,113,113),
           rgb_crop_test  ((128,128),7,16,16,112,112),
           rgb_crop_test  ((128,128),8,32,32,96,96),
           rgb_crop_test  ((128,128),9,47,47,81,81),
           rgb_crop_test  ((128,128),10,60,60,68,68),
           rgb_crop_test  ((128,128),11,16,60,112,68),
           gray8_resize_test((128,128),12,64,64),
           gray8_resize_test((128,128),13,16,16),
           gray8_resize_test((128,128),14,15,15),
           gray8_resize_test((128,128),15,47,17),
           gray8_resize_test((128,128),16,150,150),
           gray8_resize_test((128,128),17,256,256),
           bgr8U3C_resize_test((128,128),18,64,64),
           bgr8U3C_resize_test((128,128),19,16,16),
           bgr8U3C_resize_test((128,128),20,15,15),
           bgr8U3C_resize_test((128,128),21,47,17),
           bgr8U3C_resize_test((128,128),22,150,150),
           bgr8U3C_resize_test((128,128),23,256,256),
           bgr8U3C_resize_to_rgb_test((128,128),24,64,64),
           bgr8U3C_resize_to_rgb_test((128,128),25,16,16),
           bgr8U3C_resize_to_rgb_test((128,128),26,15,15),
           bgr8U3C_resize_to_rgb_test((128,128),27,47,17),
           bgr8U3C_resize_to_rgb_test((128,128),28,150,150),
           bgr8U3C_resize_to_rgb_test((128,128),29,256,256),
          ]
        
    },
    {
        "name" : "Feature Detection",
        "define": "TESTGROUP3",
        "inputs": [ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(512,512)],
                   format=Format.GRAY8,
                   path="Patterns/Ruler.tiff"),
                   ImageGen([(64,64)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff"),
                   ImageGen([(500,500)],
                   format=Format.GRAY8,
                   path="Patterns/Peppers.tiff"),
                   ImageGen([(250,250)],
                   format=Format.GRAY8,
                   path="Patterns/House2.tiff"),
                   ImageGen([(200,200)],
                   format=Format.GRAY8,
                   path="Patterns/House.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Girl3.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Gray21.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/House.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/House2.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/JellyBeans.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/JellyBeans2.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Moon.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Peppers.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/ResolutionChart.tiff"),
                   ImageGen(STANDARD_IMG_SIZES,
                   format=Format.GRAY8,
                   path="Patterns/Ruler.tiff"),
                   ],
        "tests":
          [canny_sobel_test_autoref(imgid, imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(0+len(STANDARD_IMG_SIZES), [512,512])]+
           [canny_sobel_test_autoref(1+len(STANDARD_IMG_SIZES), [64,64])]+
           [canny_sobel_test_autoref(2+len(STANDARD_IMG_SIZES), [500,500])]+
           [canny_sobel_test_autoref(3+len(STANDARD_IMG_SIZES), [250,250])]+
           [canny_sobel_test_autoref(4+len(STANDARD_IMG_SIZES), [200,200])]+
           [canny_sobel_test_autoref(imgid+5+len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+2*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+3*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+4*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+5*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+6*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+7*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+8*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+9*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]+
           [canny_sobel_test_autoref(imgid+5+10*len(STANDARD_IMG_SIZES), imgdim) for imgid,imgdim in enumerate(STANDARD_IMG_SIZES)]
    },
]

#print(allSuites[1]["tests"][78])
#exit(0)
#
# Only one test used for developping and debugging a new kernel
# If more than one image is passed, they are assumed to be all input of the test
# But generally the test will use only one input image
devTest = {
        "name" : "Linear Filters",
        "define": "TESTGROUP0",
        "inputs": [ImageGen([(32,8)],
                   format=Format.GRAY8,
                   path="Patterns/Mandrill.tiff")
                   ],
        "reference": GaussianFilter('nearest', 7),
        "check" : SimilarTensorFixp(0)
}