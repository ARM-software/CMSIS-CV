/* ----------------------------------------------------------------------
 * Project:      CMSIS DSP Library
 * Title:        GenericNodes.h
 * Description:  C++ support templates for the compute graph with static scheduler
 *
 *
 * Target Processor: Cortex-M and Cortex-A cores
 * -------------------------------------------------------------------- 
 *
 * Copyright (C) 2021-2023 ARM Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SCHEDGEN_H_
#define _SCHEDGEN_H_

#include <vector>
#include <cstring>
#include <stdarg.h>
/* 
Defined in cg_status.h by default but user
may want to use a different header to define the 
error codes of the application
*/
#ifndef CG_SUCCESS_ID_CODE
#define CG_SUCCESS_ID_CODE (CG_SUCCESS)
#endif 

#ifndef CG_SKIP_EXECUTION_ID_CODE
#define CG_SKIP_EXECUTION_ID_CODE (CG_SKIP_EXECUTION)
#endif

#ifndef CG_BUFFER_ERROR_ID_CODE
#define CG_BUFFER_ERROR_ID_CODE (CG_BUFFER_ERROR)
#endif

/*
 
 For callback mode 

*/

enum kCBStatus {
   kNewExecution = 1,
   kResumedExecution = 2
};

/* Node ID is -1 when nodes are not identified for the external
world */
#define CG_UNIDENTIFIED_NODE (-1)

namespace arm_cmsis_stream {
// FIFOS 

#ifdef DEBUGSCHED

#include <iostream>

template<typename T>
struct debugtype{
    typedef T type;
};

template<>
struct debugtype<char>{
    typedef int type;
};

template<typename T>
using Debug = struct debugtype<T>;

#endif

/***************
 * 
 * FIFOs
 * 
 **************/

template<typename T>
class FIFOBase{
public:
    virtual T* getWriteBuffer(int nb)=0;
    virtual T* getReadBuffer(int nb)=0;
    virtual ~FIFOBase() {};

    /*
    Used when FIFO buffer is enforced by a node.
    Can only be used before the FIFO has been interacted with
    */
    virtual void setBuffer(T *buffer)=0;
    /*

    Below functions are only useful in asynchronous mode.
    Synchronous implementation can provide an empty
    implementation.

    */
    virtual bool willUnderflowWith(int nb) const = 0;
    virtual bool willOverflowWith(int nb) const = 0;
    virtual int nbSamplesInFIFO() const = 0;
    virtual int nbOfFreeSamplesInFIFO() const = 0;

    /*

    New API for the vision use case. Those APIs are new
    compared to the standard FIFOBase class.

    */
    virtual int width() const = 0;
    virtual int height() const = 0;

};

template<typename T, int length, int isArray=0, int isAsync = 0>
class ImageFIFO;

/* Real ImageFIFO, Synchronous */
template<typename T, int length>
class ImageFIFO<T,length,0,0>: public FIFOBase<T> 
{
    public:
        explicit ImageFIFO(T *buffer,int delay,const uint32_t w,const uint32_t h):
        mBuffer(buffer),
        readPos(0),
        writePos(delay),
        mWidth(w),
        mHeight(h) {};
        
        /* Constructor used for memory sharing optimization.
           The buffer is a shared memory wrapper */
        explicit ImageFIFO(void *buffer,int delay,const uint32_t w,const uint32_t h):
        mBuffer((T*)buffer),
        readPos(0),
        writePos(delay),
        mWidth(w),
        mHeight(h) {};

        void setBuffer(T *buffer){mBuffer = buffer;};

        int width()  const final {return mWidth;};
        int height() const final {return mHeight;};

        /* 
        ImageFIFO are fixed and not made to be copied or moved.
        */
        ImageFIFO(const ImageFIFO&) = delete;
        ImageFIFO(ImageFIFO&&) = delete;
        ImageFIFO& operator=(const ImageFIFO&) = delete;
        ImageFIFO& operator=(ImageFIFO&&) = delete;

