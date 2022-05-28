//
// Created by Erik on 24/05/22.
//
#ifndef REDBLACKMEMORYMANAGER_OBJECTTESTS_H
#define CATCH_CONFIG_RUNNER
#include "TestClass.hpp"
#include "TestMemoryStrategy.hpp"
#include "MemoryManager.hpp"
#include "MemoryStrategy.h"
#include "Utils.hpp"
#include <catch.hpp>
using namespace MemManager;




TEST_CASE("Basic object memory test", "[ObjectMemoryTestBasic]")
{

    MemoryManager<TestMemoryStrategy> manager;
    ManagedPtr<TestClass, TestMemoryStrategy> value = manager.Create<TestClass>();

    value->At(0) = 741;
    value->At(1) = 11;


    SECTION("Integer value check for value 1")
    {
        REQUIRE(value->At(0) == 741);
        REQUIRE(value->At(1) == 11);
    }

    ManagedPtr<TestClass, TestMemoryStrategy> value2 = manager.Create<TestClass>();
    value2->At(0) = 800;
    value2->At(1) = 984;

    SECTION("Integer value check for value 2")
    {
        REQUIRE(value2->At(0) == 800);
        REQUIRE(value2->At(1) == 984);
    }


    SECTION("Memory map check")
    {
        REQUIRE(manager.GetFreeMemoryMap().size() == 1);
        REQUIRE(manager.GetFreeMemoryMap().begin()->first == 8192 - sizeof(TestClass)*2);

        int offset = 0;
        for(auto& it : manager.GetMemoryMap())
        {
            REQUIRE(it.first == offset);
            REQUIRE(it.second->GetOffset() == offset);
            REQUIRE(it.second->GetSize() == sizeof(TestClass));
            offset+=sizeof(TestClass);
        }
    }
}



TEST_CASE("Object Memory Test with large values", "[ObjectMemoryTestLarge]")
{
    MemoryManager<TestMemoryStrategy> manager;
    SECTION("1000 objects realloc and value test")
    {
        const unsigned int size = 1000;
        auto* testClassVec = InitManager(manager, size);
        CascadeFillValues(testClassVec);
        REQUIRE(CheckCascadeValues(testClassVec) == true);
        REQUIRE(manager.GetUsedMemory() == CalculateUsedMemory<TestClass>(size));
        REQUIRE(manager.GetFreeMemory() ==  CalculateFreeMemory<TestClass>(size));
        CheckCascadeAllocationMemoryMap(manager, size);
    }

    SECTION("100000 objects realloc and value test")
    {
        const unsigned int size = 100000;
        auto* testClassVec = InitManager(manager, size);
        CascadeFillValues(testClassVec);
        REQUIRE(CheckCascadeValues(testClassVec) == true);
        REQUIRE(manager.GetUsedMemory() == CalculateUsedMemory<TestClass>(size));
        REQUIRE(manager.GetFreeMemory() ==  CalculateFreeMemory<TestClass>(size));
        CheckCascadeAllocationMemoryMap(manager, size);
    }

    SECTION("1000 objects realloc, remove and value test")
    {
        const unsigned int size = 1000;
        auto* testClassVec = InitManager(manager, size);
        CascadeFillValues(testClassVec);
        REQUIRE(CheckCascadeValues(testClassVec) == true);
        REQUIRE(manager.GetUsedMemory() == CalculateUsedMemory<TestClass>(size));
        REQUIRE(manager.GetFreeMemory() == CalculateFreeMemory<TestClass>(size));
        CheckCascadeAllocationMemoryMap(manager, size);
        //Keep every third value
        int counter = 0;
        for(auto& val : *testClassVec)
        {
            if(counter == 3)
            {
                counter = 0;
                continue;
            }
            val.Free();
            counter++;
        }
        REQUIRE(manager.GetUsedMemory() == size * sizeof(TestClass) * 1/4);
        REQUIRE(manager.GetFreeMemory() == (RoundToPowerOfTwo(size * sizeof(TestClass))) - (size * sizeof(TestClass) * 1/4));

        counter = 0;
        unsigned int offsetCounter = 0;
        unsigned int stride = sizeof(TestClass) * 4;
        unsigned int offset = sizeof(TestClass) * 3;
        for(auto& val : manager.GetMemoryMap())
        {
            if(counter == 3)
            {
                REQUIRE(val.first == stride * offsetCounter + offset);
                counter = 0;
                offsetCounter++;
                continue;
            }
            offsetCounter++;
            counter++;
        }

        counter = 0;
        unsigned int totalCount = 1000 * 1/4;
        for(auto& val : manager.GetFreeMemoryMap())
        {
            //Once we are at the last one, calculate how much remaining memory there is and check
            if(counter == totalCount)
            {
                REQUIRE(val.first == CalculateFreeMemory<TestClass>(size));
                REQUIRE(val.second == (stride * counter));
                counter++;
                break;
            }
            REQUIRE(val.first == sizeof(TestClass) * 3);
            REQUIRE(val.second == (stride * counter));
            counter++;
        }
        REQUIRE(manager.GetFreeMemoryMap().size() == counter);
    }

}

#define REDBLACKMEMORYMANAGER_OBJECTTESTS_H


#endif //REDBLACKMEMORYMANAGER_OBJECTTESTS_H
