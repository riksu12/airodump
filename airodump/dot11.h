#pragma once

#include <cstdint>
#include "mac.h"

#pragma pack(push, 1)

struct Dot11Hdr {
    uint16_t frame_control;
    uint16_t duration;
    Mac addr1;
    Mac addr2;
    Mac addr3;
    uint16_t seq_ctrl;
};

struct BeaconHdr {
    uint64_t timestamp;
    uint16_t beacon_interval;
    uint16_t cap_info;
};

struct Dot11Tag {
    uint8_t type;
    uint8_t length;
};

#pragma pack(pop)