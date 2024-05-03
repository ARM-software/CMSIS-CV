import sys 
import os 
from colorama import Fore, Back
import colorama

from jinja2 import Environment, FileSystemLoader, select_autoescape

from cmsis_stream.cg.scheduler import *



from subprocess import run



class MyStyle(Style):
        def edge_label(self,fifo,typeName,length):
            #return f"{typeName}<BR/>({length})"
            return f"{typeName}"

def create_cv_test(the_graph,env,name,title,OPENCV=True,W=640,H=480):

    suffix=""
    if OPENCV:
        suffix = "_ocv"

    try:
       os.makedirs(f"stream/tests/{name}")
    except Exception as e:
        pass

    try:
        os.makedirs(f"web/{name}")
    except Exception as e:
        pass

    
    # Create a configuration object
    conf=Configuration()
    # Disable inclusion of CMSIS-DSP headers so that we don't have
    # to recompile CMSIS-DSP for such a simple example
    conf.CMSISDSP = False
    conf.asynchronous = False
    conf.callback = True
    conf.heapAllocation = True
    
    # Argument of the scheduler function.
    # Variables used in the initialization of some nodes
    conf.cOptionalArgs=["unsigned char * src",
                        "unsigned char * dst1",
                        "unsigned char * dst2",
                        "uint32_t* params1",
                        "uint32_t* params2"]
    
    # Name of files included by the scheduler
    conf.appNodesCName="AppNodes.h"
    conf.customCName="cv_app_settings.h"
    conf.schedulerCFileName=f"cv_{name}{suffix}_scheduler"
    # Prefix for FIFO buffers
    conf.prefix="cv_"
    # Name of scheduler function
    conf.schedName=f"cv_{name}{suffix}_scheduler"
    conf.bufferAllocation = True
    conf.memoryOptimization = True 
    conf.horizontal = True 
    #conf.disableDuplicateOptimization = True
    #conf.nodeIdentification=True
    
    # Compute a static scheduling of the graph 
    # The size of FIFO is also computed
    # 
    # 
    # 
    
    
    with open(f"stream/tests/{name}/pre_schedule_cv{suffix}.dot","w") as f:
        the_graph.graphviz(f)
    
    run(["dot","-Tpdf","-o",f"stream/tests/{name}/pre_schedule_cv{suffix}.pdf",f"stream/tests/{name}/pre_schedule_cv{suffix}.dot"])
    
    scheduling = the_graph.computeSchedule(config=conf)
    
    # Print some statistics about the compute schedule
    # and the memory usage
    # 
    print(f"{Fore.GREEN}Test {name}{suffix}{colorama.Style.RESET_ALL}")
    print(f"Schedule length = %d" % scheduling.scheduleLength)
    print("Memory usage %d bytes\n" % scheduling.memory)
    
    # Generate JSON configuration for js UI
    json = {}
    hasParam1 = False
    hasParam2 = False
    for n in scheduling.nodes:
        try:
            if n.paramBlock==1:
                json["param1"] = n.params
                hasParam1 = True
            elif n.paramBlock==2:
                json["param2"] = n.params
                hasParam2 = True
        except Exception as e:
            pass

    json["sched_init"] = f"{name}{suffix}_init_cv"
    json["sched_run"] =f"{name}{suffix}_stream"
    json["width"] = W
    json["height"] = H

    with open(f"web/{name}/uiconfig{suffix}.js","w") as f:
        print(f"var uiconfig={json};",file=f)
    
    # Generate the C++ code for the static scheduler
    scheduling.ccode(f"stream/tests/{name}",conf)
    
    # The generated code is including GenericNodes.h and 
    # cg_status.h
    # Those files can either be used from the CMSIS-Stream 
    # repository or they can be generated from the Python 
    # package so that it is easier to start using CMSIS-Stream
    
   
    
    
    myStyle = MyStyle()
    
    # Generate a graphviz representation of the graph
    with open(f"stream/tests/{name}/cv{suffix}.dot","w") as f:
        scheduling.graphviz(f,config=conf,style=myStyle)
    
    run(["dot","-Tpdf","-o",f"stream/tests/{name}/cv{suffix}.pdf",f"stream/tests/{name}/cv{suffix}.dot"])
    run(["dot","-Tsvg","-o",f"web/{name}/cv{suffix}.svg",f"stream/tests/{name}/cv{suffix}.dot"])
    
    indexhtml = env.get_template("index.html")
    with open(f"web/{name}/index{suffix}.html","w") as f:
        print(indexhtml.render(name=name,
            title=title,
            suffix=suffix,
            width=W,
            height=H,
            OPENCV=OPENCV,
            hasParam1=hasParam1,
            hasParam2=hasParam2),file=f)

    indexhtmlhor = env.get_template("index_horizontal.html")
    with open(f"web/{name}/index_horizontal{suffix}.html","w") as f:
        print(indexhtmlhor.render(name=name,
            title=title,
            suffix=suffix,
            width=W,
            height=H,
            OPENCV=OPENCV,
            hasParam1=hasParam1,
            hasParam2=hasParam2),file=f)

    streamc = env.get_template("stream.c")
    with open(f"stream/tests/{name}/stream{suffix}.c","w") as f:
        print(streamc.render(name=name,
            title=title,
            suffix=suffix),file=f)

    

    
def create_cv_test_gen(env,name,title):
    cmake = env.get_template("CMakeLists.txt")
    with open(f"stream/tests/{name}/CMakeLists.txt","w") as f:
        print(cmake.render(name=name,title=title),file=f)


