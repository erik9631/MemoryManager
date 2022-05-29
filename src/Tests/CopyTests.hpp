//
// Created by Erik on 28/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_COPYTESTS_HPP
#define REDBLACKMEMORYMANAGER_COPYTESTS_HPP
#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING

#include "MemoryManager.hpp"
#include "MetaData.hpp"
#include "TestMemoryStrategy.hpp"
#include <catch.hpp>
using namespace MemManager;

TEST_CASE("Basic copy test", "[CopyTestBasic]")
{
    MemoryManager<TestMemoryStrategy> manager;
    int* values = new int[2];

    values[0] = 741;
    values[1] = 11;
    auto managedPtr = manager.Copy<int>(values, 2);

    SECTION("Integer value check")
    {
        REQUIRE(managedPtr[0] == 741);
        REQUIRE(managedPtr[1] == 11);
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
            REQUIRE(it.second->GetSize() == sizeof(int[2]));
            offset+=sizeof(int[2]);
        }
    }

}


#endif //REDBLACKMEMORYMANAGER_COPYTESTS_HPP
