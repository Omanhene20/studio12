// server.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 128

int main() {
    int server_sock, client_sock;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    unlink(SOCKET_PATH);

    if (bind(server_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening...\n");

    while (1) {
        client_sock = accept(server_sock, NULL, NULL);
        if (client_sock == -1) {
            perror("accept");
            continue;
        }

        printf("New connection established.\n");

        ssize_t num_read = read(client_sock, buffer, BUFFER_SIZE);
        if (num_read > 0) {
            buffer[num_read] = '\0';
            printf("Received: %s", buffer);

            if (strncmp(buffer, "quit", 4) == 0) {
                printf("Shutdown signal received.\n");
                close(client_sock);
                break;
            }
        }

        close(client_sock);
    }

    close(server_sock);
    unlink(SOCKET_PATH);
    return 0;
}
