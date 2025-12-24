# 1. 변수 설정

# 사용할 컴파일러 지정 
CC = gcc
# 컴파일 옵션 및 헤더 파일 경로 지정
CFLAGS = -Wall -I./code/server
# 링커 옵션. 링크 단계에서 필요한 라이브러리 
LDFLAGS = -lwiringPi -ldl

# 경로 설정 
SERVER_DIR = code/server
CLIENT_DIR = code/client
EXEC_DIR = exec
LIB_EXEC_DIR = $(EXEC_DIR)/lib

# 빌드 대상 
SERVER_TARGET = $(EXEC_DIR)/server
CLIENT_TARGET = $(EXEC_DIR)/client
DEVICE_LIBS = $(LIB_EXEC_DIR)/libled.so $(LIB_EXEC_DIR)/libbuzzer.so \
              $(LIB_EXEC_DIR)/libcds.so $(LIB_EXEC_DIR)/libfnd.so

.PHONY: all clean directories

# 2. all 타겟 
all: directories $(DEVICE_LIBS) $(SERVER_TARGET) $(CLIENT_TARGET)
# make는 먼저 모든 Dependencies가 있는지 확인한다, 
# 만약 없다면, 해당 타겟을 빌드하기 위한 Rule을 찾아 실행함
# make 한번에 전체 프로젝트를 빌드할 수 있다.

directories:
	mkdir -p $(LIB_EXEC_DIR)

# 3. 그 외 빌드 타겟들

# 3-1. 공유 라이브러리 빌드 (code/server/*.c -> exec/lib/*.so)
$(LIB_EXEC_DIR)/lib%.so: $(SERVER_DIR)/%.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $< $(LDFLAGS)

# 3-2. 서버 빌드 (code/server/server.c -> exec/server)
$(SERVER_TARGET): $(SERVER_DIR)/server.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# 3-3. 클라이언트 빌드 (code/client/client.c -> exec/client)
$(CLIENT_TARGET): $(CLIENT_DIR)/client.c
	$(CC) $(CFLAGS) -o $@ $<


# 4. clean 타겟 
clean:
	rm -rf $(EXEC_DIR)


# Makefile 기본 문법 
# Target: Dependencies
#     Command 