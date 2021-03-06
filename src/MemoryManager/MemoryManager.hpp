//
// Created by Erik on 07/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_MEMORYMANAGER_HPP
#define REDBLACKMEMORYMANAGER_MEMORYMANAGER_HPP
#include <map>
#include "MetaData.hpp"
#include <memory>
#include "MemoryStrategy.h"
#include "ManagedPtr.hpp"
#include <cassert>

namespace MemManager
{
    template<class MemoryStrategyType = void, class MetaDataType = size_t>
    class MemoryManager
    {
    private:
        std::multimap<size_t, size_t> freeMemoryMap;
        std::map<size_t, size_t> freeOffsetMemoryMap;
        std::map<size_t, std::unique_ptr<MetaData<MetaDataType>>> allocatedMemoryMap;

        MemoryStrategyType memoryStrategy;
        size_t currentMemorySize = 1024;
        size_t oldMemorySize = 0;
        size_t usedMemory = 0;
        size_t memoryStartAddr = 0;

        void RoundToPowerOfTwo(size_t& value)
        {
            unsigned char position = 1;
            while(value != 1)
            {
                value >>= 1;
                position++;
            }
            value <<= position;
        }

        void Realloc(const size_t &minSize)
        {
            if(memoryStartAddr != 0)
                currentMemorySize = currentMemorySize << 1;

            if(currentMemorySize < minSize)
            {
                size_t newSize = minSize;
                RoundToPowerOfTwo(newSize);
                currentMemorySize = newSize;
            }
            //Get the node that is being extended
            const auto& lastNode = freeOffsetMemoryMap.find(usedMemory);
            if(lastNode != freeOffsetMemoryMap.end())
            {
                freeMemoryMap.erase(GetFreeMapValue(lastNode->second, lastNode->first));
                freeOffsetMemoryMap.erase(lastNode);
            }

            AddFreeMemoryNode({currentMemorySize - usedMemory, usedMemory});
            if(memoryStartAddr == 0)
            {
                memoryStartAddr = memoryStrategy.CreateMemoryChunk(currentMemorySize);
                oldMemorySize = currentMemorySize;
                return;
            }
            memoryStartAddr = memoryStrategy.Realloc(memoryStartAddr, oldMemorySize, currentMemorySize);
            oldMemorySize = currentMemorySize;
        }

        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> CreateMapEntry(MetaData<MetaDataType>* metaData, std::multimap<size_t, size_t>::iterator& freeSegment)
        {
            size_t remainingSize = freeSegment->first - metaData->GetSize();
            size_t newOffset =  metaData->GetSize() + metaData->GetOffset();
            EraseFreeMemoryNodes(freeSegment);
            if(remainingSize != 0)
                AddFreeMemoryNode({remainingSize, newOffset});

            auto newMetaData = std::unique_ptr<MetaData<MetaDataType>>(metaData);
            allocatedMemoryMap.insert({metaData->GetOffset(), std::move(newMetaData)});
            return std::move(ManagedPtr<T, MemoryStrategyType, MetaDataType>{this, metaData});
        }

        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> WriteDataToMemory(std::multimap<size_t, size_t>::iterator& freeSegment, const size_t& count, Args ... constructorArgs)
        {
            size_t size = sizeof(T) * count;
            size_t offset = freeSegment->second;
            auto* metaData = new MetaData<MetaDataType>(offset, size);
            auto managedPtr = CreateMapEntry<T, MemoryStrategyType, MetaDataType>(metaData, freeSegment);
            memoryStrategy.WriteData<T>(*metaData, usedMemory, memoryStartAddr, count, constructorArgs...);

            return managedPtr;
        }

        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> CopyDataToMemory(std::multimap<size_t, size_t>::iterator& freeSegment, T* data, const size_t& count)
        {
            size_t size = sizeof(T) * count;
            size_t offset = freeSegment->second;
            auto* metaData = new MetaData<MetaDataType>(offset, size);
            auto managedPtr = CreateMapEntry<T, MemoryStrategyType, MetaDataType>(metaData, freeSegment);
            memoryStrategy.CopyData<T>(*metaData, usedMemory, memoryStartAddr, data);

            return managedPtr;
        }


        void EraseFreeMemoryNodes(std::multimap<size_t, size_t>::iterator& freeSegment)
        {
            auto offsetIt = freeOffsetMemoryMap.find(freeSegment->second);
            if(offsetIt == freeOffsetMemoryMap.end())
            {
                throw std::exception("fatal error, offset not found!");
            }
            freeOffsetMemoryMap.erase(offsetIt);
            freeMemoryMap.erase(freeSegment);
        }

