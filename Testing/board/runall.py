import argparse
import pickle
import sys
import subprocess 
import os 
import glob
import sys 

# Description of all configurations compiler / cores are defined
# in this file
from test_configs import *

oldprint = print 

from rich import print
from rich.live import Live
from rich.table import Table


from scripts.export import *
from scripts.test_utils import *
from test_desc import *

from pathlib import Path

Path("references").mkdir(parents=True, exist_ok=True)
Path("inputs").mkdir(parents=True, exist_ok=True)
Path("results/img").mkdir(parents=True, exist_ok=True)

parser = argparse.ArgumentParser(description='Parse test description')
parser.add_argument('-avh', nargs='?',type = str, default="C:/Keil_v5/ARM/VHT", help="AVH folder")
parser.add_argument('-d', action='store_true', help="Debug log for command launch")
parser.add_argument('-n', action='store_true', help="No force rebuild")
parser.add_argument('--gen', action='store_true', help="Generate inputs and references")
parser.add_argument('--results', action='store_true', help="Generate test result images")
parser.add_argument('--dev', action='store_true', help="Kernel development mode")

# In norun, the .dat is generated and the project build
# but no AVH is run
parser.add_argument('--norun', action='store_true', help="Don't run in dev mode")
parser.add_argument('--nobuild', action='store_true', help="Don't rebuild")

parser.add_argument('--group', nargs='?',type = int, default=None, help="Only test group to run")
parser.add_argument('--test', nargs='+',type = int, default=None, help="Only test to run in a test group")
parser.add_argument('--passed', action='store_true', help="Display also passing test in test report")

args = parser.parse_args()



DEBUG=False 
if args.d:
    DEBUG=True


sys.path.append("..")

ERROR_OCCURED = False

def printTitle(s):
    print(f"\n[bold green]{s}[/bold green]")

def printSubTitle(s):
    print(f"\n[bold yellow]{s}[/bold yellow]")

def printError(s):
    print(f"\n[bold red]{s}[/bold red]")

def printMessage(s):
    print(f"\n[bold cyan]{s}[/bold cyan]")


def getTitle(l,s):
    return(f"\n[bold green]{s}[/bold green]")

def getSubTitle(l,s):
    return(f"\n[bold yellow]{s}[/bold yellow]")

def getError(l,s):
    return(f"\n[bold red]{s}[/bold red]")

def getMessage(l,s):
    return(f"\n[bold cyan]{s}[/bold cyan]")

class Result:
    def __init__(self,msg,error=False):
        self._error = error
        self._msg = msg

    @property
    def error(self):
        return self._error

    @property
    def msg(self):
        return self._msg
    
    

# Run a command and get error or result
# For the test report we don't need the stderr
# in case of error since the test report is giving
# all the details. So, there is an option to
# disable the dump of stderr
def run(*args,mustPrint=False,dumpStdErr=True,live=None):
    global ERROR_OCCURED
    global DEBUG
    try:
        if DEBUG:
            live.console.print(" ".join(args))
        result=subprocess.run(args,text=True,capture_output=True,timeout=600)
        if result.returncode !=0 :
             ERROR_OCCURED = True
             if dumpStdErr:
                return(Result(result.stderr + "\n\nSTDOUT:\n\n" + result.stdout,error=True))
             else:
                return(Result(result.stdout,error=True))

        if mustPrint:
            live.console.print(result.stdout)
        return(Result(result.stdout))
    except Exception as e:
        if DEBUG:
           printError(live,"Exception occured")
        ERROR_OCCURED = True
        return(Result(str(e),error=True))




   
# Configuration file for AVH core
configFiles={
    "CS300":"VHT-Corstone-300.txt",
    "M7DP":"VHT-M7DP.txt",
    "M4FP":"VHT-M4.txt",
    "M0plus":"VHT-M0P.txt",
}

