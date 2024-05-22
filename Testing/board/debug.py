from scripts.export import *

with open("results/output_0.dat","rb") as f:
         r = read_tensors(f)

for i in r:
    print(i.tensor)