//
// Created by Erik on 16/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_INTEGERTESTS_HPP
#define REDBLACKMEMORYMANAGER_INTEGERTESTS_HPP
#define CATCH_CONFIG_RUNNER
#include "MemoryManager.hpp"
#include "MetaData.hpp"
#include "TestMemoryStrategy.hpp"
#include <catch.hpp>



TEST_CASE("Integer Memory Test two values", "[IntegerMemoryTest1]")
{
    MemoryManager<TestMemoryStrategy> manager;
    ManagedPtr<int, TestMemoryStrategy> value = manager.Create<int>(741);
    ManagedPtr<int, TestMemoryStrategy> value2 = manager.Create<int>(11);

    SECTION("Integer value check")
    {
        REQUIRE(*value == 741);
        REQUIRE(*value2 == 11);
    }

    SECTION("Memory map check")
    {
        REQUIRE(manager.GetFreeMemoryMap().size() == 1);
        REQUIRE(manager.GetFreeMemoryMap().begin()->first == 1024 - sizeof(int)*2);

        int offset = 0;
        for(auto& it : manager.GetMemoryMap())
        {
            REQUIRE(it.first == offset);
            REQUIRE(it.second->GetOffset() == offset);
            REQUIRE(it.second->GetSize() == sizeof(int));
            offset+=sizeof(int);
        }
    }

}


void CheckIntegerMap(MemoryManager<TestMemoryStrategy>& manager, const unsigned int& elementCount,
                     const unsigned int& chunkSize, const unsigned int& freeMapSize , const char* testName)
{
    SECTION(testName)
    {
        REQUIRE(manager.GetFreeMemoryMap().size() == freeMapSize);
        int correctMemorySize = chunkSize - elementCount * sizeof(int);
        if(freeMapSize != 0)
            REQUIRE(manager.GetFreeMemoryMap().begin()->first == correctMemorySize);

        int offset = 0;
        for(auto& it : manager.GetMemoryMap())
        {
            REQUIRE(it.first == offset);
            REQUIRE(it.second->GetOffset() == offset);
            REQUIRE(it.second->GetSize() == sizeof(int));
            offset+=sizeof(int);
        }

    }
}

TEST_CASE("Integer Memory Test Large values", "[IntegerMemoryTest2]")
{
    MemoryManager<TestMemoryStrategy> manager;
    const int managerStartChunkSize = 1024;
    const int arraySizeWithoutRelloc = managerStartChunkSize / sizeof(int);
    std::vector<ManagedPtr<int, TestMemoryStrategy>> intArray;
    for(int i = 0; i < arraySizeWithoutRelloc; i++)
    {
        intArray.push_back(manager.Create<int>(i));
    }

    SECTION("Integer value check")
    {
        int counter = 0;
        for(auto& i : intArray)
        {
            REQUIRE( *i == counter);
            counter++;
        }
    }
    CheckIntegerMap(manager, arraySizeWithoutRelloc, managerStartChunkSize, 0, "Check memory map");

    //Add more integers and perform a realloc
    const int managerChunkSize = managerStartChunkSize << 1;
    const int arraySizeWithRealloc = arraySizeWithoutRelloc + 100;

    for(int i = 0; i < 100; i++)
    {
        intArray.push_back(manager.Create<int>(arraySizeWithoutRelloc+i));
    }

    CheckIntegerMap(manager, arraySizeWithRealloc, managerChunkSize, 1, "Check memory map 2");

    SECTION("Integer value check with realloc")
    {
        int counter = 0;
        for(auto& i : intArray)
        {
            auto& val = intArray[counter];
            REQUIRE(*i == counter);
            counter++;
        }
    }

    for(int i = 0; i < 500000; i++)
    {
        intArray.push_back(manager.Create<int>(arraySizeWithRealloc+i));
    }


    SECTION("Integer value check with 500000 ints")
    {
        int counter = 0;
        for(auto& i : intArray)
        {
            auto& val = intArray[counter];
            REQUIRE(*i == counter);
            counter++;
        }
    }

}

TEST_CASE("Integer Memory Test with remove", "[IntegerMemoryTestRemove]")
{
    MemoryManager<TestMemoryStrategy> manager;
    const int managerStartChunkSize = 1024;
    const int arraySizeWithoutRelloc = managerStartChunkSize / sizeof(int);
    std::vector<ManagedPtr<int, TestMemoryStrategy>> intArray;
    for(int i = 0; i < arraySizeWithoutRelloc; i++)
    {
        intArray.push_back(manager.Create<int>(i));
    }

    SECTION("Integer value check")
    {
        int counter = 0;
        for(auto& i : intArray)
        {
            REQUIRE( *i == counter);
            counter++;
        }
        CheckIntegerMap(manager, arraySizeWithoutRelloc, managerStartChunkSize, 0, "Check memory map");
    }


    SECTION("Remove integers")
    {
        int counter = 0;
        for(auto it = intArray.begin(); it != intArray.end(); it++)
        {
            if(counter & 1)
            {
                it->Free();
            }
            counter++;
        }

        SECTION("Check used memory size")
        {
            REQUIRE(manager.GetUsedMemory() == 512);
        }

        SECTION("Check memory map after remove")
        {
            counter = 0;
            for(auto i : manager.GetFreeMemoryMap())
            {
                REQUIRE(i.first == 4);
                counter++;
            }
            REQUIRE(counter == 128);
        }
    }
}

TEST_CASE("Merge memory test", "[MemoryTestMerge]")
{
    MemoryManager<TestMemoryStrategy> manager;
    const int managerStartChunkSize = 1024;
    const int arraySizeWithoutRelloc = managerStartChunkSize / sizeof(int);
    std::vector<ManagedPtr<int, TestMemoryStrategy>> intArray;
    for (int i = 0; i < arraySizeWithoutRelloc; i++)
    {
        intArray.push_back(manager.Create<int>(i));
    }

    SECTION("Object value check")
    {
        int counter = 0;
        for (auto &i: intArray)
        {
            REQUIRE(*i == counter);
            counter++;
        }
        CheckIntegerMap(manager, arraySizeWithoutRelloc, managerStartChunkSize, 0, "Check memory map");
    }

    int counter = 0;
    int erasedCounter = 0;
    for (auto it = intArray.begin(); it != intArray.end(); it++)
    {
        if (counter == 2)
        {
            counter = 0;
            continue;
        }
        it->Free();
        erasedCounter++;
        counter++;
    }
    int currentArraySize = arraySizeWithoutRelloc * (2.0f / 3.0f) * sizeof(int);
    //Round up to the closest number divisible by 4
    currentArraySize = (currentArraySize & (~3)) + 4;

        SECTION("Check used memory size")
        {
            REQUIRE(manager.GetFreeMemory() == currentArraySize);
        }

        SECTION("Check free memory map")
        {
            counter = 0;
            for(const auto& pair : manager.GetFreeMemoryMap())
            {
                if(counter == 0)
                    REQUIRE(pair.first == 4);
                else
                    REQUIRE(pair.first == 8);
                counter++;
            }
        }
        SECTION("Check allocation map")
        {
            counter = 0;
            size_t stride = 8+4;
            size_t offset = 8;
            for(const auto& pair : manager.GetMemoryMap())
            {
                REQUIRE(pair.first == (counter * (stride)) + offset );
                counter++;
            }
        }
}


#endif //REDBLACKMEMORYMANAGER_INTEGERTESTS_HPP
