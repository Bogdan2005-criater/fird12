#!/bin/bash

# Компиляция HTTP-сервера
gcc http_server.c -o http_server

# Запуск HTTP-сервера (порт 8081)
gnome-terminal -- bash -c "./http_server; exec bash"
echo "HTTP-сервер запущен на порту 8081..."

# Ожидаем несколько секунд, чтобы сервер успел запуститься
sleep 2

# Открытие браузера с HTML-страницей
xdg-open http://localhost:8081