        bool willUnderflowWith(int nb) const final 
        {
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(int nb) const final 
        {
            return((writePos - readPos + nb)>length);
        }
        int nbSamplesInFIFO() const final   {return (writePos - readPos);};
        int nbOfFreeSamplesInFIFO() const final  {return (length - writePos + readPos);};

        T * getWriteBuffer(int nb) final 
        {
            
            T *ret;
            if (readPos > 0)
            {
                /* This is re-aligning the read buffer.
                   Aligning buffer is better for vectorized code.
                   But it has an impact since more memcpy are
                   executed than required.
                   This is likely to be not so useful in practice
                   so a future version will optimize the memcpy usage
                   */
                memcpy((void*)mBuffer,(void*)(mBuffer+readPos),(writePos-readPos)*sizeof(T));
                writePos -= readPos;
                readPos = 0;
            }
            
            ret = mBuffer + writePos;
            writePos += nb; 
            return(ret);
        };

        T* getReadBuffer(int nb) final 
        {
            
            T *ret = mBuffer + readPos;
            readPos += nb;
            return(ret);
        }

        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * mBuffer;
        int readPos,writePos;
        const uint32_t mWidth,mHeight;
};

/* Buffer, Synchronous */
template<typename T, int length>
class ImageFIFO<T,length,1,0>: public FIFOBase<T> 
{
    public:
        /* 
         Delay not used and always called with delay 0
        */
        explicit ImageFIFO(T *buffer,int, const uint32_t w,const uint32_t h):
        mBuffer(buffer),
        mWidth(w),
        mHeight(h) {};
        explicit ImageFIFO(void *buffer,int, const uint32_t w,const uint32_t h):
        mBuffer((T*)buffer),
        mWidth(w),
        mHeight(h) {};

        void setBuffer(T *buffer){mBuffer = buffer;};

        int width()  const final {return mWidth;};
        int height() const final {return mHeight;};

        /* 
        ImageFIFO are fixed and not made to be copied or moved.
        */
        ImageFIFO(const ImageFIFO&) = delete;
        ImageFIFO(ImageFIFO&&) = delete;
        ImageFIFO& operator=(const ImageFIFO&) = delete;
        ImageFIFO& operator=(ImageFIFO&&) = delete;

        /* 
           Not used in synchronous mode 
           and this version of the ImageFIFO is
           never used in asynchronous mode 
           so empty functions are provided.
        */
        bool willUnderflowWith(int nb) const final   {(void)nb;return false;};
        bool willOverflowWith(int nb) const final   {(void)nb;return false;};
        int nbSamplesInFIFO() const final   {return(0);};
        int nbOfFreeSamplesInFIFO() const final  {return( 0);};


        T* getWriteBuffer(int nb) final  
        {
            (void)nb;
            return(mBuffer);
        };

        T* getReadBuffer(int nb) final 
        {
            (void)nb;
            return(mBuffer);
        }

        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * mBuffer;
        const uint32_t mWidth,mHeight;
};

/* Real ImageFIFO, Asynchronous */
template<typename T, int length>
class ImageFIFO<T,length,0,1>: public FIFOBase<T> 
{
    public:
        explicit ImageFIFO(T *buffer,int delay,const uint32_t w,const uint32_t h):
        mBuffer(buffer),
        readPos(0),
        writePos(delay),
        mWidth(w),
        mHeight(h) {};
        explicit ImageFIFO(void *buffer,int delay,const uint32_t w,const uint32_t h):
        mBuffer((T*)buffer),
        readPos(0),
        writePos(delay),
        mWidth(w),
        mHeight(h) {};

        void setBuffer(T *buffer){mBuffer = buffer;};

        int width()  const final {return mWidth;};
        int height() const final {return mHeight;};

        /* 
        ImageFIFO are fixed and not made to be copied or moved.
        */
        ImageFIFO(const ImageFIFO&) = delete;
        ImageFIFO(ImageFIFO&&) = delete;
        ImageFIFO& operator=(const ImageFIFO&) = delete;
        ImageFIFO& operator=(ImageFIFO&&) = delete;

        /* 

        Check for overflow must have been done
        before using this function 
        
        */
        T * getWriteBuffer(int nb) final 
        {
            
            T *ret;
            if (readPos > 0)
            {
                memcpy((void*)mBuffer,(void*)(mBuffer+readPos),(writePos-readPos)*sizeof(T));
                writePos -= readPos;
                readPos = 0;
            }
            
            ret = mBuffer + writePos;
            writePos += nb; 
            return(ret);
        };

