//
// Created by smoke on 5/29/2025.
//
#pragma once
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")

inline void printStackTrace() {
    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);

    void* stack[100];
    unsigned short frames = CaptureStackBackTrace(0, 100, stack, NULL);

    SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (unsigned int i = 0; i < frames; i++) {
        SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);
        std::cout << i << ": " << symbol->Name << " - 0x" << std::hex << symbol->Address << std::dec << std::endl;
    }

    free(symbol);
}
#elif defined(__APPLE__) || defined(__linux__)
#include <execinfo.h>

void printStackTrace() {
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);

    for (int i = 0; i < frames; ++i) {
        std::cout << strs[i] << std::endl;
    }

    free(strs);
}
#else
void printStackTrace() {
    std::cout << "Stack trace not supported on this platform" << std::endl;
}
#endif