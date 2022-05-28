//
// Created by Erik on 24/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_COMBINEDTEST_HPP
#define REDBLACKMEMORYMANAGER_COMBINEDTEST_HPP
#define CATCH_CONFIG_RUNNER
#include "TestClass.hpp"
#include "TestMemoryStrategy.hpp"
#include "MemoryManager.hpp"
#include "MemoryStrategy.h"
#include "Utils.hpp"
#include <catch.hpp>

TEST_CASE("Small combined test", "[SmallCombinedTest]")
{
    MemoryManager<TestMemoryStrategy> manager;
    ManagedPtr<TestClass, TestMemoryStrategy> value = manager.Create<TestClass>();
    ManagedPtr<size_t, TestMemoryStrategy> value2 = manager.Create<size_t>();

    value->At(0) = 741;
    value->At(1) = 11;
    size_t maximumLong = pow(2, 64);
    *value2 = maximumLong;

    SECTION("Value check")
    {
        REQUIRE(value->At(0) == 741);
        REQUIRE(value->At(1) == 11);
        REQUIRE(*value2 == maximumLong);

        REQUIRE(manager.GetUsedMemory() == sizeof(size_t) + sizeof(TestClass));

        auto it = manager.GetMemoryMap().begin();
        REQUIRE(it->first == 0);
        REQUIRE(it->second->GetOffset() == 0);
        REQUIRE(it->second->GetSize() == sizeof(TestClass));

        it++;
        REQUIRE(it->first == sizeof(TestClass));
        REQUIRE(it->second->GetOffset() == sizeof(TestClass));
        REQUIRE(it->second->GetSize() == sizeof(size_t));
    }
}

#endif //REDBLACKMEMORYMANAGER_COMBINEDTEST_HPP