        /* 
        
        Check for undeflow must have been done
        before using this function 
        
        */
        T* getReadBuffer(int nb) final 
        {
           
            T *ret = mBuffer + readPos;
            readPos += nb;
            return(ret);
        }

        bool willUnderflowWith(int nb) const final  
        {
            return((writePos - readPos - nb)<0);
        }

        bool willOverflowWith(int nb) const final  
        {
            return((writePos - readPos + nb)>length);
        }

        int nbSamplesInFIFO() const final   {return (writePos - readPos);};
        int nbOfFreeSamplesInFIFO() const final   {return (length - writePos + readPos);};


        #ifdef DEBUGSCHED
        void dump()
        {
            int nb=0;
            std::cout << std::endl;
            std::cout << "FIFO nb samples = " << (writePos - readPos) << std::endl;
            for(int i=0; i < length ; i++)
            {
                std::cout << (typename Debug<T>::type)mBuffer[i] << " ";
                nb++;
                if (nb == 10)
                {
                    nb=0;
                    std::cout << std::endl;
                }
            }
            std::cout << std::endl;
            std::cout << std::endl;
        }
        #endif

    protected:
        T * mBuffer;
        int readPos,writePos;
        const uint32_t mWidth,mHeight;
};

/***************
 * 
 * GENERIC NODES
 * 
 **************/

class NodeBase
{
public:
    virtual int run()=0;
    virtual int prepareForRunning()=0;
    virtual ~NodeBase() {};

    NodeBase(){};

    /* 
    Nodes are fixed and not made to be copied or moved.
    */
    NodeBase(const NodeBase&) = delete;
    NodeBase(NodeBase&&) = delete;
    NodeBase& operator=(const NodeBase&) = delete;
    NodeBase& operator=(NodeBase&&) = delete;

    void setID(int id)   {mNodeID = id;};
    int nodeID() const   {return(mNodeID);};

    void setExecutionStatus(kCBStatus id)   {mExecutionStatus = id;};
    kCBStatus executionStatus() const   {return(mExecutionStatus);};

private:
    int mNodeID = CG_UNIDENTIFIED_NODE;
    kCBStatus mExecutionStatus = kNewExecution;
};

template<typename IN, int inputSize,typename OUT, int outputSize>
class GenericNode:public NodeBase
{
public:
     explicit GenericNode(FIFOBase<IN> &src,FIFOBase<OUT> &dst):mSrc(src),mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb = outputSize) {return mDst.getWriteBuffer(nb);};
     IN * getReadBuffer(int nb = inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};
     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

     int inputWidth()  const  {return mSrc.width();};
     int inputHeight() const  {return mSrc.height();};

     int outputWidth()  const  {return mDst.width();};
     int outputHeight() const  {return mDst.height();};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT> &mDst;
};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericToManyNode:public NodeBase
{
public:
     explicit GenericToManyNode(FIFOBase<IN> &src,
                       std::initializer_list<FIFOBase<OUT>*> dst):mSrc(src),mDstList(dst){};


protected:
     size_t getNbOutputs() const {return(mDstList.size());};

     IN * getReadBuffer(int nb = inputSize) {return mSrc.getReadBuffer(nb);};
     OUT * getWriteBuffer(int id=0,int nb = outputSize) {return mDstList[id]->getWriteBuffer(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};
     bool willOverflow(int id=0,int nb = outputSize) const {return mDstList[id]->willOverflowWith(nb);};

     int inputWidth()  const  {return mSrc.width();};
     int inputHeight() const  {return mSrc.height();};

     int outputWidth(int id=0)  const  {return mDstList[id]->width();};
     int outputHeight(int id=0) const  {return mDstList[id]->height();};

private:
    FIFOBase<IN> &mSrc;
    const std::vector<FIFOBase<OUT>*> mDstList;
};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericFromManyNode:public NodeBase
{
public:
     explicit GenericFromManyNode(std::initializer_list<FIFOBase<IN>*> src,
                         FIFOBase<OUT> &dst):mSrcList(src),mDst(dst){};


protected:
     size_t getNbInputs() const {return(mSrcList.size());};

     IN  *getReadBuffer(int id=0,int nb = inputSize) {return mSrcList[id]->getReadBuffer(nb);};
     OUT *getWriteBuffer(int nb = outputSize) {return mDst.getWriteBuffer(nb);};

     bool willUnderflow(int id=0,int nb = inputSize) const {return mSrcList[id]->willUnderflowWith(nb);};
     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};

     int inputWidth(int id=0)  const  {return mSrcList[id]->width();};
     int inputHeight(int id=0) const  {return mSrcList[id]->height();};

     int outputWidth()  const  {return mDst.width();};
     int outputHeight() const  {return mDst.height();};

private:
    const std::vector<FIFOBase<IN>*> mSrcList;
    FIFOBase<OUT> &mDst;

};

template<typename IN, int inputSize,
         typename OUT, int outputSize>
class GenericManyToManyNode:public NodeBase
{
public:
     explicit GenericManyToManyNode(std::initializer_list<FIFOBase<IN>*> src,
                           std::initializer_list<FIFOBase<OUT>*> dst):mSrcList(src),mDstList(dst){};

    
protected:
     size_t getNbInputs() const {return(mSrcList.size());};
     size_t getNbOutputs() const {return(mDstList.size());};

