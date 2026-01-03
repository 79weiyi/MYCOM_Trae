// win32fix.cpp - 解决MinGW与Qt 6链接时缺少__imp___argc和__imp___argv的问题

#include <cstdlib>

// 定义缺失的符号
extern "C" {
    int __argc;
    char **__argv;
    int *__imp___argc = &__argc;
    char ***__imp___argv = &__argv;
}