from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from nodes import *

from .opencv_nodes import *


class WebDisplay(GenericSink):
    def __init__(self,name,w,h,buf="dst1"):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.RGBA32)
        self.addInput("i",src_t,src_t.nb_bytes)

        self.i.setBufferConstraint(name=f"{buf}",mustBeArray=True,assignedByNode=False)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "WebDisplay"

class WebDisplay1(WebDisplay):
    def __init__(self,name,w,h):
        WebDisplay.__init__(self,name,w,h,buf="dst1")

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "WebDisplay"

class WebDisplay2(WebDisplay):
    def __init__(self,name,w,h):
        WebDisplay.__init__(self,name,w,h,buf="dst2")

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "WebDisplay"

class WebCamera(GenericSource):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        dst_t = CImageType(w,h,CImageType.RGBA32)
        self.addOutput("o",dst_t,dst_t.nb_bytes)
        self.o.setBufferConstraint(name="src",mustBeArray=True,assignedByNode=False)


    def __call__(self,g):
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "WebCamera"

class Copy(GenericNode):
    def __init__(self,name,w,h):
        GenericNode.__init__(self,name,identified=False)
        dst_t = CImageType(w,h,CImageType.RGBA32)
        self.addInput("i",dst_t,dst_t.nb_bytes)
        self.addOutput("o",dst_t,dst_t.nb_bytes)


    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Copy"
