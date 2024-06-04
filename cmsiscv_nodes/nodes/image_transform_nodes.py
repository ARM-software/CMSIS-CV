from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

from .node_types import *

class _ImgCropping(GenericNode):
    def __init__(self,name,img_type,w,h,left,top,right,bottom):
        GenericSink.__init__(self,name,identified=False)
        src_t = CImageType(w,h,img_type)
        dst_t = CImageType(right-left,bottom-top,img_type)

        self.addInput("i",src_t,src_t.nb_bytes)
        self.addOutput("o",dst_t,dst_t.nb_bytes)

        if isinstance(left,int):
            self.addLiteralArg(left)
        else:
            self.addVariableArg(left)

        if isinstance(top,int):
            self.addLiteralArg(top)
        else:
            self.addVariableArg(top)

        if isinstance(right,int):
            self.addLiteralArg(right)
        else:
            self.addVariableArg(right)
 
        if isinstance(bottom,int):
            self.addLiteralArg(bottom)
        else:
            self.addVariableArg(bottom)

     def __call__(self, i):
        g,n = i 
        k = next(iter(n._outputs))
        o = n._outputs[k]
        g.connect(o,self.i)
        return(g,self)
        

class Gray8Cropping(_ImgCropping):
    def __init__(self,name,w,h):
        _ImgCropping.__init__(self,name,CImageType.GRAY8,
            w,h,left,top,right,bottom,
            identified=False)
   
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Gray8Cropping"

class RGB24Cropping(_ImgCropping):
    def __init__(self,name,w,h):
        _ImgCropping.__init__(self,name,CImageType.RGB24,
            w,h,left,top,right,bottom,
            identified=False)
   
    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "RGB24Cropping"