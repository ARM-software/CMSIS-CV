import numpy as np
import struct 
from PIL import Image
import PIL
import numpy as np
import os

def _serialize_u32(file, tensor):
    """ writes one uint32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint32).flatten()
    b = struct.pack(f'{len(tensor)}L', *tensor)
    file.write(b)
    return(len(b))

def _serialize_u16(file, tensor):
    """ writes one uint32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint16).flatten()
    b = struct.pack(f'{len(tensor)}H', *tensor)
    file.write(b)
    return(len(b))

def _serialize_u8(file, tensor):
    """ writes one f32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint8).flatten()
    b = struct.pack(f'{len(tensor)}B', *tensor)
    file.write(b)
    return(len(b))

def _serialize_s32(file, tensor):
    """ writes one uint32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint32).flatten()
    b = struct.pack(f'{len(tensor)}l', *tensor)
    file.write(b)
    return(len(b))

def _serialize_s16(file, tensor):
    """ writes one uint32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint16).flatten()
    b = struct.pack(f'{len(tensor)}h', *tensor)
    file.write(b)
    return(len(b))

def _serialize_s8(file, tensor):
    """ writes one f32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.uint8).flatten()
    b = struct.pack(f'{len(tensor)}b', *tensor)
    file.write(b)
    return(len(b))