     IN  *getReadBuffer(int id=0,int nb = inputSize) {return mSrcList[id]->getReadBuffer(nb);};
     OUT *getWriteBuffer(int id=0,int nb = outputSize) {return mDstList[id]->getWriteBuffer(nb);};

     bool willUnderflow(int id=0,int nb = inputSize) const {return mSrcList[id]->willUnderflowWith(nb);};
     bool willOverflow(int id=0,int nb = outputSize) const {return mDstList[id]->willOverflowWith(nb);};

     int inputWidth(int id=0)  const  {return mSrcList[id]->width();};
     int inputHeight(int id=0) const  {return mSrcList[id]->height();};

     int outputWidth(int id=0)  const  {return mDstList[id]->width();};
     int outputHeight(int id=0) const  {return mDstList[id]->height();};

private:
    const std::vector<FIFOBase<IN>*> mSrcList;
    const std::vector<FIFOBase<OUT>*> mDstList;
};

template<typename IN, int inputSize,typename OUT1, int output1Size,typename OUT2, int output2Size>
class GenericNode12:public NodeBase
{
public:
     explicit GenericNode12(FIFOBase<IN> &src,FIFOBase<OUT1> &dst1,FIFOBase<OUT2> &dst2):mSrc(src),
     mDst1(dst1),mDst2(dst2){};

protected:
     OUT1 * getWriteBuffer1(int nb=output1Size) {return mDst1.getWriteBuffer(nb);};
     OUT2 * getWriteBuffer2(int nb=output2Size) {return mDst2.getWriteBuffer(nb);};
     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow1(int nb = output1Size) const {return mDst1.willOverflowWith(nb);};
     bool willOverflow2(int nb = output2Size) const {return mDst2.willOverflowWith(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

     int inputWidth()  const  {return mSrc.width();};
     int inputHeight() const  {return mSrc.height();};

     int output1Width()  const  {return mDst1.width();};
     int output1Height() const  {return mDst1.height();};

     int output2Width()  const  {return mDst2.width();};
     int output2Height() const  {return mDst2.height();};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT1> &mDst1;
    FIFOBase<OUT2> &mDst2;
};

template<typename IN,   int inputSize,
         typename OUT1, int output1Size,
         typename OUT2, int output2Size,
         typename OUT3, int output3Size>
class GenericNode13:public NodeBase
{
public:
     explicit GenericNode13(FIFOBase<IN> &src,
                   FIFOBase<OUT1> &dst1,
                   FIFOBase<OUT2> &dst2,
                   FIFOBase<OUT3> &dst3
                   ):mSrc(src),
     mDst1(dst1),mDst2(dst2),mDst3(dst3){};

protected:
     OUT1 * getWriteBuffer1(int nb=output1Size) {return mDst1.getWriteBuffer(nb);};
     OUT2 * getWriteBuffer2(int nb=output2Size) {return mDst2.getWriteBuffer(nb);};
     OUT3 * getWriteBuffer3(int nb=output3Size) {return mDst3.getWriteBuffer(nb);};

     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willOverflow1(int nb = output1Size) const {return mDst1.willOverflowWith(nb);};
     bool willOverflow2(int nb = output2Size) const {return mDst2.willOverflowWith(nb);};
     bool willOverflow3(int nb = output3Size) const {return mDst3.willOverflowWith(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};

     int inputWidth()  const  {return mSrc.width();};
     int inputHeight() const  {return mSrc.height();};

     int output1Width()  const  {return mDst1.width();};
     int output1Height() const  {return mDst1.height();};

     int output2Width()  const  {return mDst2.width();};
     int output2Height() const  {return mDst2.height();};

     int output3Width()  const  {return mDst3.width();};
     int output3Height() const  {return mDst3.height();};

private:
    FIFOBase<IN> &mSrc;
    FIFOBase<OUT1> &mDst1;
    FIFOBase<OUT2> &mDst2;
    FIFOBase<OUT3> &mDst3;

};

template<typename IN1, int input1Size,typename IN2, int input2Size,typename OUT, int outputSize>
class GenericNode21:public NodeBase
{
public:
     explicit GenericNode21(FIFOBase<IN1> &src1,FIFOBase<IN2> &src2,FIFOBase<OUT> &dst):mSrc1(src1),
     mSrc2(src2),
     mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb=outputSize) {return mDst.getWriteBuffer(nb);};
     IN1 * getReadBuffer1(int nb=input1Size) {return mSrc1.getReadBuffer(nb);};
     IN2 * getReadBuffer2(int nb=input2Size) {return mSrc2.getReadBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};
     bool willUnderflow1(int nb = input1Size) const {return mSrc1.willUnderflowWith(nb);};
     bool willUnderflow2(int nb = input2Size) const {return mSrc2.willUnderflowWith(nb);};

     int input1Width()  const  {return mSrc1.width();};
     int input1Height() const  {return mSrc1.height();};

     int input2Width()  const  {return mSrc2.width();};
     int input2Height() const  {return mSrc2.height();};

     int outputWidth()  const  {return mDst.width();};
     int outputHeight() const  {return mDst.height();};

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;
    FIFOBase<OUT> &mDst;
};

template<typename IN1, int input1Size,
         typename IN2, int input2Size,
         typename IN3, int input3Size,
         typename OUT, int outputSize>
class GenericNode31:public NodeBase
{
public:
     explicit GenericNode31(FIFOBase<IN1> &src1,
                   FIFOBase<IN2> &src2,
                   FIFOBase<IN3> &src3,
                   FIFOBase<OUT> &dst):mSrc1(src1),
     mSrc2(src2),
     mSrc3(src3),
     mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb=outputSize) {return mDst.getWriteBuffer(nb);};
     IN1 * getReadBuffer1(int nb=input1Size) {return mSrc1.getReadBuffer(nb);};
     IN2 * getReadBuffer2(int nb=input2Size) {return mSrc2.getReadBuffer(nb);};
     IN3 * getReadBuffer3(int nb=input3Size) {return mSrc3.getReadBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};
     bool willUnderflow1(int nb = input1Size) const {return mSrc1.willUnderflowWith(nb);};
     bool willUnderflow2(int nb = input2Size) const {return mSrc2.willUnderflowWith(nb);};
     bool willUnderflow3(int nb = input3Size) const {return mSrc3.willUnderflowWith(nb);};

     int input1Width()  const  {return mSrc1.width();};
     int input1Height() const  {return mSrc1.height();};

     int input2Width()  const  {return mSrc2.width();};
     int input2Height() const  {return mSrc2.height();};

     int input3Width()  const  {return mSrc3.width();};
     int input3Height() const  {return mSrc3.height();};

     int outputWidth()  const  {return mDst.width();};
     int outputHeight() const  {return mDst.height();};

private:
    FIFOBase<IN1> &mSrc1;
    FIFOBase<IN2> &mSrc2;
    FIFOBase<IN3> &mSrc3;

    FIFOBase<OUT> &mDst;
};



template<typename OUT, int outputSize>
class GenericSource:public NodeBase
{
public:
     explicit GenericSource(FIFOBase<OUT> &dst):mDst(dst){};

protected:
     OUT * getWriteBuffer(int nb=outputSize) {return mDst.getWriteBuffer(nb);};

