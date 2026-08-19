### Demo
https://github.com/user-attachments/assets/b17be4e3-fb67-4f5a-bca3-ddac5799fbf8

# 실습 명령어

```bash
# 1. 방해되는 네트워크 관리 프로세스 강제 종료
sudo airmon-ng check kill

# 2. wlan0 인터페이스를 모니터 모드로 전환 (wlan0mon 생성)
sudo airmon-ng start wlan0

# 3. 랜카드(RTL8188FTV 등)가 절전 상태로 빠지는 것을 막기 위해 강제로 깨우기
sudo ip link set wlan0mon up

# 4. 실행
sudo ./airodump wlan0mon