def _serialize_f64(file, tensor):
    """ writes one f32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.double).flatten()
    b = struct.pack(f'{len(tensor)}d', *tensor)
    file.write(b)
    return(len(b))

def _serialize_f32(file, tensor):
    """ writes one f32 tensor to file that is open in wb mode """
    tensor = tensor.astype(float).flatten()
    b = struct.pack(f'{len(tensor)}f', *tensor)
    file.write(b)
    return(len(b))

def _serialize_f16(file, tensor):
    """ writes one f32 tensor to file that is open in wb mode """
    tensor = tensor.astype(np.float16).flatten()
    b = struct.pack(f'{len(tensor)}e', *tensor)
    file.write(b)
    return(len(b))


def _read_uint32_array(file,nb):
    b = file.read(4*nb)
    a = struct.unpack(f'{nb}L', b)
    return(list(a))

def _read_uint16_array(file,nb):
    b = file.read(2*nb)
    a = struct.unpack(f'{nb}H', b)
    return(list(a))

def _read_uint8_array(file,nb):
    b = file.read(nb)
    a = struct.unpack(f'{nb}B', b)
    return(list(a))

def _read_sint32_array(file,nb):
    b = file.read(4*nb)
    a = struct.unpack(f'{nb}l', b)
    return(list(a))

def _read_sint16_array(file,nb):
    b = file.read(2*nb)
    a = struct.unpack(f'{nb}h', b)
    return(list(a))

def _read_sint8_array(file,nb):
    b = file.read(nb)
    a = struct.unpack(f'{nb}b', b)
    return(list(a))

def _read_f64_array(file,nb):
    b = file.read(8*nb)
    a = struct.unpack(f'{nb}d', b)
    return(list(a))

def _read_f32_array(file,nb):
    b = file.read(4*nb)
    a = struct.unpack(f'{nb}f', b)
    return(list(a))

def _read_f16_array(file,nb):
    b = file.read(2*nb)
    a = struct.unpack(f'{nb}e', b)
    return(list(a))

# This iw the general image type used for the test
# It can contain a Pillow RGB or GRAY8 image
# or a numpy tensor (shape must have at most 4 dimensions)
class AlgoImage:
    IMG_RGB_TYPE = 0
    IMG_GRAY8_TYPE = 1
    IMG_NUMPY_TYPE_UINT8  = 2
    IMG_NUMPY_TYPE_UINT16 = 3
    IMG_NUMPY_TYPE_UINT32 = 4
    IMG_NUMPY_TYPE_SINT8  = 5
    IMG_NUMPY_TYPE_SINT16 = 6
    IMG_NUMPY_TYPE_SINT32 = 7
    IMG_NUMPY_TYPE_F16  = 8
    IMG_NUMPY_TYPE_F32 = 9
    IMG_NUMPY_TYPE_F64 = 10

    @classmethod
    def open(cls, path):
        file_name, file_extension = os.path.splitext(path)
        if file_extension == ".npy":
            return AlgoImage(np.load(path))
        else:
            return AlgoImage(PIL.Image.open(path))

    def __init__(self,img):
        self._img = img

    def save(self,path):
        file_name, file_extension = os.path.splitext(path)
        if self.is_image:
            self.img.save(file_name + ".tiff")
        else:
            np.save(file_name + ".npy",self.tensor)



    @property 
    def is_image(self):
        return not isinstance(self._img,np.ndarray)

    @property 
    def dtype(self):
        if self.is_image:
           if self._img.mode == "RGB":
              return AlgoImage.IMG_RGB_TYPE
           elif self._img.mode == "L":
              return AlgoImage.IMG_GRAY8_TYPE
           else:
              raise NameError(f"Unsupported image mode {self._img.mode}")
        elif isinstance(self._img,np.ndarray):
            if self._img.dtype == np.uint8:
                return AlgoImage.IMG_NUMPY_TYPE_UINT8
            if self._img.dtype == np.uint16:
                return AlgoImage.IMG_NUMPY_TYPE_UINT16
            if self._img.dtype == np.uint32:
                return AlgoImage.IMG_NUMPY_TYPE_UINT32
            if self._img.dtype == np.int8:
                return AlgoImage.IMG_NUMPY_TYPE_SINT8
            if self._img.dtype == np.int16:
                return AlgoImage.IMG_NUMPY_TYPE_SINT16
            if self._img.dtype == np.int32:
                return AlgoImage.IMG_NUMPY_TYPE_SINT32
            if self._img.dtype == np.float16:
                return AlgoImage.IMG_NUMPY_TYPE_F16
            if self._img.dtype == float:
                return AlgoImage.IMG_NUMPY_TYPE_F32
            if self._img.dtype == np.double:
                return AlgoImage.IMG_NUMPY_TYPE_F64
    
        raise NameError(f"Unsupported datatype {self._img}")


    @property 
    def dim(self):
        t = list(self.tensor.shape)
        r = len(t)
        return ([r] + t + [0]*(4-r))

    @property
    def get_size(self):
        return self.tensor.nbytes

    @property 
    def tensor(self):
        if self.is_image:
           buf = np.array(self._img)
           return(buf)
        else:
            return(self._img)

    @property
    def img(self):
        return self._img
    

    def serialize(self,file):
        t = self.tensor 
        if t.dtype == np.uint8:
           return _serialize_u8(file,t)
        if t.dtype == np.uint16:
           return _serialize_u16(file,t)
        if t.dtype == np.uint32:
           return _serialize_u32(file,t)

        if t.dtype == np.int8:
           return _serialize_s8(file,t)
        if t.dtype == np.int16:
           return _serialize_s16(file,t)
        if t.dtype == np.int32:
           return _serialize_s32(file,t)

        if t.dtype == np.float16:
           return _serialize_f16(file,t)
        if t.dtype == float:
           return _serialize_f32(file,t)
        if t.dtype == np.double:
           return _serialize_f64(file,t)

        raise NameError(f"Unsupported NumPy datatype for serialization {t.dtype}")

   
def unserialize(file,dt,nbbytes,dims):
    a = bytes(_read_uint8_array(file,nbbytes))
    if dt == AlgoImage.IMG_NUMPY_TYPE_UINT8:
       res = np.frombuffer(a,dtype=np.uint8).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_UINT16:
       res = np.frombuffer(a,dtype=np.uint16).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_UINT32:
       res = np.frombuffer(a,dtype=np.uint32).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_SINT8:
       res = np.frombuffer(a,dtype=np.int8).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_SINT16:
       res = np.frombuffer(a,dtype=np.int16).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_SINT32:
       res = np.frombuffer(a,dtype=np.int32).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_F16:
       res = np.frombuffer(a,dtype=np.float16).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_F32:
       res = np.frombuffer(a,dtype=float).reshape(dims)
    elif dt == AlgoImage.IMG_NUMPY_TYPE_F64:
       res = np.frombuffer(a,dtype=np.double).reshape(dims)
    elif dt == AlgoImage.IMG_RGB_TYPE:
        res = Image.frombytes('RGB',(dims[1],dims[0]),a)
    elif dt == AlgoImage.IMG_GRAY8_TYPE:
        res = Image.frombytes('L',(dims[1],dims[0]),a)
    else:
        raise NameError(f"Unsupported NumPy datatype for unserialization {dt}")

    return(AlgoImage(res))


def _align(file,pos,alignment):
    r = pos % alignment
    if r != 0:
       r = alignment - r 
       z = np.zeros(r,dtype=np.uint8)
       #z = np.ones(r,dtype=np.uint8)*255
       res = struct.pack(f'{len(z)}B', *z)
       file.write(res)
    return(r)


def serialize_tensors(file,tensors,alignment=8):
    """
    Serialize the tensors to a binary file with alignment.

    The format is as simple as possible and with enouhg information
    to be able to read the samples back with Python.

    The shapes and strides are not saved. The C code using this data
    has the knowledge of the shapes and strides.

    The format is:
    Nb of tensors : uint32 
    Array of uint32 lengths for each tensor. Length expressed in bytes.
    Array of uint32 shape for each tensor. Shape is 5 word32
      Number of dimensions. The dimensions. Padded with 0 if less than 4 dimensions
    Array of uint32 datatype (to be able to read back with Python)
    Array of uint32 offset in the file where to find the tensor
    Aligned tensor follows
    """
    pos = 0

    # Write number of tensors
    pos += _serialize_u32(file,np.array([len(tensors)]))

    # Write lengths of the tensors
    lengths_in_bytes=np.array([x.get_size for x in tensors],dtype=np.uint32)
    pos += _serialize_u32(file,lengths_in_bytes)
    #print(lengths_in_bytes)

    dims = []
    for x in tensors:
        dims += x.dim
    dims=np.array(dims,dtype=np.uint32)
    #print(dims)
    
    # Write dimensions of the images
    # 0,0 when it is not an image
    pos += _serialize_u32(file,dims)
    
    # Write datatypes of tensors
    dt=np.array([x.dtype for x in tensors],dtype=np.uint32)
    pos += _serialize_u32(file,dt)
    #print(dt)

    # Offsets of tensors in files
    offset_pos = pos
    offsets=np.zeros(len(tensors),dtype=np.uint32)
    # Write offsets of the tensors
    pos += _serialize_u32(file,offsets)
    pos += _align(file,pos,alignment)

    # Write tensors
    k = 0 
    for t in tensors:
        offsets[k] = pos 
        pos += t.serialize(file)
        
        k = k + 1
        if k != len(tensors):
           pos += _align(file,pos,alignment)

    file.seek(offset_pos)
    _serialize_u32(file,offsets)
    #print(offsets)

def _read_uint32(file):
    b = file.read(4)
    nb_tensors = struct.unpack(f'L', b)
    return(nb_tensors[0])


def format_shape(l):
    nb = l[0]
    return tuple(l[1:1+nb])

def read_tensors(file):
    res = []
    nb=_read_uint32(file)
    tensor_sizes = _read_uint32_array(file,nb)
    shapes = np.array(_read_uint32_array(file,5*nb))
    shapes = shapes.reshape(nb,5)
    tensor_dims = [format_shape(s) for s in shapes]
    tensor_dt = _read_uint32_array(file,nb)
    
    #print(nb)
    #print(tensor_sizes)
    #print(tensor_dims)
    #print(tensor_dt)

    tensor_offsets = _read_uint32_array(file,nb)

    #print(tensor_offsets)


    for s,dims,dt,o in zip(tensor_sizes,tensor_dims,tensor_dt,tensor_offsets):
        # Position at tensor offset
        file.seek(o)

        a = unserialize(file,dt,s,dims)
        
        res.append(a)

    return(res)
    

if __name__ == "__main__":
    from pathlib import Path

    Path("references").mkdir(parents=True, exist_ok=True)

    imga = Image.open("Patterns/Mandrill_cropped.tiff")
    #NB = 50
    #imga = imga.crop((NB,0,512-NB,512))
    #imga.save("Patterns/Mandrill_cropped.tiff")
    imgb = Image.open("Patterns/JellyBeans.tiff")
    imgb = imgb.convert("L")

    imga.save("results/img/input_0.tiff")
    imgb.save("results/img/input_1.tiff")

    tests = AlgoImage(np.array(range(10),dtype=np.uint32).reshape(2,5))

    with open("test.dat","wb") as f:
         serialize_tensors(f,[tests,AlgoImage(imga),AlgoImage(imgb)])


    with open("test.dat","rb") as f:
         r = read_tensors(f)

    for i in r:
        #print(i.dim)
        if i.is_image:
            i.img.show()