     bool willOverflow(int nb = outputSize) const {return mDst.willOverflowWith(nb);};


     int outputWidth()  const  {return mDst.width();};
     int outputHeight() const  {return mDst.height();};

private:
    FIFOBase<OUT> &mDst;
};

template<typename IN,int inputSize>
class GenericSink:public NodeBase
{
public:
     explicit GenericSink(FIFOBase<IN> &src):mSrc(src){};

protected:
     IN * getReadBuffer(int nb=inputSize) {return mSrc.getReadBuffer(nb);};

     bool willUnderflow(int nb = inputSize) const {return mSrc.willUnderflowWith(nb);};


     int inputWidth()  const  {return mSrc.width();};
     int inputHeight() const  {return mSrc.height();};

private:
    FIFOBase<IN> &mSrc;
};


#define REPEAT(N) for(int i=0;i<N;i++)


/***************
 * 
 * BUFFER MANAGEMENT SUPPORT
 * 
 * Classes to help manage buffers in a graph
 * 
 **************/
template<typename T, bool shared=true>
struct Shared {

  explicit Shared(T* t) : val_(t) {}
  Shared(int) : val_(nullptr) {}
  

  /*

  When shared we can only get a pointer to
  const data

  */
  const T* get() const { return val_; }

  Shared<T,true> share()
  {
      return(*this);
  };

 
private:
  
