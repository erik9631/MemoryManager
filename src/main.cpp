#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <iostream>
#include "IntegerTests.hpp"
#include "ObjectTests.hpp"
#include "Benchmarks.h"
#include "CombinedTest.hpp"

int main( int argc, char* argv[] )
{
    Catch::Session().run(argc, argv);
    return 0;
}
