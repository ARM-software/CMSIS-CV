# README

The CMSIS-Stream node must follow a specific API to work with the web environment.

The creation API is:

```python
def __init__(self,name,w,h,firstAlgo=True,config=False):
```

* `name` Name identification and name of variable in generated CMSIS-Stream scheduler
* `w` and `h` : Width and height of the processed image
* `firstAlgo` :  The web UI has two set of parameter controls. If true, this node is controlled by the first set
* `config` : If `False` this node is not controlled by the web UI so it is using its default parameters

The constructor must define argument of the C++ nodes for the width, height and parameters:

```python
self.addLiteralArg(w)
self.addLiteralArg(h)
```

And for the parameters:

```python
if not config:
    self._paramBlock = 0
    self.addVariableArg("nullptr")
else:
    if firstAlgo:
        self._paramBlock = 1
        self.addVariableArg("params1")
    else:
        self._paramBlock = 2
        self.addVariableArg("params2")
```

To make it easier to create a graph, the node are providing a functional interface.

Instead of having to write something like:

```python
g.connect(a.o,b.i)
```

One can write:

```
result = b(a)
```

For this, the node must implement `__call__`:

```python
def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
```

The default implementation is connecting the first found output to the input of this node.

Finally, the node must describe its parameters for the web UI:

```python
@property
def paramBlock(self):
    return self._paramBlock
```

This property has been initialized int he constructor and describes if the block of parameters used to control the nod in the web UI (1 or 2) or 0 if no UI to control this node

The list of parameters and their bounds are described with:

```python
@property
def params(self):
    return {"name":"CMSIS-CV Canny Edge", 
            "values":
                 [{"value":2500,"name":"low","min":0,"max":3000,"step":10},
                  {"value":1060,"name":"high","min":0,"max":4500,"step":10}
                 ]
              }
```

The title is used above the parameter control UI.

Then each parameter is described by giving:

* The default value
* The name of the parameter
* The settings to use for the slider

In the code, the parameter can be accessed from the array `params1` or `params2`. It is a `uint32_t` array. The parameters are in the same order in the array than in the Python list describing them.

For instance, in above example, the parameter `low` is at index `0` in the `params` array.

