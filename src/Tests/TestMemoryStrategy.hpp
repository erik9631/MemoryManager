//
// Created by Erik on 23/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_TESTMEMORYSTRATEGY_HPP
#define REDBLACKMEMORYMANAGER_TESTMEMORYSTRATEGY_HPP
#include "MemoryStrategy.h"
#include "MetaData.hpp"
#include <memory.h>
#include <cstdlib>
using namespace MemManager;

class TestMemoryStrategy : public MemoryStrategy
{
public:
    size_t CreateMemoryChunk(const size_t &size) override
    {
        void* newAddr = malloc(size);
        return (size_t)newAddr;
    }

    size_t Realloc(const size_t &memoryStartAddr, const size_t &oldSize, const size_t &newSize) override
    {
        void* oldAddr = (void*)memoryStartAddr;
        void* newBlockAddr = (void*)malloc(newSize);
        memcpy(newBlockAddr, oldAddr, oldSize);
        free(oldAddr);
        return (size_t)newBlockAddr;

    }
    template<typename T, typename MetaDataType>
    void EraseData(const MetaData<MetaDataType>& metaData, size_t& usedMemory, const size_t& memoryStartAddr)
    {
        T* objPtr = reinterpret_cast<T*>(metaData.GetOffset() + memoryStartAddr);
        objPtr->~T();
        usedMemory -= sizeof(T);
    }


    template<typename T, typename MetaDataType, typename ... Args>
    void CopyData(MetaData<MetaDataType>& metaData, size_t& usedMemory, const size_t& memoryStartAddr, T* src)
    {
        memcpy((void*)memoryStartAddr, (void*)src, metaData.GetSize());
        usedMemory+= metaData.GetSize();
    }

    template<typename T, typename MetaDataType, typename ... Args>
    void WriteData(MetaData<MetaDataType>& metaData, size_t& usedMemory, const size_t& memoryStartAddr, const size_t& size, Args ... args)
    {
        if(size > 1)
            new((void*)(metaData.GetOffset() + memoryStartAddr)) T[size];
        else
            new((void*)(metaData.GetOffset() + memoryStartAddr)) T(args ...);
        usedMemory+= metaData.GetSize();
    }

    template<typename T>
    T* ReadData(const size_t& offset, const size_t& memoryStartAddr)
    {
        return reinterpret_cast<T*>(memoryStartAddr+offset);
    }


};



#endif //REDBLACKMEMORYMANAGER_TESTMEMORYSTRATEGY_HPP
