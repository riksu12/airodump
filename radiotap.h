#pragma once

#include <cstdint>

#pragma pack(push, 1)
struct RadiotapHdr {
    uint8_t it_version;
    uint8_t it_pad;
    uint16_t it_len;
    uint32_t it_present;
};
#pragma pack(pop)