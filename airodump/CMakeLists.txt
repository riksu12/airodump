cmake_minimum_required(VERSION 3.5)

project(airodump LANGUAGES CXX)

# C++ 17 표준 사용 (mac.cpp 등의 문법 지원을 위해 필요)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 컴파일할 소스 코드 목록
set(SRCS
    main.cpp
    airodump.cpp
    mac.cpp
)

# 2. 프로젝트 트리에 보여질 헤더 파일 목록
set(HEADERS
    airodump.h
    dot11.h
    radiotap.h
    mac.h
)

# 3. 실행 파일(airodump) 생성
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

# 4. pcap 라이브러리 링크 (가장 중요한 부분!)
target_link_libraries(${PROJECT_NAME} pcap)