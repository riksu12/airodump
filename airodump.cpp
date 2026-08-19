#include "airodump.h"
#include <iomanip> // setw 등 화면 칸을 맞추기 위한 라이브러리
#include <cstdlib> // system("clear") 사용

using namespace std;

void print_airodump(const map<Mac, ApInfo>& ap_list, const map<Mac, StationInfo>& station_list) {
    system("clear"); // 갱신을 위한 화면 지우기

    // 상단: AP 목록 출력
    cout << left << setw(20) << "BSSID"
         << right << setw(5) << "PWR"
         << setw(10) << "Beacons"
         << setw(10) << "#Data"
         << "  " << "ESSID" << endl;
    cout << "-----------------------------------------------------------------" << endl;

    for (auto const& [mac, info] : ap_list) {
        cout << left << setw(20) << string(mac)       // BSSID (Key 값)
             << right << setw(5) << info.pwr          // PWR
             << setw(10) << info.beacons              // Beacons
             << setw(10) << info.data_count           // #Data
             << "  " << info.essid << endl;           // ESSID
    }

    cout << "\n";

    // 하단: Station 목록 출력
    cout << left << setw(20) << "BSSID"
         << setw(20) << "STATION"
         << right << setw(5) << "PWR"
         << setw(10) << "Frames" << endl;
    cout << "-----------------------------------------------------------------" << endl;

    for (auto const& [mac, info] : station_list) {
        // bssid가 비어있거나 특정 값이면 (not associated)로 출력
        string bssid_str = info.bssid.isBroadcast() || info.bssid.isNull() ? "(not associated)" : string(info.bssid);

        cout << left << setw(20) << bssid_str         // 연결된 AP 주소
             << setw(20) << string(mac)               // Station MAC (Key 값)
             << right << setw(5) << info.pwr          // PWR
             << setw(10) << info.frames << endl;      // Frames
    }
}