# Windows executable
# (At some point this script will also support
# unix)
avhExe={
    "CS310":"VHT_Corstone_SSE-310.exe",
    "CS300":"VHT_Corstone_SSE-300_Ethos-U55.exe",
    "M55":"VHT_MPS2_Cortex-M55.exe",
    "M33_DSP_FP":"VHT_MPS2_Cortex-M33.exe",
    "M7DP":"VHT_MPS2_Cortex-M7.exe",
    "M4FP":"VHT_MPS2_Cortex-M4.exe",
    "M3":"VHT_MPS2_Cortex-M3.exe",
    "M23":"VHT_MPS2_Cortex-M23.exe",
    "M0plus":"VHT_MPS2_Cortex-M0plus.exe",
}

AVHROOT = args.avh

# Run AVH
def runAVH(live,build,core):
    axf="cprj/out/cmsiscv/%s/Release/cmsiscv.axf" % (build,)
    elf="cprj/out/cmsiscv/%s/Release/cmsiscv.elf" % (build,)
    app = axf 
    if os.path.exists(axf):
        app = axf 
    if os.path.exists(elf):
        app = elf
    config = os.path.join("fvp_configs",configFiles[core])
    avh = os.path.join(AVHROOT,avhExe[core])
    res=run(avh,"-f",config,app,live=live)
    return(res)
   
def clean_old_results():
    g = glob.glob("results/*.dat")
    for f in g:
        os.remove(f)




HTMLHEADER="""<html>
<header>
<title>CMSIS-CV Test summary</title>
</header>
<body>
""" 

HTMLFOOTER="""
</body></html>
"""

# We can select a specific test only when a specific group has been selected
if args.group is None:
    args.test = None



def gen_table(the_list):
    table = Table()
    table.add_column("Compiler",width=15,vertical="top")
    table.add_column("Target",width=25,vertical="top")
    table.add_column("Test Group",width=20,vertical="top")
    table.add_column("Status",width=20,vertical="top")

    for i in the_list:
        table.add_row(*i)
    return table 

if args.dev:
   args.results = True
   printMessage("Kernel development mode enabled")
   allSuites = mk_test_suite_from_dev(devTest)
   #print(allSuites)
if args.gen:
   printMessage("Generate input images")
   # Generate all input images for all test groups
   # Can be used for debugging an error
   for group_id,testSuite in enumerate(allSuites):
       print(f"Generate group {group_id}")
       clean_input_images(args,group_id)
       generate_input_images(args,group_id,testSuite)
   
   printMessage("Generate reference images")
   # Generate all reference images
   for group_id,testSuite in enumerate(allSuites):
       print(f"Generate group {group_id}")
       clean_reference_images(args,group_id)
       generate_reference_images(args,group_id,testSuite)

if args.results:
   printMessage("Result images will be recorded (images or .npy files)")

if args.n:
   printMessage("Solution will not be fully rebuilt")
   print("It is ok if you don't mix different target and compilers")

if args.norun:
   printMessage("Tests won't be executed")

if args.nobuild:
   printMessage("Tests won't be built")

if not args.group is None:
    printMessage(f"Only group {args.group} will be built/run")
    if not args.test is None:
        printMessage(f"Only test {args.test} will be built/run")

# Run the tests and log the result
# in a summary.html file

