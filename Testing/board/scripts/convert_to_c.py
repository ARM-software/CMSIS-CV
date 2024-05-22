import argparse
from .export import serialize_tensors,read_tensors


def convert(filename,name,input):
    with open(input,"rb") as f:
        res = f.read()
        
    COLS = 10
    with open(f"{filename}.c","w") as c:
        nb = 0 
        l = len(res)
        print(f"""#include "{filename}.h"
#include "arm_math_types.h"
    
#ifndef ALIGN_NETWORK 
#define ALIGN_NETWORK __ALIGNED(16)
#endif
    
#if defined(CINPUT)
""",file=c)
        print("ALIGN_NETWORK",file=c)
        print(f"const uint8_t {name}[NB_{name.upper()}]={{",file=c,end="")
        for b in res:
            print("0x%02x," % b,file=c,end="")
            nb = nb + 1
            if (nb == COLS):
                nb = 0 
                print("",file=c)
        print("};",file=c)
        print("#endif",file=c)
    
    with open(f"{filename}.h","w") as c:
        nb = 0 
        l = len(res)
        print(f"""#ifndef {name.upper()}_H 
#define {name.upper()}_H 
    
#include "arm_math_types.h"
    
#ifdef   __cplusplus
extern "C"
{{
#endif
    
#if defined(CINPUT)
    """,file=c)
        print(f"#define NB_{name.upper()} {l}",file=c)
        print(f"extern const uint8_t {name}[NB_{name.upper()}];",file=c)
        
        print("""
#endif

#ifdef   __cplusplus
}
#endif
""",file=c)
    
        print("#endif",file=c)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='convert_to_c',
                    description='Convert network to a C array')

    parser.add_argument('filename')
    parser.add_argument('-n', '--name',default="network")
    parser.add_argument('-i', '--input')
    
    args = parser.parse_args()

    convert(args.filename,args.name,args.i)