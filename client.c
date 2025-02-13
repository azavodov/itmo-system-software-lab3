#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "settings.h"


int main(int argc, char *argv[]) {
    if (argc < 3) {
        return -1;
    }
    ssize_t bytes_w_r;

    char client_message[MAX_LENGTH], server_message[MAX_LENGTH];
    int tcp_client_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof socket_address);
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(PORT);
    inet_pton(AF_INET, argv[1], &socket_address.sin_addr);
    (void)connect(tcp_client_socket_descriptor, (struct sockaddr *) &socket_address, sizeof socket_address);

    for (int i = 2; i < argc; ++i) {
        memset(client_message, 0, MAX_LENGTH);
        memset(server_message, 0, MAX_LENGTH);
        printf(" --- Path: %s\n", argv[i]);
        strcat(client_message, argv[i]);
        strcat(client_message, "\n");
        bytes_w_r = write(tcp_client_socket_descriptor, client_message, strlen(client_message));
        if (bytes_w_r < 0) {
            printf("client: some problem with server connection...");
        }
        bytes_w_r = read(tcp_client_socket_descriptor, server_message, MAX_LENGTH);
        if (bytes_w_r < 0) {
            printf("client: some problem with server connection...");
        }
        printf("%s", server_message);
    }

    memset(client_message, 0, MAX_LENGTH);
    strcat(client_message, "exit\n");
    bytes_w_r = write(tcp_client_socket_descriptor, client_message, strlen(client_message));
    if (bytes_w_r < 0) {
        printf("client: some problem with server connection...");
    }
    return 0;
}