  T* val_;
};

template<typename T>
struct Shared<T,false> {
  explicit Shared(T* t) : val_(t) {}
  Shared(int) : val_(nullptr) {}

  /*

  When unique we can get a pointer to
  non const data

  */
  T* get() { return val_; }
  const T* get() const { return val_; }

  operator T*() { return val_; }


  Shared<T,true> share()
  {
      Shared<T,true> res(val_);

      return(res);
  };


private:
  T* val_;


};


template<typename IN, int inputSize,
         typename OUT, int outputSize>
class Duplicate;

template<typename IO, 
         bool inputShareStatus,
         int ioSize>
class Duplicate<Shared<IO,inputShareStatus>, ioSize,
                Shared<IO,true>, ioSize>:
public GenericToManyNode<Shared<IO,inputShareStatus>, ioSize,
                         Shared<IO,true>, ioSize>
{
public:
    explicit Duplicate(FIFOBase<Shared<IO,inputShareStatus>> &src,
              std::initializer_list<FIFOBase<Shared<IO,true>>*> dst):
    GenericToManyNode<Shared<IO,inputShareStatus> , ioSize,
                      Shared<IO,true>, ioSize>(src,dst)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           if (this->willOverflow(i))
           {
              return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
           }
        }


        return(CG_SUCCESS_ID_CODE);
    };

    int run() final {
        Shared<IO,inputShareStatus> *a=this->getReadBuffer();
        
        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           Shared<IO,true> *b=this->getWriteBuffer(i);
           for(int k=0;k<ioSize;k++)
           {
              b[k] = a[k].share();
           }
        }
        
        return(CG_SUCCESS_ID_CODE);
    };

};

template<typename IO, int inputOutputSize>
class Duplicate<IO, inputOutputSize,
                IO, inputOutputSize>:
public GenericToManyNode<IO, inputOutputSize,
                         IO, inputOutputSize>
{
public:
    explicit Duplicate(FIFOBase<IO> &src,
              std::initializer_list<FIFOBase<IO>*> dst):
    GenericToManyNode<IO, inputOutputSize,IO, inputOutputSize>(src,dst)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           if (this->willOverflow(i))
           {
              return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
           }
        }


        return(CG_SUCCESS_ID_CODE);
    };

    int run() final {
        IO *a=this->getReadBuffer();
        
        for(unsigned int i=0;i<this->getNbOutputs();i++)
        {
           IO *b=this->getWriteBuffer(i);
           memcpy(b,a,sizeof(IO)*inputOutputSize);
        }
        
        return(CG_SUCCESS_ID_CODE);
    };

};


} // End namespace
  

#endif
