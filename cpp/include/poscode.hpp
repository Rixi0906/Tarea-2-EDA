#ifndef __POSCODE_HPP__
#define __POSCODE_HPP__
#include "poscode.hpp"
#include <string>

class Poscode{
    private:
        std::string data;
    public:
        Poscode();
        Poscode(std::string _data);
        char getValue(size_t i);   
        const std::string &getData(); 
};

#endif