        void AddFreeMemoryNode(const std::pair<size_t, size_t> &nodes)
        {
            freeMemoryMap.insert({nodes});
            freeOffsetMemoryMap.insert({nodes.second, nodes.first});
        }

        std::multimap<size_t, size_t>::iterator GetFreeMapValue(const size_t& size, const size_t& offset)
        {
            const auto& offsetIterator = freeOffsetMemoryMap.find(offset);
            if(offsetIterator == freeOffsetMemoryMap.end())
                return freeMemoryMap.end();
            const auto& freeMemoryRange = freeMemoryMap.equal_range(offsetIterator->second);
            for(auto it = freeMemoryRange.first; it != freeMemoryRange.second; it++)
            {
                if(it->second == offset)
                    return it;
            }
            return freeMemoryMap.end();
        }

        void MergeMemory()
        {
            std::pair<size_t, size_t> lastPair = {-1, -1};
            for(auto pair : freeOffsetMemoryMap)
            {
                if(lastPair.first == -1)
                {
                    lastPair = pair;
                    continue;
                }
                //If the offset + its size place us to the next pair, that must mean they are next to each other
                if(lastPair.first + lastPair.second == pair.first)
                {
                    EraseFreeMemoryNodes(GetFreeMapValue(pair.second, pair.first));
                    EraseFreeMemoryNodes(GetFreeMapValue(lastPair.second, lastPair.first));
                    AddFreeMemoryNode({lastPair.second+pair.second, lastPair.first});
                    return;
                }
                lastPair = pair;
            }
        }

    public:
        template<typename ... Args>
        MemoryManager(Args ... args) : memoryStrategy(args ...){}

        MemoryStrategyType& GetMemoryStrategy()
        {
            return memoryStrategy;
        }

        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> Create(Args ... constructorArgs)
        {
            return CreateArray<T>(1, constructorArgs ...);
        }

        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> CreateArray(const size_t& count, Args ... constructorArgs)
        {
            size_t requestedSize = sizeof(T)*count;
            //Find exact match
            auto bestFitSegment = freeMemoryMap.find(requestedSize);

            //Find closest match
            if(bestFitSegment == freeMemoryMap.end())
                bestFitSegment = freeMemoryMap.upper_bound(requestedSize);

            //If no segment is found, realloc is needed
            if(bestFitSegment == freeMemoryMap.end())
            {
                Realloc(sizeof(T));
                return CreateArray<T>(count, constructorArgs ...);
            }
            return WriteDataToMemory<T, Args ...>(bestFitSegment, count, constructorArgs ...);
        }
        template<typename T, typename ... Args>
        ManagedPtr<T, MemoryStrategyType, MetaDataType> Copy(T* data, const size_t& count)
        {
            size_t requestedSize = sizeof(T)*count;
            //Find exact match
            auto bestFitSegment = freeMemoryMap.find(requestedSize);

            //Find closest match
            if(bestFitSegment == freeMemoryMap.end())
                bestFitSegment = freeMemoryMap.upper_bound(requestedSize);

            //If no segment is found, realloc is needed
            if(bestFitSegment == freeMemoryMap.end())
            {
                Realloc(sizeof(T));
                return Copy<T>(data, count);
            }
            return CopyDataToMemory<T, Args ...>(bestFitSegment, data, count);
        }

        template<class T>
        void Remove(const MetaData<MetaDataType>& metaData)
        {
            const auto& iterator = allocatedMemoryMap.find(metaData.GetOffset());
            if(iterator == allocatedMemoryMap.end())
                assert("Error, memory address isn't allocated");
            memoryStrategy.EraseData<T>(metaData, usedMemory, memoryStartAddr);
            AddFreeMemoryNode({metaData.GetSize(), metaData.GetOffset()});
            allocatedMemoryMap.erase(iterator);
            MergeMemory();
        }

        template<typename T>
        T* GetData(size_t offset)
        {
            return memoryStrategy.ReadData<T>(offset, memoryStartAddr);
        }

        const std::multimap<size_t, size_t>& GetFreeMemoryMap() const
        {
            return freeMemoryMap;
        }

        const std::map<size_t, size_t>& GetFreeOffsetMap() const
        {
            return freeOffsetMemoryMap;
        }

        const std::map<size_t, std::unique_ptr<MetaData<MetaDataType>>>& GetMemoryMap() const
        {
            return allocatedMemoryMap;
        }
        const size_t& GetUsedMemory()
        {
            return usedMemory;
        }

        const size_t GetFreeMemory()
        {
            return currentMemorySize - usedMemory;
        }

    };
}



#endif //REDBLACKMEMORYMANAGER_MEMORYMANAGER_HPP
