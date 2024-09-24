#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8081
#define BUF_SIZE 4096

void serve_html(int client_socket, const char *message) {
    char response[BUF_SIZE];
    char *html_start =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<!DOCTYPE html>"
        "<html lang=\"ru\">"
        "<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Чат</title></head>"
        "<body><h1>Добро пожаловать в чат!</h1>";

    char *html_form =
        "<form action=\"/send\" method=\"GET\">"
        "<label for=\"message\">Введите сообщение:</label><br>"
        "<input type=\"text\" id=\"message\" name=\"message\"><br><br>"
        "<input type=\"submit\" value=\"Отправить\">"
        "</form>";

    char *html_end = "</body></html>";

    if (message) {
        snprintf(response, BUF_SIZE, "%s<p>Ваше сообщение: %s</p>%s%s", html_start, message, html_form, html_end);
    } else {
        snprintf(response, BUF_SIZE, "%s%s%s", html_start, html_form, html_end);
    }

    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}

void handle_request(int client_socket) {
    char buffer[BUF_SIZE];
    read(client_socket, buffer, BUF_SIZE);
    
    char *message_start = strstr(buffer, "message=");
    char message[BUF_SIZE] = {0};

    if (message_start) {
        message_start += 8; // Пропускаем "message="
        char *message_end = strstr(message_start, " ");
        if (message_end) {
            strncpy(message, message_start, message_end - message_start);
        } else {
            strcpy(message, message_start);
        }
    }

    printf("Получено сообщение: %s\n", message);

    serve_html(client_socket, message);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Создаем сокет
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Ошибка сокета");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Привязываем сокет
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Ошибка привязки");
        exit(EXIT_FAILURE);
    }

    // Прослушивание
    if (listen(server_fd, 3) < 0) {
        perror("Ошибка прослушивания");
        exit(EXIT_FAILURE);
    }

    printf("HTTP-сервер запущен на порту %d...\n", PORT);

    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("Ошибка подключения");
            exit(EXIT_FAILURE);
        }

        handle_request(new_socket);
    }

    return 0;
}

