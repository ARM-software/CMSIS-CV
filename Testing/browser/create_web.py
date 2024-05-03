import argparse
import shutil
import os
import sys

parent_directory = os.path.abspath('../../stream_nodes')
sys.path.append(parent_directory)


from cmsis_stream.cg.scheduler import *

from stream.create import create_cv_test,create_cv_test_gen

from stream.tests import *

from jinja2 import Environment, FileSystemLoader, select_autoescape


from colorama import just_fix_windows_console
just_fix_windows_console()

env = Environment(
    loader=FileSystemLoader("templates"),
    autoescape=select_autoescape()
)

parser = argparse.ArgumentParser(
                    prog='create_web',
                    description='Generate scheduler and web pages for all the tests')

parser.add_argument('-cv',action='store_true')
parser.add_argument('--width', default=320,type=int)
parser.add_argument('--height', default=240,type=int)

args = parser.parse_args()

# GenericNodes.h is created in the folder "generated"
# Generic nodes was customized to use different FIFO class
# so it must not be overidden by the default version.
#generateGenericNodes(".")
    
# cg_status.h is created in the folder "generated"
generateCGStatus(".")

class _T:
    def __init__(self,title):
       self._title = title 

    @property
    def title(self):
        return self._title

class H1(_T):
    pass

class H2(_T):
    pass

class Test:
    def __init__(self,func,cname,title,regenerate=True,hasCV = True):
        self._func = func 
        self._cname = cname
        self._title = title 
        self._regenerate = regenerate 
        self._hasCV = hasCV 

    @property
    def func(self):
        return self._func

    @property
    def cname(self):
        return self._cname

    @property
    def title(self):
        return self._title

    @property
    def regenerate(self):
        return self._regenerate

    @property
    def hasCV(self):
        return self._hasCV
    
    
    
    
    
    

# Test func, c name and folder, title
TESTS=[H1("Feature Detection"),
  Test(mk_canny_edge,"canny_edge","Canny Edge",regenerate=True),
  H1("Linear Filter"),
  Test(mk_gaussian,"gaussian","Gaussian Filter",hasCV=True)
]

for test in TESTS:
    if isinstance(test,Test):
   
       if test.regenerate:
          if test.hasCV:
             g = test.func(OPENCV=True,W=args.width,H=args.height)
             create_cv_test(g,env,test.cname,test.title,OPENCV=True,W=args.width,H=args.height)
      
          g = test.func(OPENCV=False,W=args.width,H=args.height)
          create_cv_test(g,env,test.cname,test.title,OPENCV=False,W=args.width,H=args.height)
          
          create_cv_test_gen(env,test.cname,test.title)


exported_list = ["malloc"]
for t in TESTS:
    if isinstance(t,Test):
       exported_list.append(f"{t.cname}_init_cv")
       exported_list.append(f"{t.cname}_stream")
       exported_list.append(f"{t.cname}_ocv_init_cv")
       exported_list.append(f"{t.cname}_ocv_stream")

exported_tests = []
for t in TESTS:
    if isinstance(t,Test):
       exported_tests.append(t)
    if isinstance(t,H1):
        exported_tests.append({"h1":t.title})

exported = env.get_template("exported.cmake")
with open(f"exported.cmake","w") as f:
        print(exported.render(tests=exported_list),file=f)

roothtml = env.get_template("root.html")
with open(f"web/index.html","w") as f:
        print(roothtml.render(tests=exported_tests,
            H1=H1,
            H2=H2,
            isinstance=isinstance),file=f)