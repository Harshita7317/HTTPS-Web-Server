#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

// Helper function to send any HTTP request and receive the response
void send_request(int sock,const char*request) {
    char buffer[1024] = {0};  // Buffer to hold the response
    // Send the HTTP request to the server
    send(sock, request, strlen(request), 0);
    // Receive the server's response
    recv(sock, buffer, 1024, 0);
    // Print the server's response
    printf("Server response: \n%s\n", buffer);
}

// Function to send a GET request
void send_get_request(int sock) {
    char *getreq =
        "GET /hello.txt HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";
    printf("Sending GET request\n");
    send_request(sock, getreq);
}

// Function to send a HEAD request
void send_head_request(int sock) {
    char *headreq =
        "HEAD /hello.txt HTTP/1.1\r\n"
        "Host: localhost\r\n\r\n";
    printf("Sending HEAD request\n");
    send_request(sock, headreq);
}

// Function to send a POST request
void send_post_request(int sock) {
    char *postreq =
        "POST /json HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Accept: application/json\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 81\r\n"
        "\r\n"
        "{\n"
        "  \"Company\": \"Qbit Labs\",\n"
        "  \"Customer\": \"Harshita Mishra\",\n"
        "  \"ID\": 12345,\n"
        "  \"Contact\": 000000000\n"
        "}";
    printf("Sending POST request\n");
    send_request(sock, postreq);
}

// Function to send a PUT request
void send_put_request(int sock) {
    char *putreq =
        "PUT /json HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Accept: application/json\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 68\r\n"
        "\r\n"
        "{\n"
        "  \"Customer\": \"Harshita Mishra\",\n"
        "  \"UpdatedField\": \"Email\"\n"
        "}";
    printf("Sending PUT request\n");
    send_request(sock, putreq);
}

void send_delete_request(int sock) {
    const char *delete_request =
        "DELETE /json HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n\r\n";
        
    send_request(sock, delete_request);
}


int main() {
    int sock;
    struct sockaddr_in server_address;

    // Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IP address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Prompt user for the request type
    int choice;
    printf("Choose HTTP request type:\n");
    printf("1. GET\n");
    printf("2. HEAD\n");
    printf("3. POST\n");
    printf("4. PUT\n");
    printf("5. DELETE\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

  //Choose any request
    switch (choice) {
        case 1:
            send_get_request(sock);
            break;
        case 2:
            send_head_request(sock);
            break;
         case 3:
            send_post_request(sock);
            break;
        case 4:
            send_put_request(sock);
            break;
        case 5:
            send_delete_request(sock);
            break;
        default:
            printf("Invalid choice.\n");
    }


    // Close the socket after communication
    close(sock);

    return 0;
}
