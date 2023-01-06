#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <string>

class Allocator
{
public:
    static Allocator &getInstance();
    std::string fresh_var(const std::string &name);

private:
    int reg_num;
    Allocator();
};

#endif
