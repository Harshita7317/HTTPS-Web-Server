#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

char *parse_request(char *request, char *request_method, char *request_header,
                    char *request_body) {
    // method
    const char *method_begin = request;
    const char *method_end = strchr(request, ' ');

    strncpy(request_method, method_begin, method_end - method_begin);
    request_method[method_end - method_begin] = '\0';

    // uri
    const char *uri_begin = method_end + 1;
    const char *uri_end = strchr(uri_begin, ' ');
    char *path = malloc((uri_end - uri_begin) + 2);
    memset(path, 0, uri_end - uri_begin + 2);
    path[0] = '.';
    strncat(path, uri_begin, uri_end - uri_begin);
    path[uri_end - uri_begin + 1] = '\0';

    // header
    const char *header_begin = strstr(uri_end, "\r\n") + 2;
    const char *header_end = strstr(header_begin, "\r\n\r\n");
    strncpy(request_header, header_begin, header_end - header_begin);
    request_header[header_end - header_begin] = '\0';

    // body
    const char *body = header_end + 4;
    strncpy(request_body, body, strlen(body));

    return path;
}

void build_response(char *request, char *response) {
    ///////////////////////////////////////////////
    /// TODO : Parse Request and build response ///
    ///////////////////////////////////////////////

    char request_method[8] = {0};
    char request_header[1024] = {0};
    char request_body[BUFFER_SIZE] = {0};
    char *path =
        parse_request(request, request_method, request_header, request_body);

    /////////////////////
    // request headers //
    /////////////////////

    printf("%s %s\n", request_method, path);
    printf("%s\n", request_header);
    printf("%s\n\n", request_body);

    /////////////////////
    // request methods //
    /////////////////////

    // GET
    if (strcmp(request_method, "GET") == 0) {
        FILE *file_fd = fopen(path, "r");
        if (file_fd == NULL) {
            snprintf(response, BUFFER_SIZE,
                     "HTTP/1.1 404 Not Found\r\n"
                     "Server: localhost:8080\r\n"
                     "Content-Type: plain/text\r\n"
                     "\r\n"
                     "404 Not Found");
            return;
        }

        fseek(file_fd, 0L, SEEK_END);
        size_t file_size = ftell(file_fd);
        rewind(file_fd);
        char file_content[BUFFER_SIZE] = {0};
        fgets(file_content, BUFFER_SIZE, file_fd);
        fclose(file_fd);
        free(path);

        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Server: localhost:8080\r\n"
                 "Content-Type: plain/text\r\n"
                 "Content-Length: %lu\r\n"
                 "\r\n",
                 file_size);
        strcat(response, file_content);
    }

    // HEAD
    else if (strcmp(request_method, "HEAD") == 0) {
        FILE *file_fd = fopen(path, "r");
        if (file_fd == NULL) {
            snprintf(response, BUFFER_SIZE,
                     "HTTP/1.1 404 Not Found\r\n"
                     "Server: localhost:8080\r\n"
                     "Content-Type: plain/text\r\n"
                     "\r\n");
            return;
        }

        fseek(file_fd, 0L, SEEK_END);
        size_t file_size = ftell(file_fd);
        fclose(file_fd);
        free(path);

        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Server: localhost:8080\r\n"
                 "Content-Type: plain/text\r\n"
                 "Content-Length: %lu\r\n"
                 "\r\n",
                 file_size);
    }

    // POST
    else if (strcmp(request_method, "POST") == 0) {
        FILE *file_fd = fopen(path, "w");
        fprintf(file_fd, "%s", request_body);
        fclose(file_fd);
        free(path);

        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Server: localhost:8080\r\n"
                 "Content-Type: plain/text\r\n"
                 "\r\n"
                 "Request processed successfully!");
    }

    // PUT
    else if (strcmp(request_method, "PUT") == 0) {
        FILE *file_fd = fopen(path, "w");
        fprintf(file_fd, "%s", request_body);
        fclose(file_fd);
        free(path);

        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 200 OK\r\n"
                 "Server: localhost:8080\r\n"
                 "Content-Type: plain/text\r\n"
                 "\r\n"
                 "Request processed successfully!");
    }

    // DELETE
    else if (strcmp(request_method, "DELETE") == 0) {
        remove(path);
        free(path);

        snprintf(response, BUFFER_SIZE,
                 "HTTP/1.1 201 Created\r\n"
                 "Server: localhost:8080\r\n"
                 "\r\n"
                 "Request processed !!");
        return;
    }
}

void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE] = {0};
    char response[BUFFER_SIZE] = {0};

    // receive request
    recv(client_fd, buffer, BUFFER_SIZE, 0);

    // for debug
    // puts(buffer);

    build_response(buffer, response);
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
}

int main(int argc, char *argv[]) {
    int server_sock;
    int opt = 1;

    struct sockaddr_in server_addr = {.sin_addr = AF_INET,
                                      .sin_addr.s_addr = INADDR_ANY,
                                      .sin_port = htons(PORT)};

    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
        0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(server_sock, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 1) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    puts("waiting for connection ...\n");

    while (1) {
        int client_sock;
        struct sockaddr_in client_addr;
        int client_addrlen = sizeof(client_addr);

        if ((client_sock = accept(server_sock, (struct sockaddr *)&client_addr,
                                  (socklen_t *)&client_addrlen)) < 0) {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        handle_client(client_sock);
    }

    return EXIT_SUCCESS;
}
