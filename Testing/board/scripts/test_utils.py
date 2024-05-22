from PIL import ImageOps
import PIL
import numpy as np 
from pathlib import Path
import glob

# Start of input tensors
TENSOR_START = 2

from .export import *
from .convert_to_c import convert

class Format:
    GRAY8 = 0
    RGB = 1

class Image:
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

def check_tiff_or_npy(path):
    if Path(path + ".tiff").is_file():
        path += ".tiff"
    else:
        path += ".npy"
    return(path)

# Management of input images
def get_input_img(args,group_id,image_id):
    if args.dev:
       path = f"inputs/dev/img_{image_id}"
    else:
       path = f"inputs/group_{group_id}/img_{image_id}"

    path = check_tiff_or_npy(path)
    return (AlgoImage.open(path))

def clean_input_images(args,group_id):
    if args.dev:
       Path(f"inputs/dev").mkdir(parents=True, exist_ok=True)
       path = f"inputs/dev/"
    else:
      Path(f"inputs/group_{group_id}").mkdir(parents=True, exist_ok=True)
      path = f"inputs/group_{group_id}"

    g = glob.glob(f"{str(path)}/*.tiff")
    for f in g:
        if args.d:
            print(f"Remove {f}")
        os.remove(f)

    g = glob.glob(f"{str(path)}/*.npy")
    for f in g:
        if args.d:
            print(f"Remove {f}")
        os.remove(f)

def record_input_img(args,group_id,image_id,image):
    if args.dev:
       Path(f"inputs/dev").mkdir(parents=True, exist_ok=True)
       path = f"inputs/dev/img_{image_id}"
    else:
      Path(f"inputs/group_{group_id}").mkdir(parents=True, exist_ok=True)
      path = f"inputs/group_{group_id}/img_{image_id}"
    path = check_tiff_or_npy(path)
    image.save(path)

# References

def clean_reference_images(args,group_id):
    if args.dev:
       Path(f"references/dev").mkdir(parents=True, exist_ok=True)
       path = f"references/dev"
    else:
       Path(f"references/group_{group_id}").mkdir(parents=True, exist_ok=True)
       path = f"references/group_{group_id}"

    g = glob.glob(f"{str(path)}/*.tiff")
    for f in g:
        if args.d:
            print(f"Remove {f}")
        os.remove(f)

    g = glob.glob(f"{str(path)}/*.npy")
    for f in g:
        if args.d:
            print(f"Remove {f}")
        os.remove(f)


def record_reference_img(args,group_id,test_id,image_id,image):
    if args.dev:
       Path(f"references/dev").mkdir(parents=True, exist_ok=True)
       path = f"references/dev/test_{test_id}_img_{image_id}"
    else:
       Path(f"references/group_{group_id}").mkdir(parents=True, exist_ok=True)
       path = f"references/group_{group_id}/test_{test_id}_img_{image_id}"

    path = check_tiff_or_npy(path)
    image.save(path)

def get_reference_img(args,group_id,test_id,image_id):
    if args.dev:
       path = f"references/dev/test_{test_id}_img_{image_id}"
    else:
       path = f"references/group_{group_id}/test_{test_id}_img_{image_id}"

    path = check_tiff_or_npy(path)
    return (AlgoImage.open(path))

# Results
# Should depend on compiler and test suite
def record_result_img(args,compiler,target,group_id,test_id,image_id,image):
    if args.dev:
       Path(f"results/img/{compiler}/{target}/dev").mkdir(parents=True, exist_ok=True)
       path = Path(f"results/img/{compiler}/{target}/dev/test_{test_id}_img_{image_id}")
    else:
       Path(f"results/img/{compiler}/{target}/group_{group_id}").mkdir(parents=True, exist_ok=True)
       path = Path(f"results/img/{compiler}/{target}/group_{group_id}/test_{test_id}_img_{image_id}")
    path = check_tiff_or_npy(str(path))
    image.save(path)




