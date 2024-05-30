from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from .node_types import *

class YUV420ToRGB24(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV420)
        dst_t = CImageType(w,h,CImageType.RGB24)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "YUV420ToRGB24"

class YUV420ToGray16(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV420)
        dst_t = CImageType(w,h,CImageType.GRAY16)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "YUV420ToGray16"

class YUV420ToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.YUV420)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "YUV420ToGray8"


class Gray16ToRGB24(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.RGB24)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray16ToRGB24"

class Gray8ToRGB24(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.RGB24)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray8ToRGB24"

class Gray8ToGray16(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.GRAY16)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray8ToGray16"

class Gray16ToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray16ToGray8"

class Gray8ToRGBA32(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY8)
        dst_t = CImageType(w,h,CImageType.RGBA32)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray8ToRGBA32"

class Gray16ToRGBA32(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.GRAY16)
        dst_t = CImageType(w,h,CImageType.RGBA32)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "Gray16ToRGBA32"

class RGBA32ToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.RGBA32)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "RGBA32ToGray8"

class RGB24ToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.RGB24)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "RGB24ToGray8"

class BGR_8U3CToRGB24(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.BGR_8U3C)
        dst_t = CImageType(w,h,CImageType.RGB24)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "BGR_8U3CToRGB24"

class BGR_8U3CToGray8(GenericNode):
    def __init__(self,name,w,h):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,CImageType.BGR_8U3C)
        dst_t = CImageType(w,h,CImageType.GRAY8)

        self.addInput("i",src_t,src_t.nb_bytes)
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
        return "BGR_8U3CToGray8"