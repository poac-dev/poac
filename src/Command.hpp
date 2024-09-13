#pragma once

#include <cstdio>
#include <iostream>
#include <string>

class Command
{
private:
    std::string buf;
public:
    Command(std::string arg) {
        buf.append(arg);
    }
    ~Command() {
    }
    Command arg(std::string arg) {
        buf.append(" ");
        buf.append(arg);
        return *this;
    }
    Command output() {
        std::cout << buf << std::endl;
        system(buf.data());
        return *this;
    }
};