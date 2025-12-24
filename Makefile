# 1. 변수 설정

# 사용할 컴파일러 지정 
CC = gcc
# 컴파일 옵션 및 헤더 파일 경로 지정
CFLAGS = -Wall -I./include
# 링커 옵션. 링크 단계에서 필요한 라이브러리 
LDFLAGS = -lwiringPi -ldl

# 경로 설정 
LIB_DIR = ./lib
SRC_DIR = ./src
BUILD_DIR = ./build

# 빌드 대상 
SERVER_TARGET = $(BUILD_DIR)/server
CLIENT_TARGET = $(BUILD_DIR)/client
LIBS = $(LIB_DIR)/libled.so $(LIB_DIR)/libbuzzer.so $(LIB_DIR)/libcds.so $(LIB_DIR)/libfnd.so

.PHONY: all clean

# 2. all 타겟 
all: $(LIBS) $(SERVER_TARGET) $(CLIENT_TARGET)
# make는 먼저 모든 Dependencies가 있는지 확인한다, 
# 만약 없다면, 해당 타겟을 빌드하기 위한 Rule을 찾아 실행함
# make 한번에 전체 프로젝트를 빌드할 수 있다.

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 3. 그 외 빌드 타겟들

# 3-1. ./lib 디렉토리 내의 소스 코드 -> 공유 라이브러리  
$(LIB_DIR)/lib%.so: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< $(LDFLAGS)

# 3-2. server.c 소스 코드 -> server 실행 파일
$(SERVER_TARGET): $(SRC_DIR)/server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 3-3. client.c 소스 코드 -> client 실행 파일
$(CLIENT_TARGET): $(SRC_DIR)/client.c
	$(CC) $(CFLAGS) -o $@ $<


# 4. clean 타겟 
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(SERVER_TARGET) $(CLIENT_TARGET) $(LIB_DIR)/*.so


# Makefile 기본 문법 
# Target: Dependencies
#     Command 