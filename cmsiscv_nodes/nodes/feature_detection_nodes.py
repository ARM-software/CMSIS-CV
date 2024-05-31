from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from .node_types import *

class CannyEdge(GenericNode):
    def __init__(self,name,w,h,firstAlgo=True,config=False):
        GenericSink.__init__(self,name)
        src_t = CImageType(w,h,CImageType.GRAY8)
<<<<<<< HEAD
<<<<<<< HEAD
        dst_t = CImageType(w,h,CImageType.GRAY8)
=======
        dst_t = CImageType(w,h,CImageType.GRAY16)
>>>>>>> 0eb2bec (update type for gaussian and cannyedge)
=======
        dst_t = CImageType(w,h,CImageType.GRAY8)
>>>>>>> 62e9e18 (changed output datatype for sobel)

        self.addInput("i",src_t,src_t.nb_bytes)
        self.addOutput("o",dst_t,dst_t.nb_bytes)
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
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "CannyEdge"

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)

    @property
    def paramBlock(self):
        return self._paramBlock

    @property
    def params(self):
        return {"name":"CMSIS-CV Canny Edge", 
                "values":
<<<<<<< HEAD
<<<<<<< HEAD
                  [{"value":78,"name":"low","min":0,"max":255,"step":1},
                   {"value":33,"name":"high","min":0,"max":255,"step":1}
=======
                  [{"value":108,"name":"low","min":0,"max":255,"step":1},
                   {"value":95,"name":"high","min":0,"max":255,"step":1}
>>>>>>> 0eb2bec (update type for gaussian and cannyedge)
=======
                  [{"value":78,"name":"low","min":0,"max":255,"step":1},
                   {"value":33,"name":"high","min":0,"max":255,"step":1}
>>>>>>> caadad4 (test browser)
                  ]
                }


