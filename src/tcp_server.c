// 에코 서버 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define TCP_PORT 5100 				/* 서버의 포트 번호 */  // 클라이언트 포트 번호는 recvfrom() 함수에서 알아서 찾아서 서버 -> 클라이언트로 패킷 전송함 

int main(int argc, char **argv)
{
    int ssock; 					/* 소켓 디스크립트 정의 */
    socklen_t clen;
    int n;
    struct sockaddr_in servaddr, cliaddr; 	/* 주소 구조체 정의 */
    char mesg[BUFSIZ];

    /* 서버 소켓 생성 */
    if((ssock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {   // socket() 함수로 소켓 생성, SOCK_STREAM = TCP 
        perror("socket()");
        return -1;
    }

    /* 주소 구조체에 주소 지정 */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  // 도메인: IPv4 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 모든 IP 주소로부터의 접속 허용 (+ 서버의 IP 주소를 자동으로 인식)
    servaddr.sin_port = htons(TCP_PORT); 	/* 사용할 포트 지정 */

    /* bind 함수를 사용하여 서버 소켓의 주소 설정 */
    if(bind(ssock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {  // 운영체제에 서비스 등록  // 인자로 서버 자신의 IP 주소와 포트 번호를 전달 (근데, INADDR_ANY을 사용하면 서버 IP 주소를 자동으로 채워준다고 함.)
        perror("bind()");
        return -1;
    }

    /* 동시에 접속하는 클라이언트의 처리를 위한 대기 큐를 설정 */
    if(listen(ssock, 8) < 0) {  // 8은 대기 큐의 크기 
        perror("listen()");   // TCP니까 listen 
        return -1;
    }

    clen = sizeof(cliaddr);
    do {
        /* 클라이언트가 접속하면 접속을 허용하고 클라이언트 소켓 생성 */
        int n, csock = accept(ssock, (struct sockaddr *)&cliaddr, &clen);  // 클라이언트 접속 대기 (cliaddr에 클라이언트 주소 저장)
        // csock은 cliaddr와 맵핑 (그때그때 cliaddr를 받아서 소켓을 생성함)

        /* 네트워크 주소를 문자열로 변경 */
        inet_ntop(AF_INET, &cliaddr.sin_addr, mesg, BUFSIZ);
        printf("Client is connected : %s\n", mesg);
        if((n = read(csock, mesg, BUFSIZ)) <= 0)   // TCP니까 read  // 클라이언트로부터 데이터 읽기
            perror("read()");
        printf("Received data : %s", mesg);

        /* 클라이언트로 buf에 있는 문자열 전송 */ 
        if(write(csock, mesg, n) <= 0)    // TCP니까 write 
            perror("write()");
        close(csock); 			/* 클라이언트 소켓을 닫음 */
    } while(strncmp(mesg, "q", 1));

    close(ssock); 			/* 서버 소켓을 닫음 */

    return 0;
}
