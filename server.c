#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include "settings.h"

int tcp_server_socket_descriptor;

void *client_request(void *p_client_socket_descriptor) {
    int client_socket_descriptor = *((int *) p_client_socket_descriptor);
    free(p_client_socket_descriptor);
    uint8_t client_message[MAX_LENGTH + 1], server_message[MAX_LENGTH + 1];
    memset(client_message, 0, MAX_LENGTH);
    memset(server_message, 0, MAX_LENGTH);
    int message_length;
    while (1) {
        memset(client_message, 0, MAX_LENGTH);
        while ((message_length = read(client_socket_descriptor, client_message, MAX_LENGTH)) > 0) {

            if (client_message[message_length - 1] == '\n') break;
        }
        client_message[message_length - 1] = 0;
        if (message_length - 2 >= 0 && client_message[message_length - 2] == '\r')
            client_message[message_length - 2] = 0;
        if (strcmp((const char *) client_message, "exit") == 0) {
            close(client_socket_descriptor);
            return NULL;
        }
        printf("server: client request: %s\n", client_message);
        struct dirent *file;
        DIR *directory = opendir((const char *) client_message);
        if (!directory) {
            char *s = "server: path does not exist\n";
            write(client_socket_descriptor, s, strlen(s));
        } else {
            memset(server_message, 0, MAX_LENGTH);
            while ((file = readdir(directory)) != NULL) {
                strcat((char *) server_message, file->d_name);
                strcat((char *) server_message, "\n");
            }
            closedir(directory);
            write(client_socket_descriptor, (char *) server_message, strlen((char *) server_message));
        }
    }
}


int main() {
    struct sockaddr_in socket_address;
    memset(&socket_address, 0, sizeof socket_address);
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(PORT);
    socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

    tcp_server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    bind(tcp_server_socket_descriptor, (struct sockaddr *) &socket_address, sizeof(socket_address));
    listen(tcp_server_socket_descriptor, QUEUE_LIMIT);

    printf("server: waiting for connections ...\n");
    while (1) {
        int *tcp_client_socket_descriptor = malloc(sizeof(int));
        *tcp_client_socket_descriptor = accept(tcp_server_socket_descriptor, (struct sockaddr *) NULL, NULL);
        pthread_t connection;
        pthread_create(&connection, NULL, client_request, tcp_client_socket_descriptor);
    }

}
