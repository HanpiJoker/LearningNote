#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFSIZE 1024

void error_handling(char* message);

int main(int argc, const char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[BUFSIZE];
    int str_len = 0, recv_len, recv_cnt;
    /* int idx = 0, read_len = 0; */

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(-1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
        error_handling("socket() error");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect() error!");
    else
        printf("Connected...................\n");

    while (1) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, BUFSIZE, stdin);

        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        str_len = write(sock, message, strlen(message));

        /* 优化原因:
         * 因为TCP协议传输的时候是无边界的，所以存在多次调用write函数传递
         * 的字符串一次性的发送给服务端。这时候会从服务端接收到多个字符串；
         * 此外还有可能由于字符串数据太长，导致操作系统分包发送。这个时候
         * read函数会在没有接收到所有数据的时候进行一次调用！ */
        recv_len = 0;
        while(recv_len < str_len){
            recv_cnt = read(sock, &message[recv_len], BUFSIZE -  1);
            if (recv_cnt == -1) 
                error_handling("read() error");
            recv_len += recv_cnt;
        }
        message[recv_len] = 0;
        /* str_len = read(sock, message, BUFSIZE - 1); */
        /* message[str_len] = 0; */
        printf("Message from server: %s\n", message);
    }

    close(sock);
    return 0;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
