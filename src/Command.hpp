#pragma once

#include <iostream>
#include <string>
#include <cstring>

class Command
{
private:
    char* buf;
    int len;
public:
    Command(const char *arg) {
        len = 0;
        buf = (char *) malloc(sizeof(char) * strlen(arg));
        if (buf == NULL) {
            throw std::runtime_error("unable to allocate memory");
        }
        len += snprintf(buf+len, 3, "%s", arg);
    }
    Command arg(const char *arg) {
        buf = (char *) realloc(buf, strlen(buf)+strlen(arg));
        if (buf == NULL) {
            throw std::runtime_error("unable to allocate memory");
        }
        len += snprintf(buf+len, strlen(buf)+strlen(arg), " %s", arg);
        return *this;
    }
    Command output() {
        fprintf(stdout, "%s\n", buf);
        system(buf);
        free(buf);
        return *this;
    }
};