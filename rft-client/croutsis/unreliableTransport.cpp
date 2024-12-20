//
// Created by Phillip Romig on 7/15/24.
//

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include "unreliableTransport.h"
#include "logging.h"


// Constructor for the client
// hostname: the name of the server
// portNum: the port number to connect to
unreliableTransportC::unreliableTransportC(const std::string &hostname, const uint16_t &portNum) {
    TRACE << "Creating a unreliableTransportC object with flavor client." << std::endl;

    // Create a UDP socketFd
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd == -1) {
        FATAL << "Error creating socketFd, throwing error #" << errno << std::endl;
        throw(std::system_error(std::make_error_code(static_cast<std::errc>(errno)), strerror(errno)));
    }


    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);

    struct addrinfo hints{};
    struct addrinfo *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int status = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (status != 0) {
        FATAL << "Error looking up IPv4 Address for " << hostname << "error #" << status << std::endl;
        throw(std::runtime_error(gai_strerror(status)));
    }

    if (result == nullptr) {
        FATAL << "result == nullptr. This should be impossible"    << std::endl;
        throw(std::runtime_error("result == nullptr. This should be impossible"));
    }
    DEBUG << "Found IPv4 Address for " << hostname << " as " << inet_ntoa(((struct sockaddr_in *)result->ai_addr)->sin_addr) << std::endl;
    DEBUG << "Length of result->ai_addr->sa_data: " << result->ai_addrlen << std::endl;

    serverAddr.sin_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr;
    DEBUG << "Found IPv4 Address for " << hostname << " as " << inet_ntoa(serverAddr.sin_addr) << std::endl;

    freeaddrinfo(result);
}


void unreliableTransportC::udt_send(const datagramS &data) const {


    DEBUG << "Sending datagram to " << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << std::endl;
    TRACE << "Sending: " << toString(data) << std::endl;
    ssize_t bytesSent = sendto(socketFd, &data, sizeof(datagramS), 0,
                               (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    if (bytesSent == -1) {
        FATAL << "Error sending datagram." << std::endl;
        close(socketFd);
        throw(std::system_error(std::make_error_code(static_cast<std::errc>(errno)), strerror(errno)));
    }
    DEBUG << "Successfully sent " << bytesSent << " bytes." << std::endl;
}

ssize_t unreliableTransportC::udt_receive(const datagramS &data) const  {
    TRACE << "In udt_receive()" << std::endl;
    ssize_t bytesRead = 0;

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(socketFd, &read_fds);
    struct timeval timeout{};
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;


    // Set a timeout of 0 seconds (non-blocking)
    if (select(socketFd+1, &read_fds, nullptr, nullptr, &timeout) < 0) {
        FATAL << "Error in select." << std::endl;
        throw (std::system_error(std::make_error_code(static_cast<std::errc>(errno)), strerror(errno)));
    }

    if (FD_ISSET(socketFd,&read_fds)) {
        TRACE << "Data is available to read." << std::endl;
        bytesRead = recvfrom(socketFd, (void *) &data, sizeof(datagramS), 0, nullptr, nullptr);
        TRACE << "Received " << bytesRead << " bytes." << std::endl;
        if (bytesRead == -1) {
            FATAL << "Error when calling recvfrom() throwing error #" << errno << std::endl;
            close(socketFd);
            throw(std::system_error(std::make_error_code(static_cast<std::errc>(errno)), strerror(errno)));
        }
        DEBUG << " " << bytesRead << " bytes." << std::endl;
        TRACE << "Received: " << toString(data) << std::endl;
    }
    TRACE << "Exiting udt_receive()" << std::endl;
    return bytesRead;
}

unreliableTransportC::~unreliableTransportC() {
    if (socketFd != 0) {
        close(socketFd);
    }

}

