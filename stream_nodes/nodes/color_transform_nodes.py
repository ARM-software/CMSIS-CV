from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from .node_types import *

class YUVToRGB(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV)
        dst_t = CImageType(w,h,CImageType.RGB)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "YUVToRGB"

class YUVToGray16(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV)
        dst_t = CImageType(w,h,CImageType.GRAY16)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "YUVToGray16"

class YUVToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "YUVToGray8"


class Gray16ToRGB(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.RGB)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray16ToRGB"

class Gray8ToRGB(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.RGB)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray8ToRGB"

class Gray8ToGray16(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.GRAY16)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray8ToGray16"

class Gray16ToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray16ToGray8"

class Gray8ToRGBA(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.RGBA)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray8ToRGBA"

class Gray16ToRGBA(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.RGBA)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray16ToRGBA"

class RGBAToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.RGBA)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t._nb_bytes)
        self.addOutput("o",dst_t,dst_t._nb_bytes)

    def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "RGBAToGray8"