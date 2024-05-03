from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from .node_types import *


class GaussianFilter(GenericNode):
    def __init__(self,name,w,h,firstAlgo=True,config=False):
        GenericSink.__init__(self,name)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.GRAY16)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

        if not config:
            self._paramBlock = 0
        else:
            if firstAlgo:
               self._paramBlock = 1
            else:
               self._paramBlock = 2

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "GaussianFilter"

    @property
    def paramBlock(self):
        return self._paramBlock

    @property
    def params(self):
        return {"name":"CMSIS-CV Gaussian Filter"}
