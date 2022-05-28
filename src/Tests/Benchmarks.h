//
// Created by Erik on 24/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_BENCHMARKS_H
#define REDBLACKMEMORYMANAGER_BENCHMARKS_H

#define CATCH_CONFIG_RUNNER
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "TestClass.hpp"
#include "TestMemoryStrategy.hpp"
#include "MemoryManager.hpp"
#include "MemoryStrategy.h"
#include "Utils.hpp"
#include <catch.hpp>


int CustomMemoryTest(MemoryManager<TestMemoryStrategy>& manager, const size_t& size)
{
    for(int i = 0; i < size; i++)
        manager.Create<TestClass>();
    return 0;
}

int MallocTest(const size_t& size)
{
    for(int i = 0; i < size; i++)
        new TestClass();
    return 0;
}

TEST_CASE("Allocation speed test", "[BenchmarkSpeedAllocation]")
{
    const unsigned int size = 10000;
    MemoryManager<TestMemoryStrategy> manager;
    BENCHMARK("10 000 object benchmark")
    {
        return CustomMemoryTest(manager, size);
    };

    BENCHMARK("10 000 object malloc benchmark")
    {
        return MallocTest(size);
    };
}

#endif //REDBLACKMEMORYMANAGER_BENCHMARKS_H