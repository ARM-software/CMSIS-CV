from cmsis_stream.cg.scheduler import StreamFIFO,CGStaticType,SINT8,UINT32,CType,CStructType,GenericNode,GenericSink,GenericSource

from html import escape

class ImageFIFO(StreamFIFO):
    @property
    def cname(self):
        return "ImageFIFO"

    @property
    def args(self):
        # The type is an image type
        return [f"{self._ctype.width}",
                f"{self._ctype.height}"]

#tfType = CStructType("TfLiteTensor",10)
#posType = CStructType("struct_position",8)

#bufferType = CType(SINT8)
activateType = CType(UINT32)

class CImageType(CGStaticType):
    YUV420 = 1 # Unpacked
    RGB24 = 2
    GRAY16 = 3
    GRAY8 = 4
    RGBA32 = 5
    Q15 = 6
    GRADIENT_Q15 = 7
    BGR_8U3C = 8 # Unpacked

    def __init__(self,w,h,t=YUV420):
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
        # because in input / output creations in the node we give
        # the number of bytes.
        # So the sample size is 1 byte
        # Having int8 FIFOs is more flexible since we can 
        # do some casting without having to introduce casting nodes in the
        # graph. But then it means compatibility check between IOs
        # when no casting possible must be done at the Python level.
        return 1
        
          
    @property
    def nb_bytes(self):
        if self._pixel_type == CImageType.YUV420:
           return(int(1.5*self._w*self._h))
        if self._pixel_type == CImageType.RGB24:
           return(int(3*self._w*self._h))
        if self._pixel_type == CImageType.GRAY16:
           return(int(2*self._w*self._h))
        if self._pixel_type == CImageType.Q15:
           return(int(2*self._w*self._h))
        if self._pixel_type == CImageType.GRADIENT_Q15:
           return(int(4*self._w*self._h))
        if self._pixel_type == CImageType.GRAY8:
           return(int(self._w*self._h))
        if self._pixel_type == CImageType.RGBA32:
           return(int(4*self._w*self._h))
        if self._pixel_type == CImageType.BGR_8U3C:
           return(int(3*self._w*self._h))

    @property
    def ctype(self):
        return(CType(SINT8).ctype)

    @property
    def graphViztype(self):
        if self.format == CImageType.YUV420:
           return(escape(f"YUV420_{self.width}_{self.height}"))
        if self.format == CImageType.RGB24:
           return(escape(f"RGB24_{self.width}_{self.height}"))
        if self.format == CImageType.GRAY16:
           return(escape(f"GRAY16_{self.width}_{self.height}"))
        if self.format == CImageType.Q15:
           return(escape(f"Q15_{self.width}_{self.height}"))
        if self.format == CImageType.GRADIENT_Q15:
           return(escape(f"GRAD_Q15_{self.width}_{self.height}"))
        if self.format == CImageType.GRAY8:
           return(escape(f"GRAY8_{self.width}_{self.height}"))
        if self.format == CImageType.RGBA32:
           return(escape(f"RGBA32_{self.width}_{self.height}"))
        if self.format == CImageType.BGR_8U3C:
           return(escape(f"BGR_8U3C{self.width}_{self.height}"))
