#include <iostream>
#include <pcap.h>
#include <map>
#include <cstring>
#include "mac.h"
#include "radiotap.h"
#include "dot11.h"
#include "airodump.h"

using namespace std;

void usage() {
    cout << "syntax : airodump <interface>" << endl;
    cout << "sample : airodump mon0" << endl;
}

int main(int argc, char* argv[]) { // 인터페이스(인자) 전달했는지 검증
    if (argc != 2) {
        usage();
        return -1;
    }

    char* dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];

    // pcap 라이브러리 써서 무선 랜카드 열기
    pcap_t* pcap = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (pcap == NULL) {
        cerr << "pcap_open_live(" << dev << ") return null - " << errbuf << endl;
        return -1;
    }

    // 수집한 데이터 리스트 선언
    map<Mac, ApInfo> ap_list;
    map<Mac, StationInfo> station_list;

    int update_count = 0; // 화면 깜빡이 조절

    // 패킷 계속 잡기 start
    while (true) {
        struct pcap_pkthdr* header;
        const u_char* packet;
        int res = pcap_next_ex(pcap, &header, &packet);
        if (res == 0) continue; // 패킷 수신 없음 -> Timeout 종료
        if (res == PCAP_ERROR || res == PCAP_ERROR_BREAK) break;

        // 1) Radiotap 헤더 까기 + PWR 찾기
        RadiotapHdr* radiotap = (RadiotapHdr*)packet;

        int8_t pwr = 0; // 신호 강도 변수(음수니까 int8_t)

        // 5번째 비트(Antenna signal)가 1인지 비트 연산(&)으로 확인
        // Radiotap은 길이가 제각각이라 it_present보고 5번째 비트(=Antenna Signal)가 있는지 확인해야 함
        if (radiotap->it_present & (1 << 5)) {
            int offset = 8; // 기본 헤더 8바이트부터 시작 (version 1 + pad 1 + len 2 + present 4)

            // PWR 데이터 앞쪽에 다른 데이터들이 있다면 그 크기만큼 건너뜀(offset 증가)
            if (radiotap->it_present & (1 << 0)) offset += 8; // TSFT (8바이트)
            if (radiotap->it_present & (1 << 1)) offset += 1; // Flags (1바이트)
            if (radiotap->it_present & (1 << 2)) offset += 1; // Rate (1바이트)
            if (radiotap->it_present & (1 << 3)) { // Channel (4바이트)
                if (offset % 2 != 0) offset++; // 짝수 바이트 정렬
                offset += 4;
            }
            if (radiotap->it_present & (1 << 4)) { // FHSS (2바이트)
                if (offset % 2 != 0) offset++;
                offset += 2;
            }

            // 최종적으로 도착한 offset 위치의 1바이트가 PWR
            pwr = *(int8_t*)(packet + offset);
        }

        // 802.11 분석
        Dot11Hdr* dot11 = (Dot11Hdr*)(packet + radiotap->it_len); //Radiotap 전체 길이(it_len)만큼 건너뛰면 와이파이 헤더
        uint8_t type_subtype = dot11->frame_control & 0xFF; // 비콘인지 데이터인지 -> 프레임 종류 식별

        if (type_subtype == 0x80) { //AP 방송용 비콘 프레임일 경우
            Mac bssid = dot11->addr3; //비콘에서 3번째 주소가 BSSID
            ap_list[bssid].beacons++; //리스트에 추가
            ap_list[bssid].pwr = pwr; //강도 기록

            // ESSID는 가변 태그 영역으로 들어가서, 802.11 헤더(24) + 비콘 고정 정보(12) = 36바이트 건너뛰어서 읽음
            const u_char* tag_pos = (const u_char*)(dot11 + 1) + sizeof(BeaconHdr);
            Dot11Tag* tag = (Dot11Tag*)tag_pos;

            // 태그 번호 = 0(ESSID) + 리스트에 없을 경우 추가
            if (tag->type == 0 && tag->length > 0 && ap_list[bssid].essid.empty()) {
                char essid_buf[33] = {0,};
                memcpy(essid_buf, tag_pos + 2, tag->length); //태그 종류 (1) + 길이 (1) 건너뛰고 문자열만 저장
                ap_list[bssid].essid = string(essid_buf);
            }
        }
        else if ((type_subtype & 0x0C) == 0x08) { //Data 프레임인 경우
            Mac bssid = dot11->addr2; //보통 2번 주소를 AP(송신자)로 가정
            Mac station = dot11->addr1; // 보통 1번 주소를 Station(수신자)로 가정

            // 스위치 장비나 브로드캐스트 주소는 패스
            if (station.isBroadcast() || station.isMulticast()) continue;

            // 리스트에 기록
            ap_list[bssid].data_count++; // AP의 데이터 통신량 증가
            station_list[station].frames++; //Station의 통신량 증가
            station_list[station].bssid = bssid; // 연결정보 갱신
            station_list[station].pwr = pwr; //신호 강도 갱신
        }

        update_count++;
        if (update_count % 10 == 0) { //패킷 10개 잡을 때마다 한 번씩 지우고 다시
            print_airodump(ap_list, station_list);
        }
    }

    pcap_close(pcap);
    return 0;
}