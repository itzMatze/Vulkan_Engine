#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>

#define VE_C_RED "\033[91m"
#define VE_C_GREEN "\033[92m"
#define VE_C_YELLOW "\033[93m"
#define VE_C_BLUE "\033[94m"
#define VE_C_PINK "\033[95m"
#define VE_C_LBLUE "\033[96m"
#define VE_C_WHITE "\033[0m"

inline auto get_time()
{
    auto time_point = std::chrono::system_clock::now();
    const std::time_t t = std::chrono::system_clock::to_time_t(time_point);
    return std::put_time(std::localtime(&t), VE_C_LBLUE "[%H:%M:%S] " VE_C_WHITE);
}

namespace ve
{
    inline std::string to_string(double number, uint32_t precision = 2)
    {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << number;
        return ss.str();
    }
}// namespace ve

#define VE_DEBUG 0
#define VE_INFO 1
#define VE_WARN 2

//#define VE_TO_USER
#define VE_LOGGING
#define VE_LOG_LEVEL VE_INFO
#define VE_CHECKING

#if defined(VE_USER_INPUT)
#define VE_TO_USER(X) std::cout << X
#else
#define VE_TO_USER(X)
#endif

#if defined(VE_LOGGING)

#define VE_CONSOLE_ADD(L, X) \
    if (L >= VE_LOG_LEVEL) std::cout << X << VE_C_WHITE
#define VE_ERR_CONSOLE_ADD(X) std::cerr << X << VE_C_WHITE

#define VE_LOG_CONSOLE(L, X) \
    if (L >= VE_LOG_LEVEL) std::cout << get_time() << X << VE_C_WHITE
#define VE_ERR_CONSOLE(X) std::cerr << get_time() << VE_C_RED << X << VE_C_WHITE

#define VE_THROW(X)                         \
    {                                       \
        VE_ERR_CONSOLE(X << std::endl);     \
        std::stringstream ss;               \
        ss << X << std::endl;               \
        throw std::runtime_error(ss.str()); \
    }
#else
#define VE_CONSOLE_ADD(L, X)
#define VE_ERR_CONSOLE_ADD(L, X)

#define VE_LOG_CONSOLE(L, X)
#define VE_ERR_CONSOLE(X)

#define VE_THROW(X)                         \
    {                                       \
        std::stringstream ss;               \
        ss << X;                            \
        throw std::runtime_error(ss.str()); \
    }
#endif

#if defined(VE_CHECKING)
#define VE_ASSERT(X, M) \
    if (!(X)) VE_THROW(M);
#define VE_CHECK(X, M) vk::resultCheck(X, M)
#else
#define VE_ASSERT(X, M) void(X)
#define VE_CHECK(X, M) void(X)
#endif