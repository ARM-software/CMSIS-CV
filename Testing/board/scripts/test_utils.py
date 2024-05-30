import numpy as np 
from pathlib import Path
import glob

# Start of input tensors
TENSOR_START = 2

from .export import *
from .convert_to_c import convert


def _check_tiff_or_npy(path):
    if Path(path + ".tiff").is_file():
        path += ".tiff"
    elif Path(path + "_yuv420.npy").is_file():
        path += "_yuv420.npy"
    else:
        path += ".npy"
    return(path)

# Management of input images
def get_input_img(args,group_id,image_id):
    if args.dev:
       path = f"inputs/dev/img_{image_id}"
    else:
       path = f"inputs/group_{group_id}/img_{image_id}"

    path = _check_tiff_or_npy(path)
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
    path = _check_tiff_or_npy(path)
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

    path = _check_tiff_or_npy(path)
    image.save(path)

def get_reference_img(args,group_id,test_id,image_id):
    if args.dev:
       path = f"references/dev/test_{test_id}_img_{image_id}"
    else:
       path = f"references/group_{group_id}/test_{test_id}_img_{image_id}"

    path = _check_tiff_or_npy(path)
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
    path = _check_tiff_or_npy(str(path))
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


def list_of_enabled_tests(args,group_id,test):
    nb_tests = len(test["tests"])
    if (args.group is None) or (args.test is None):
       enabled = np.ones(nb_tests,dtype=np.uint32)
    else:
       enabled = np.zeros(nb_tests,dtype=np.uint32)
       for i in args.test:
           enabled[i] = 1
    return enabled


def _mk_group_input(args,group_id,test):
    inputs = test["inputs"]
    
    enabled = list_of_enabled_tests(args,group_id,test)
    
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

    enabled = list_of_enabled_tests(args,group_id,test)

    for test_id,t in enumerate(test["tests"]):
        if enabled[test_id] == 1:
           result_path = f"results/output_{test_id}.dat"
           try:
               with open(result_path,"rb") as f:
                    results = read_tensors(f)
   
               if args.results:
                  save_results(args,compiler,target,group_id,test_id,results)
                  resultsSaved = True
   
               refs = get_reference_imgs(args,group_id,test_id,t,t["useimg"])
               if not _validate_test(refs,results,t):
                   print(f'''<p><font color=\"red\">Error running : {t["desc"]} (Test ID = {test_id})</font></p><PRE>
{t["check"].errors}
                </PRE>
''',file=report_file)
                   had_error = True
               elif args.passed:
                   print(f'<p>Passed : {t["desc"]} (Test ID = {test_id})</p>',file=report_file)

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
