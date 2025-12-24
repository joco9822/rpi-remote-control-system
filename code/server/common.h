#ifndef COMMON_H
#define COMMON_H

/* 명령 종류 (Command Type) */
#define CMD_WRITE    1  /* 장치 제어 요청 */
#define CMD_READ     2  /* 센서 값 읽기 요청 */
#define CMD_RESPONSE 3  /* 서버의 응답 */

/* 대상 장치 ID (Device ID) */
#define DEV_LED      10
#define DEV_BUZZER   20
#define DEV_CDS      30
#define DEV_FND      40

/* 동적 라이브러리 사용을 위한 함수 포인터 */
typedef int (*device_init_t)(void);
typedef int* (*device_ctl_t)(int);
typedef int* (*device_read_t)(void);

/* server <-> client 프로토콜 구조체 정의 */
#pragma pack(push, 1)

typedef struct {
    int type;           /* 명령 종류 (CMD_xxx) */
    int deviceid;       /* 대상 장치 ID (DEV_xxx) */
    int data;           /* 제어 값 또는 센서 데이터 */
    char msg[32];       /* 상태 메시지 또는 추가 정보 */
} Protocol;

#pragma pack(pop)

#endif // COMMON_H