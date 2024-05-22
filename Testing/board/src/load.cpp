#include "load.hpp"

extern "C" {
#include "common.h"
#include "cinput.h"
}

#include <cstring>
#include <iostream>
#include <fstream>


const unsigned char* load_test_mem(const char* filepath)
{
    const unsigned char *buf;

#if defined(CINPUT) 
    //buf = (const unsigned char*)INPUT_ADDR;
    (void)filepath;
    buf = (const unsigned char*)cinput;
#else
    std::ifstream fin(filepath, std::ios::binary);
    if (!fin) 
    {
        return(nullptr);
    }

    std::streampos begin,end;
    begin = fin.tellg();
    fin.seekg (0, std::ios::end);
    end = fin.tellg();
    fin.seekg (0, std::ios::beg);

    printf("%s : %lld bytes (%llX)\r\n",filepath,end-begin,end-begin);
    

    buf =  (const unsigned char*)aligned_malloc(end-begin,BUFFER_ALIGNMENT);

    if (buf == nullptr)
    {
        printf("Not enough memory\r\n");
        return(nullptr);
    }

    fin.read(reinterpret_cast<char*>(const_cast<unsigned char*>(buf)), end-begin);

#endif

    return(buf);
}

void save_mem(const char* filepath,const unsigned char*buf,std::size_t size)
{
#if !defined(NOOUTPUT)
  std::ofstream outfile (filepath,std::ofstream::binary);
  outfile.write (reinterpret_cast<const char*>(buf),size);
  outfile.close();
#else
  (void)filepath;
  (void)buf;
  (void)size;
#endif
  //FILE *f;
  //f=fopen(filepath,"wb");
  //fwrite(buf,size,1,f);
  //fclose(f);
}

static uint32_t align(uint32_t pos)
{
    uint32_t r = pos % BUFFER_ALIGNMENT;
    if (r != 0)
    {
        r  = BUFFER_ALIGNMENT - r;
        pos += r;
    }
    
    return(pos);

}

unsigned char* create_write_buffer(const std::vector<BufferDescription> &bufs,
                                   uint32_t &total_bytes)
{
    total_bytes = 0;
    for(auto b:bufs)
    {
        total_bytes += b.bytes() + BUFFER_ALIGNMENT;
    }
    if (total_bytes == BUFFER_ALIGNMENT)
    {
        return nullptr;
    }

    total_bytes += sizeof(uint32_t);
    // b bytes
    // dims
    // dt 
    // offsets
    total_bytes += bufs.size() * (1+5+1+1)*sizeof(uint32_t);

    uint32_t* buf;

    buf =  (uint32_t*)aligned_malloc(total_bytes,BUFFER_ALIGNMENT);

    if (buf == nullptr)
    {
        printf("Not enough memory\r\n");
        return(nullptr);
    }

    uint32_t pos=0;
    buf[0] = bufs.size();
    pos += sizeof(uint32_t);

    // Tensor sizes
    for(auto b:bufs)
    {
        buf[pos>>2] = b.bytes();
        pos += sizeof(uint32_t);
    }

    // Tensor dims
    for(auto b:bufs)
    {
        buf[pos>>2]       = b.shape().nb;
        buf[(pos>>2) + 1] = b.shape().dims[0];
        buf[(pos>>2) + 2] = b.shape().dims[1];
        buf[(pos>>2) + 3] = b.shape().dims[2];
        buf[(pos>>2) + 4] = b.shape().dims[3];
        pos += 5*sizeof(uint32_t);
    }

    // Datatypes
    for(auto b:bufs)
    {
        buf[pos>>2] = b.dt();
        pos += sizeof(uint32_t);
    }

    uint32_t offset_pos = pos;
    pos += sizeof(uint32_t)*bufs.size();
    std::vector<uint32_t> offset(bufs.size(),0);

    pos = align(pos);
    int k=0;
    for(auto b:bufs)
    {
       offset[k] = pos;
       pos += b.bytes();
       pos = align(pos);
       k++;
    }

    for(unsigned int i=0;i < bufs.size();i++)
    {
        buf[(offset_pos>>2)+i] = offset[i];
    }


    return((unsigned char*)buf);


}