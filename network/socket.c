#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SOCK_NONBLOCK 0
/*
 * CLIENT FLOW
 * -------------     ------------------    ----------------------
 * \ create sock \-> \ connect to port \ ->\ write to connection \
 * --------------    ------------------    ---------------------- 
 *                                              ^      \
 *                                              \      v
 *                                          ---------------------
 *                                         \ read from connection \ 
 *                                          ---------------------- 
 * */
void *client(void *arg) {
    /*
     * Creates an unnamed socket inside kernel.
     * Parameters:
     *     Initialize an IPV4 Socket,
     *     Utilizing the TCP protocol (connection-oriented)
     *     Non-blocking I/O
     * returns a file descriptor
     * */
    int sockfd = socket(AF_INET, SOCK_STREAM, SOCK_NONBLOCK);

    /*
     * Specifies address family and port of an endpoint
     * */
    const char *localhost = "127.0.0.1";
    struct sockaddr_in host_addr;


    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(3000);
    /*
     * parses the string localhost into a sequence of bytes
     * "127.0.0.1" -> 7F 00 00 01
     * */
    inet_pton(AF_INET, localhost, &host_addr.sin_addr.s_addr);
    connect(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr));
    char recv_buf[1024];
    char send_buf[1024];
    ssize_t n = 0;
    while( (n = recv(sockfd, recv_buf, sizeof(recv_buf), 0)) > 0 ) {
        if(n == 1) {
            printf("client closing connection ...\n");
            close(sockfd);
            break;
        }
        if(n > 0) {
            printf("client received: %s", recv_buf);
            strcpy(send_buf, "hello from client\n");
            send(sockfd, send_buf, strlen(send_buf), 0);
        }
    }
    return NULL;
}

/*
 * SERVER FLOW
 *
 *   -----      -------       --------     -------  -> -------
 * \ bind \ -> \ listen \ -> \ accept \ -> \ read \   \ write \
 *   -----      -------       --------      ------ <-  -------
 *                                             ^
 *                                             \
 *                                            -----
 *                                           \ close \ (sent from client
 *                                           -------- 
 * */
void *server(void *arg) {
    int sockfd = socket(AF_INET, SOCK_STREAM, SOCK_NONBLOCK);

    const char *localhost = "127.0.0.1";
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(3000);
    inet_pton(AF_INET, localhost, &serv_addr.sin_addr.s_addr);

    char recv_buf[1024];
    char send_buf[1024];

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 1);
    /*
     * Parameters:
     *  listenning socket
     *  client's address
     *  i/o of client's address
     *
     *  Passing NULLs indicates we don't care who connected
     * */
    accept(sockfd, (struct sockaddr *)NULL, NULL);
    int count = 10;
    ssize_t n = 0;
    while(1) {
        
        n = recv(sockfd, recv_buf, sizeof(recv_buf), 0);
        printf("server received: %s", recv_buf);
        if(count > 0) {
            strcpy(send_buf, "hello client\n");
            send(sockfd, send_buf, strlen(send_buf), 0);
            count--;

        } else {
            memset(send_buf, 0, sizeof(send_buf));
            send_buf[0] = 1;
            send(sockfd, send_buf, 1, 0);
            printf("server closing connection ...");
            close(sockfd);
            break;
        }
    }
    return NULL; 
}

int main() {
    pthread_t thread_server;
    pthread_t thread_client;
   if (pthread_create(&thread_server, NULL, server, NULL) != 0) {
        perror("pthread_create server");
        return 1;
    }
    if (pthread_create(&thread_client, NULL, client, NULL) != 0) {
        perror("pthread_create client");
        return 1;
    }


    pthread_join(thread_server, NULL);
    pthread_join(thread_client, NULL);
}
