#ifndef VERBOSE_ASSERTION_H
#define VERBOSE_ASSERTION_H

#include <source_location>
#include <string>
#include <cstdlib>
#include <windows.h>

inline void VerboseAssertion(const bool expr, const char* expr_str,
    const std::source_location& loc = std::source_location::current())
{
    if (!expr)
    {
        std::string msg = "Assertion failed: ";
        msg += expr_str;
        msg += "\nIn function: ";
        msg += loc.function_name();
        msg += "\nFile: ";
        msg += loc.file_name();
        msg += ":";
        msg += std::to_string(loc.line());

        MessageBoxA(nullptr, msg.c_str(), "Assertion Failed", MB_ICONERROR | MB_OK);
        std::exit(EXIT_FAILURE);
    }
}

#define vassert(expr) VerboseAssertion((expr), #expr)

#endif