MAX_ROWS=4
latest=[]
with Live(gen_table([]), refresh_per_second=4) as live: 

    with open("summary.html","w") as f:
        print(HTMLHEADER,file=f)
        nb_compilers = len(compiler)
        for compid,c in enumerate(compiler):
            #printTitle(live,f"Process compiler {c} ({compid+1}/{nb_compilers})")
            tableCompiler = getTitle(live,f"{c} ({compid+1}/{nb_compilers})")
            #if res.error:
            #    printError("Error csolution")
            #    print("<p><font color=\"red\">Error converting csolution %s</font></p><PRE>" % s,file=f)
            #    print(res.msg,file=f)
            #    print("</PRE>",file=f)
            #    continue
            print("<h1>Compiler %s</h1>" % c,file=f)
            maxNbBuilds=len(compiler[c])
            buildNb=0
            for target_core in compiler[c]:
                if len(target_core)>1:
                    build,core = target_core
                else:
                    build = target_core[0]
                    core = None
                buildNb = buildNb + 1
                print("<h2>Target %s</h2>" % build,file=f)
                #printTitle(live,"Process target %s (%d/%d)" % (build,buildNb,maxNbBuilds))
                tableTarget = getTitle(live,"%s (%d/%d)" % (build,buildNb,maxNbBuilds))
                buildFile="cmsiscv.Release+%s" % build
                maxNb = len(allSuites)
    
    
                for group_id,testSuite in enumerate(allSuites):
                    if (len(latest)>MAX_ROWS):
                        latest=latest[1:]

                    if (args.group is None) or args.group == group_id:
                       if args.group is None:
                          #printSubTitle(live,f'Process group : {testSuite["name"]} ({group_id+1}/{maxNb})')
                          tableGroup=getSubTitle(live,f'{testSuite["name"]} ({group_id+1}/{maxNb})')
                       else:
                          #printSubTitle(live,f'Process group : {testSuite["name"]}')
                          tableGroup=getSubTitle(live,f'{testSuite["name"]}')
                       latest.append([tableCompiler,tableTarget,tableGroup,"?"])

                       imgs = prepare_tests_group(args,group_id,testSuite)
                       #live.console.print("Building ...")
                       if not args.nobuild:
                          msg = "Building..."
                          latest[-1][-1]=msg
                          if (not args.group is None or group_id==0):
                             # -r is needed for first
                             # build when we switch
                             # between different solutions
                             # (Like one using AC6 and the other
                             # using gcc)
                             if args.n:
                                #live.console.print("Incremental build")
                                msg += " Incremental build"
                                latest[-1][-1]=msg
                                live.update(renderable=gen_table(latest))
                                res=run("cbuild","-O" ,"cprj",'cmsiscv.csolution.yml',"--toolchain" ,c,"-c",buildFile,live=live)
                             else:
                                #live.console.print("Rebuild all (and RTE update)")
                                msg += " Rebuild all (and RTE update)"
                                latest[-1][-1]=msg
                                live.update(renderable=gen_table(latest))
                                res=run("cbuild","-O" ,"cprj",'cmsiscv.csolution.yml',"--update-rte","-r","--toolchain" ,c,"-c",buildFile,live=live)
                          else:
                             #live.console.print("Incremental build")
                             msg += " Incremental build"
                             latest[-1][-1]=msg
                             live.update(renderable=gen_table(latest))
                             res=run("cbuild","-O" ,"cprj",'cmsiscv.csolution.yml',"--toolchain" ,c,"-c",buildFile,live=live)
                       
                       
                          if res.error:
                              latest[-1][-1]="[red]Error cbuild"
                              live.update(renderable=gen_table(latest))
                              #printError(live,"Error cbuild")
                              print(f'<p><font color="red">Error building {testSuite["name"]}</font></p><PRE>',file=f)
                              print(res.msg,file=f)
                              print("</PRE>",file=f)
                              continue
                       if not args.norun and (not core is None):
                           latest[-1][-1]="Run AVH"
                           live.update(renderable=gen_table(latest))
                           #printSubTitle(live,"Run AVH")
                           clean_old_results()
                           res=runAVH(live,build,core)
                           if res.error:
                               latest[-1][-1]="[red]Error running AVH"
                               live.update(renderable=gen_table(latest))
                               #printError(live,"Error running AVH")
                               print(f'<p><font color="red">Error running {testSuite["name"]} with {avhExe[core]}</font></p><PRE>',file=f)
                               print(res.msg,file=f)
                               print("</PRE>",file=f)
                               continue
                           else:
                               had_error = validate_test_group(args,c,build,group_id,testSuite,imgs,f)
                               if had_error:
                                   ERROR_OCCURED = True
                                   latest[-1][-1]="[red]Failed"
                                   live.update(renderable=gen_table(latest))
                                   continue
                       # In case of no issue, we drop the status
                       # Status table only contain failure
                       latest=latest[:-1]
                       live.update(renderable=gen_table(latest))
                       
                   
        print(HTMLFOOTER,file=f)

# Refresh cursor
oldprint('\033[?25h', end="")

if ERROR_OCCURED:
    sys.exit("Error occurred")
else:
    sys.exit(0)



