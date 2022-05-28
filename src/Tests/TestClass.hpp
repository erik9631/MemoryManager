//
// Created by Erik on 24/05/22.
//

#ifndef REDBLACKMEMORYMANAGER_TESTCLASS_HPP
#define REDBLACKMEMORYMANAGER_TESTCLASS_HPP
class TestClass
{
public:
    const unsigned int& GetValueAt(const unsigned int& index)
    {
        return values[index];
    }
    void AddIndex(const unsigned int& a, const unsigned int& b)
    {
        values[a] += values[b];
    }
    unsigned int& operator [] (const unsigned int& index)
    {
        return values[index];
    }

    unsigned int& At(const unsigned int& index)
    {
        return values[index];
    }
    static constexpr int arraySize = 1000;
private:
    unsigned int values[arraySize];
};
#endif //REDBLACKMEMORYMANAGER_TESTCLASS_HPP
