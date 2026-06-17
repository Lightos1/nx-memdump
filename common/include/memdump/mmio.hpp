#pragma once
#include <string>
#include <switch.h>

struct MmioInfo {
    char name[100] = {};
    u32 offset;
};

struct MmioTableEntry {
    char name[100] = {};
    u64 address;
    size_t size;
    MmioInfo *info  = nullptr;
    size_t infoSize = 0;
};
