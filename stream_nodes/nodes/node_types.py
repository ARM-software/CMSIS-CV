from cmsis_stream.cg.scheduler import CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape


#tfType = CStructType("TfLiteTensor",10)
#posType = CStructType("struct_position",8)

#bufferType = CType(SINT8)
activateType = CType(UINT32)

class CImageType(CGStaticType):
    YUV = 1
    RGB = 2
    GRAY16 = 3
    GRAY8 = 4
    RGBA = 5

    def __init__(self,w,h,t=YUV):
        CGStaticType.__init__(self)
        self._w = w 
        self._h = h
        self._pixel_type = t

    def __eq__(self, other):
      return(CGStaticType.__eq__(self,other) and 
             self._w == other._w and
             self._h == other._h and
             self._pixel_type == other._pixel_type)

    @property
    def width(self):
        return self._w

    @property
    def height(self):
        return self._h

    @property
    def format(self):
        return self._pixel_type
    
    
    
    @property
    def bytes(self):
        # The C code is using array of int8
        # This type is just used at Python / Graphviz level
        return 1
        #if self._pixel_type == CImageType.YUV:
        #   return(int(1.5*self._w*self._h))
        #if self._pixel_type == CImageType.RGB:
        #   return(int(3*self._w*self._h))
        # 
          
    @property
    def _nb_bytes(self):
        if self._pixel_type == CImageType.YUV:
           return(int(1.5*self._w*self._h))
        if self._pixel_type == CImageType.RGB:
           return(int(3*self._w*self._h))
        if self._pixel_type == CImageType.GRAY16:
           return(int(2*self._w*self._h))
        if self._pixel_type == CImageType.GRAY8:
           return(int(self._w*self._h))
        if self._pixel_type == CImageType.RGBA:
           return(int(4*self._w*self._h))

    @property
    def ctype(self):
        return(CType(SINT8).ctype)

    @property
    def graphViztype(self):
        if self.format == CImageType.YUV:
           return(escape(f"YUV_{self.width}_{self.height}"))
        if self.format == CImageType.RGB:
           return(escape(f"RGB_{self.width}_{self.height}"))
        if self.format == CImageType.GRAY16:
           return(escape(f"GRAY16_{self.width}_{self.height}"))
        if self.format == CImageType.GRAY8:
           return(escape(f"GRAY8_{self.width}_{self.height}"))
        if self.format == CImageType.RGBA:
           return(escape(f"RGBA_{self.width}_{self.height}"))
