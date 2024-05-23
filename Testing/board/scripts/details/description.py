from . import *

# Function to make it easier to write tests
# It is an example
def linear_copy_test(imgid,imgdim,funcid=0,img_type="gray8"):
    return {"desc":f"Copy {img_type} image {imgdim[0]}x{imgdim[1]}",
            "funcid": funcid,
            "useimg": [imgid],
            "reference": CopyInput(),
            "check" : IdenticalImage()
           }