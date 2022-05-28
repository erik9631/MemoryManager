set(Src
    main.cpp
    )
source_group(RedBlackMemoryManager/src FILES ${src})

set(MemoryManager
        MemoryManager/MemoryManager.hpp
        MemoryManager/MetaData.hpp
        MemoryManager/ManagedPtr.hpp
        )
source_group(RedBlackMemoryManager/src/MemoryManager FILES ${MemoryManager})

set(Api
        Api/MemoryStrategy.h
)
source_group(RedBlackMemoryManager/src/Api FILES ${Api})

set(Tests
        Tests/IntegerTests.hpp
        Tests/TestMemoryStrategy.hpp
        Tests/ObjectTests.hpp Tests/TestClass.hpp Tests/Utils.hpp Tests/CombinedTest.hpp Tests/Benchmarks.h)
source_group(RedBlackMemoryManager/src/Tests FILES ${Tests})


set(ALL_FILES
    ${Src}
    ${MemoryManager}
    ${Api}
    ${Tests}
    )


add_executable(RedBlackMemoryManager ${ALL_FILES})

target_include_directories(RedBlackMemoryManager PRIVATE
        Api
        MemoryManager
        Tests
        )