#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define OPSZ 4
#define RLT_SIZE 4

void error_handling(char* message);
int operator_jundge(char operator);

int main(int argc, const char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;

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
        printf("Connected..........\n");
    
    fputs("Operand Count: ", stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0] = (char)opnd_cnt;
    for( i = 0; i < opnd_cnt; i++){
        printf("Operand %d: ", i);
        scanf("%d", (int *)&opmsg[i*OPSZ+1]);
    }

    fgetc(stdin);
    fputs("Operator: ", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    while(operator_jundge(opmsg[opnd_cnt*OPSZ+1]) != 1){
        fputs("Input can't be identified\n", stdout);
        fputs("Operator: ", stdout);
        scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);
    }

    write(sock, opmsg, opnd_cnt*OPSZ +2);
    read(sock, &result, RLT_SIZE);
    printf("Operation result: %d\n", result);

    close(sock);
    return 0;
}

int operator_jundge(char operator)
{
    if ((operator!= '+') && (operator!= '-') && (operator!= '*') && (operator!= '/'))
        return 0;
    else
        return 1;
}

void error_handling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
