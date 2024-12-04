#include "headers.h"
#include "iman.h"

int iman(char* command) {
    int sock;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char request[1024];
    char response[BUFFER_SIZE];
    char* response_buffer = (char*) malloc(BUFFER_SIZE * sizeof(char));
    if (response_buffer == NULL) {
        perror_red("Memory allocation failed");
        return 1;
    }
    int bytes_received;
    const char* sections[] = {"man1", "man2", "man3", "man4", "man5", "man6", "man7", "man8", "man9"};
    int section_found = -1;

    const char *hostname = "man.he.net";
    const int port = 80; 

    for (int i = 0; i < 9; i++) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror_red("Socket creation failed");
            free(response_buffer);  
            return 1;
        }

        server = gethostbyname(hostname);
        if (server == NULL) {
            fprintf(stderr, RED "Error: No such host\n" RESET);
            close(sock);
            free(response_buffer);  
            return 1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror_red("Connection failed");
            close(sock);
            free(response_buffer); 
            return 1;
        }

        snprintf(request, sizeof(request), "GET /%s/%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", sections[i], command, hostname);
        // printf("request: %s\n", request);

        if (send(sock, request, strlen(request), 0) < 0) {
            perror_red("Send failed");
            close(sock);
            free(response_buffer); 
            return 1;
        }

        bytes_received = recv(sock, response, INITIAL_READ_SIZE, 0);
        if (bytes_received > 0) {
            response[bytes_received] = '\0'; 
            if (strstr(response, "DESCRIPTION") != NULL) {
                // printf("Command '%s' is found in section %s\n", command, sections[i]);
                section_found = i;
                break;
            }
        }
 
        close(sock);
    }

    if (section_found == -1) {
        printf("Command '%s' not found in any man section.\n", command);
    } else {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror_red("Socket creation failed");
            free(response_buffer);  
            return 1;
        }

        server = gethostbyname(hostname);
        if (server == NULL) {
            fprintf(stderr, RED "Error: No such host\n" RESET);
            close(sock);
            free(response_buffer);
            return 1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);

        if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror_red("Connection failed");
            close(sock);
            free(response_buffer); 
            return 1;
        }

        snprintf(request, sizeof(request), "GET /%s/%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", sections[section_found], command, hostname);

        // printf("request: %s\n", request);
        if (send(sock, request, strlen(request), 0) < 0) {
            perror_red("Send failed");
            close(sock);
            free(response_buffer);
            return 1;
        }
        int total_bytes_received = 0;

        while ((bytes_received = recv(sock, response, sizeof(response) - 1, 0)) > 0) {
            response[bytes_received] = '\0';
            if (total_bytes_received + bytes_received < BUFFER_SIZE) {
                strncat(response_buffer, response, bytes_received);
                total_bytes_received += bytes_received;
            } else {
                break;
            }
        }

        char* html_start = strstr(response_buffer, "<STRONG>");
        if (html_start != NULL) {
            printf("\n%s\n", html_start);
        } else {
            printf(RED "HTML content not found." RESET "\n");
        }
        close(sock);
    }
    free(response_buffer);

    return 0;
}
