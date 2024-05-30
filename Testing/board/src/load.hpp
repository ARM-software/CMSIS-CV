#pragma once

#include "common.h"    


#include <vector>

template<typename T>
struct Buffer{
    static const T* read(const unsigned char *test,
                         const int buffer_nb)
    {
        return(reinterpret_cast<const T*>(get_read_buffer(test,buffer_nb)));
    }

    static T* write(unsigned char *test,
                       const int buffer_nb)
    {
        return(reinterpret_cast<T*>(get_write_buffer(test,buffer_nb)));
    }
};

extern const unsigned char* load_test_mem(const char* filepath);

extern void save_mem(const char* filepath,const unsigned char*,std::size_t);

enum buffer_type {
   kIMG_RGB_TYPE = 0,
   kIMG_GRAY8_TYPE = 1,
   kIMG_NUMPY_TYPE_UINT8  = 2,
   kIMG_NUMPY_TYPE_UINT16 = 3,
   kIMG_NUMPY_TYPE_UINT32 = 4,
   kIMG_NUMPY_TYPE_SINT8  = 5,
   kIMG_NUMPY_TYPE_SINT16 = 6,
   kIMG_NUMPY_TYPE_SINT32 = 7,
   kIMG_NUMPY_TYPE_F16  = 8,
   kIMG_NUMPY_TYPE_F32 = 9,
   kIMG_NUMPY_TYPE_F64 = 10,
   kIMG_YUV420_TYPE = 11,

};

struct Shape
{
  

   constexpr explicit Shape(uint32_t h,uint32_t w,uint32_t c,uint32_t d):
   nb(4),dims{h,w,c,d}{};

   constexpr explicit Shape(uint32_t h,uint32_t w,uint32_t c):
   nb(3),dims{h,w,c,0}{};

   constexpr explicit Shape(uint32_t h,uint32_t w):
   nb(2),dims{h,w,0,0}{};
   
   constexpr explicit Shape(uint32_t l):
   nb(1),dims{l,0,0,0}{};

   constexpr uint32_t nb_elems() const
   {
      uint32_t r = 1;
      for(uint32_t i=0;i<nb;i++)
      {
         r *= dims[i];
      }
      return(r);
   };

   uint32_t nb;
   uint32_t dims[4];
};


struct BufferDescription 
{
   explicit constexpr BufferDescription(const Shape &s,
                                        enum buffer_type type=kIMG_RGB_TYPE):
   mShape(s),buf_type(type)
   {
       if (type == kIMG_RGB_TYPE)
       {
          // Force channel to 3
          mShape.nb = 3;
          mShape.dims[2] = 3;
       }
       if (type == kIMG_YUV420_TYPE)
       {
          // Force channel to 2
          mShape.nb = 2;
       }
   };

  
   constexpr uint32_t bytes() const
   {
     switch(buf_type)
     {
     case kIMG_RGB_TYPE:
        // Shape is h,w,c
        return(mShape.nb_elems());
     case kIMG_GRAY8_TYPE:
        return(mShape.nb_elems());
     case kIMG_NUMPY_TYPE_UINT8:
        return (mShape.nb_elems());
     case kIMG_NUMPY_TYPE_UINT16:
      return (2*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_UINT32:
      return (4*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_SINT8:
      return (mShape.nb_elems());
     case kIMG_NUMPY_TYPE_SINT16:
      return (2*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_SINT32:
      return (4*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_F16:
      return (2*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_F32:
      return (4*mShape.nb_elems());
     case kIMG_NUMPY_TYPE_F64:
        return (8*mShape.nb_elems());
     case kIMG_YUV420_TYPE:
        // Shape is h,w
        return(mShape.nb_elems() + (mShape.nb_elems()>>1));
     default:
       return(0);
     }
   };

   constexpr uint32_t dt() const
   {
     return((uint32_t)buf_type);
   };

   constexpr Shape shape() const {return mShape;};

protected:
   Shape mShape;
   enum buffer_type buf_type; 
};

extern unsigned char* create_write_buffer(const std::vector<BufferDescription> &buf,uint32_t &total_bytes);