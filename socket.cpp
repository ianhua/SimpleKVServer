#include "socket.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <errno.h>

/**
 * Socket
 * @author ianhua
 */

namespace mmtraining {

/************************ClientSocket***************************/

ClientSocket::ClientSocket() : fd(-1) {}
    
ClientSocket::ClientSocket(int fd) {
    this->fd = fd;
}

ClientSocket::~ClientSocket() {
    Close();
}
    
int ClientSocket::Connect(const char* ip, unsigned short port) {
    // socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Client: socket error!" << std::endl;
        return -1;
    }

    // Set server socket address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // Convert the byte order from host to network 
    server_addr.sin_port = htons(port);
    // Convert ip string to in_addr
    inet_pton(AF_INET, ip, &(server_addr.sin_addr));

    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN);

    std::cout << "Client connecting: " << buf << " : " << ntohs(server_addr.sin_port) << std::endl;

    // connect
    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "Client: connect error!" << std::endl;
        return -1;
    }

    std::cout << "Client connected!" << std::endl;

    return 0;
}
    
int ClientSocket::Write(const void* buffer, int bufferSize) {
    int nleft = bufferSize;
    int nwritten;
    char* bufp = (char*)buffer;

    // Robust write
    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) == -1) {
            return -1;
        } 
        nleft -= nwritten;
        bufp += nwritten;
    }
    
    return bufferSize;
}

int ClientSocket::WriteLine(const std::string& line) {
    const char* buffer = line.c_str();
    int nwritten;
    int bufferSize = line.length();
    if ((nwritten = Write(buffer, bufferSize)) == -1) {
        return -1;
    } 
    return nwritten;
}

int ClientSocket::Read(void* buffer, int bufferSize) {
    int nleft = bufferSize;
    int nread;
    char* bufp = (char*)buffer;

    // Robust read
    while (nleft > 0) {
        if ((nread = read(fd, buffer, bufferSize)) == -1) {
            return -1;
        }  else if (nread == 0) {
            break;
        }
        nleft -= nread;
        bufp += nread;
    }
    
    return (bufferSize - nleft);
}

int ClientSocket::ReadLine(std::string& line) {
    int n, nread;
    int maxlen = 1024;
    char c;
    char buffer[maxlen];
    char* bufp = buffer;

    for (n = 1; n < maxlen; n++) {
        if ((nread = Read(&c, 1)) == 1) {
            //std::cout << "Read:" << c << std::endl;
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        } else if (nread == 0) {
            if (n == 1) {
                return 0;
            } else {
                break;
            }
        } else {
            return -1;
        }
    }
    // Null-terminated
    *bufp = 0;
    line.assign(buffer);
    return n - 1;
}

int ClientSocket::Close() {
    if (close(fd) != 0) {
        return -1;
    }
    return 0;
}

/************************ServerSocket***************************/

ServerSocket::ServerSocket() : fd(-1) {}

ServerSocket::~ServerSocket() {
    Close();
}

int ServerSocket::Listen(const char* ip, unsigned short port) {
    // socket
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cout << "Server: socket error!" << std::endl;
        return -1;
    }

    // Eliminates "Address already in use" error from bind
    int optval = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    // Set server socket address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    // Convert the byte order from host to network 
    server_addr.sin_port = htons(port);
    // Convert ip string to in_addr
    inet_pton(AF_INET, ip, &(server_addr.sin_addr));

    // bind
    if (bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cout << "Server: bind error!" << std::endl;
        return -1;
    } 

    // listen
    if (listen(fd, 10) == -1) {
        std::cout << "Server: listen error!" << std::endl;
        return -1;
    }

    return 0;
}

ClientSocket* ServerSocket::Accept() {
    struct sockaddr_in client_addr;

    // accept
    int connect_fd;
    int client_addr_len = sizeof(client_addr);

    std::cout << "Server accepting..." << std::endl;

    if ((connect_fd = accept(fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len)) == -1) {
        std::cout << "Server: accpet error!" << std::endl;
        return NULL;
    }

    char buf[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, buf, INET_ADDRSTRLEN);

    std::cout << "Server accepted: " << buf << " : " << ntohs(client_addr.sin_port) << std::endl;

    ClientSocket* client = new ClientSocket(connect_fd);
    return client;
}

int ServerSocket::Close() {
    if (close(fd) != 0) {
        return -1;
    }
    return 0;
}

} // namespace mmtraining
