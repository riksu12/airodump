#pragma once

#include <iostream>
#include <map>
#include <string>
#include "mac.h"

// AP 정보를 담을 구조체
struct ApInfo {
    int beacons = 0;      // 비콘 프레임 누적 갯수
    int data_count = 0;   // 데이터 프레임 누적 갯수
    int pwr = 0;          // 신호 강도
    std::string essid = ""; // 와이파이 이름
};

// Station 정보를 담을 구조체
struct StationInfo {
    Mac bssid;            // 연결된 AP의 MAC 주소 (연결 안 됐으면 브로드캐스트)
    int pwr = 0;          // 신호 강도
    int frames = 0;       // 주고받은 데이터 프레임 누적 갯수
};

// 화면 출력을 담당할 함수 선언
void print_airodump(const std::map<Mac, ApInfo>& ap_list, const std::map<Mac, StationInfo>& station_list);