def generate_reference_images(args,group_id,test):
    for test_id,t in enumerate(test["tests"]):
        images = [get_input_img(args,group_id,x) for x in t["useimg"]]
        # generate references for this group, test
        t["reference"](args,group_id,test_id,images)

def get_reference_imgs(args,group_id,test_id,test,inputs):
    nb = test["reference"].nb_references(inputs)
    refs = []
    for i in range(nb):
        refs.append(get_reference_img(args,group_id,test_id,i))
    return(refs)


def generate_input_images(args,group_id,test):
    inputs = test["inputs"]

    image_id = 0
    for i in inputs:
        new_imgs = i()
        for im in new_imgs:
            record_input_img(args,group_id,image_id,im)
            image_id = image_id + 1


def _mk_group_input(args,group_id,test):
    inputs = test["inputs"]
    nb_tests = len(test["tests"])
    if (args.group is None) or (args.test is None):
       enabled = np.ones(nb_tests,dtype=np.uint32)
    else:
       enabled = np.zeros(nb_tests,dtype=np.uint32)
       enabled[args.test] = 1
    imgs = [] 

    funcid = []
    for t in test["tests"]:
        funcid.append(t["funcid"])

    # Add list of funcid to run for the tests
    imgs.append(AlgoImage(np.array(funcid,dtype=np.uint32)))

    # Add list of test to run
    imgs.append(AlgoImage(enabled))

    image_id = 0
    for i in inputs:
        for _ in range(i.nb_images):
            im = get_input_img(args,group_id,image_id)
            image_id = image_id + 1
            imgs.append(im)


    return(imgs)

def _validate_test(refs,results,test):
    return test["check"](refs,results)

def prepare_tests_group(args,group_id,test):
    ginput = _mk_group_input(args,group_id,test)
    with open("test_config.h","w") as f:
        print(f"""#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

#define {test["define"]}

#endif
""",file=f)

    with open("test.dat","wb") as f:
        serialize_tensors(f,ginput)

    convert("cinput","cinput","test.dat")

    return(ginput[TENSOR_START:])

def save_results(args,compiler,target,group_id,test_id,results):
    for image_id,img in enumerate(results):
        record_result_img(args,compiler,target,group_id,test_id,image_id,img)



def validate_test_group(args,compiler,target,group_id,test,imgs,report_file):
    had_error = False
    print(f'<h3>{test["name"]}</h3>',file=report_file)
    resultsSaved = False
    results = None
    for test_id,t in enumerate(test["tests"]):
        result_path = f"results/output_{test_id}.dat"
        try:
            with open(result_path,"rb") as f:
                 results = read_tensors(f)

            if args.results:
               save_results(args,compiler,target,group_id,test_id,results)
               resultsSaved = True

            refs = get_reference_imgs(args,group_id,test_id,t,t["useimg"])
            if not _validate_test(refs,results,t):
                print(f'''<p><font color=\"red\">Error running {t["desc"]}</font></p><PRE>
{t["check"].errors}
                </PRE>
''',file=report_file)
                had_error = True
        except Exception as e:
            print(f'''<p><font color=\"red\">Exception running {t["desc"]}</font> </p><p>Exception raised</p><PRE>
{str(e)}
</PRE>
''',file=report_file)
            had_error = True

    if had_error and not resultsSaved:
        if not (results is None):
           save_results(args,compiler,target,group_id,test_id,results)
    return had_error

def mk_test_suite_from_dev(d):
    t = {
        "name" : "Development",
        "define": "TESTDEV",
        "inputs": [],
        "tests":[
           {"desc":"Kernel being developped",
            "funcid": 0,
            "useimg": [0],
            "reference": None,
            "check" : None
           }
           ]
        }

    t["inputs"] = d["inputs"]
    nb = 0 
    for i in t["inputs"]:
        nb = nb + i.nb_images

    # All images are input of the dev kernels
    t["tests"][0]["useimg"]=[x for x in range(nb)]
    t["tests"][0]["reference"] = d["reference"]
    t["tests"][0]["check"] = d["check"]

    return([t])
