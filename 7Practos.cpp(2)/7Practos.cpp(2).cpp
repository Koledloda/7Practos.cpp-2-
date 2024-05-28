﻿#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; // Объявление структуры WSADATA
    ADDRINFO hints; // Объявление структуры ADDRINFO
    ADDRINFO* addrResult; // Объявление указателя на структуру ADDRINFO
    SOCKET ListenSocket = INVALID_SOCKET; // Объявление переменной ListenSocket и присвоение ей INVALID_SOCKET
    SOCKET ConnectSocket = INVALID_SOCKET; // Объявление переменной ConnectSocket и присвоение ей INVALID_SOCKET
    char recvBuffer[512]; // Массив recvBuffer размером 512

    const char* sendBuffer = "Hello from server"; // Объявление указателя sendBuffer на строку "hello from server"

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); // Инициализация библиотеки Windows Sockets
    if (result != 0) { // Проверка на ошибку
        cout << "WSAStartup failed with error: " << result << endl; // Вывод сообщения об ошибке
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); // Обнуление памяти для hints
    hints.ai_family = AF_INET; // Семейство адресов (IPv4)
    hints.ai_socktype = SOCK_STREAM; // Тип сокета (потоковый)
    hints.ai_protocol = IPPROTO_TCP; // Протокол (TCP)
    hints.ai_flags = AI_PASSIVE; // Пассивный режим

    result = getaddrinfo(NULL, "666", &hints, &addrResult); // Получение информации об адресе для прослушивания
    if (result != 0) { // Проверка на ошибку
        cout << "getaddrinfo failed with error: " << result << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrResult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrresult->ai_protocol); // Создание сокета
    if (ListenSocket == INVALID_SOCKET) { // Проверка на ошибку при создании сокета
        cout << "socket creation failed" << endl; // Вывод сообщения об ошибке
        freeaddrinfo(addrresult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    result = bind(ListenSocket, addrresult->ai_addr, (int)addrresult->ai_addrlen); // Привязка сокета к адресу
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "bind failed with error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrresult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); // Перевод сокета в режим прослушивания
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "listen failed with error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrresult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    ConnectSocket = accept(ListenSocket, NULL, NULL); // Принятие входящего подключения
    if (ConnectSocket == INVALID_SOCKET) { // Проверка на ошибку
        cout << "accept failed with error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
        closesocket(ListenSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    closesocket(ListenSocket); // Закрытие слушающего сокета

    do {
        ZeroMemory(recvBuffer, 512); // Обнуление буфера
        result = recv(ConnectSocket, recvBuffer, 512, 0); // Прием данных
        if (result > 0) {
            cout << "received " << result << " bytes" << endl; // Вывод количества принятых байт
            cout << "received data: " << recvBuffer << endl; // Вывод принятых данных

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0); // Отправка данных
            if (result == SOCKET_ERROR) { // Проверка на ошибку при отправке
                cout << "send failed with error: " << result << endl; // Вывод сообщения об ошибке
                closesocket(ConnectSocket); // Закрытие сокета
                freeaddrinfo(addrResult); // Освобождение памяти
                WSACleanup(); // Завершение работы с библиотекой
                return 1;
            }
        }
        else if (result == 0) { // Проверка если соединение закрылось
            cout << "connection closing" << endl; // Вывод сообщения о закрытии соединения
        }
        else { // Если есть ошибка при приеме данных
            cout << "recv failed with error: " << WSAGetLastError() << endl; // Вывод сообщения об ошибке
            closesocket(ConnectSocket); // Закрытие сокета
            freeaddrinfo(addrresult); // Освобождение памяти
            WSACleanup(); // Завершение работы с библиотекой
            return 1;
        }
    } while (result > 0); // Повторять до тех пор, пока есть данные для приема

    result = shutdown(ConnectSocket, SD_SEND); // Отключение сокета от отправки
    if (result == SOCKET_ERROR) { // Проверка на ошибку
        cout << "shutdown failed with error: " << result << endl; // Вывод сообщения об ошибке
        closesocket(ConnectSocket); // Закрытие сокета
        freeaddrinfo(addrResult); // Освобождение памяти
        WSACleanup(); // Завершение работы с библиотекой
        return 1;
    }

    closesocket(ConnectSocket); // Закрытие подключенного сокета
    freeaddrinfo(addrResult); // Освобождение памяти
    WSACleanup(); // Завершение работы с библиотекой
    return 0;
}