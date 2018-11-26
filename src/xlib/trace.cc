/*
 * Copyright [2018] <Copyright u35s>
 */

#include <execinfo.h>
#include <cxxabi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <stack>
#include <memory>

#include "xlib/trace.h"
#include "xlib/log.h"

namespace xlib {

void DemangleSymbol(std::string* symbol) {
    size_t size = 0;
    int status = -4;
    char temp[1024] = {'\0'};
    /*
     * ./px(_ZN11ProxyServer6UpdateEv+0x4c) [0x40d5fa]
     * ./px(_ZN11ProxyServer5ServeEv+0x75) [0x40d853]
     * ./px(main+0x105) [0x40cbf2]
     *  匹配括号中的符号调用cxa_demangle替换
     * ./px(ProxyServer::Update()+0x4c) [0x40d5fa]
     * ./px(ProxyServer::Serve()+0x75) [0x40d853]
     * ./px(main+0x105) [0x40cbf2]
     */
    if (sscanf(symbol->c_str(), "%*[^(_](%[^ )+]", temp) == 1) {
        char* demangled = abi::__cxa_demangle(temp, NULL, &size, &status);
        if (demangled != NULL) {
            int start = symbol->find(temp);
            symbol->replace(start, strlen(temp), "");
            symbol->insert(start, demangled);
            free(demangled);
        }
    }
}

void GetStackTrace(std::string* stack) {
    void* addresses[1024];
    int size = backtrace(addresses, 1024);
    char** symbols = backtrace_symbols(addresses, size);
    for (int i = 0; i < size; ++i) {
        std::string demangled(symbols[i]);
        DemangleSymbol(&demangled);
        stack->append(demangled);
        stack->append("\n");
    }
    free(symbols);
}

void PrintStack() {
    std::string stack;
    GetStackTrace(&stack);
    ERR("%s", stack.c_str());
}

}  // namespace xlib
