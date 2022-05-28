//
// Created by Erik on 24/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_UTILS_HPP
#define REDBLACKMEMORYMANAGER_UTILS_HPP

size_t RoundToPowerOfTwo(size_t value)
{
    unsigned char position = 1;
    while(value != 1)
    {
        value >>= 1;
        position++;
    }
    value <<= position;
    return value;
}
template<typename type>
size_t CalculateFreeMemory(const size_t& elementCount)
{
    return RoundToPowerOfTwo(elementCount * sizeof(type)) - elementCount *  sizeof(type);
}

template<typename type>
size_t CalculateUsedMemory(const size_t& elementCount)
{
    return elementCount * sizeof(TestClass);
}



void CascadeFillValues(std::vector<ManagedPtr<TestClass, TestMemoryStrategy>>* testClassVector)
{
    unsigned int counter = 0;
    for(auto& value : *testClassVector)
    {
        for(int i = 0; i < TestClass::arraySize; i++)
        {
            value->At(i) = counter + i;
        }
    }
}

bool CheckCascadeValues(std::vector<ManagedPtr<TestClass, TestMemoryStrategy>>* testClassVector)
{
    unsigned int counter = 0;
    for(auto& value : *testClassVector)
    {
        for(int i = 0; i < TestClass::arraySize; i++)
        {
            if(value->At(i) != counter + i)
                return false;
        }
    }
    return true;
}

void CheckCascadeAllocationMemoryMap(MemoryManager<TestMemoryStrategy>& memoryManager, unsigned int numberOfObjects)
{
    unsigned int counter = 0;
    unsigned int offset = 0;
    for(auto& memorySegment : memoryManager.GetMemoryMap())
    {
        REQUIRE(memorySegment.first == offset);
        REQUIRE(memorySegment.second->GetOffset() == offset);
        REQUIRE(memorySegment.second->GetSize() == sizeof(TestClass));
        offset += sizeof(TestClass);
        counter++;
    }
    REQUIRE(counter == numberOfObjects);
}

std::vector<ManagedPtr<TestClass, TestMemoryStrategy>>* InitManager(MemoryManager<TestMemoryStrategy>& memoryManager, unsigned int numberOfObjects)
{
    auto* resultVector = new std::vector<ManagedPtr<TestClass, TestMemoryStrategy>>();
    for(int i = 0; i < numberOfObjects; i++)
        resultVector->push_back(memoryManager.Create<TestClass>());
    return resultVector;
}


#endif //REDBLACKMEMORYMANAGER_UTILS_HPP
