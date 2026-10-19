// Test-only read-only transport: Android consumes original assets in memory.
#pragma once
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#ifdef GOF2_ATLAS_REMOTE_FILES
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

static std::vector<unsigned char> readFixtureAsset(const std::string &root, const char *path) {
#ifdef GOF2_ATLAS_REMOTE_FILES
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    assert(socketFd >= 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(std::stoi(root));
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    assert(connect(socketFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == 0);
    std::string request = std::string(path) + "\n";
    size_t sent = 0;
    while (sent < request.size()) {
        ssize_t count = send(socketFd, request.data() + sent, request.size() - sent, 0);
        assert(count > 0);
        sent += count;
    }
    auto receive = [socketFd](void *buffer, size_t size) {
        auto *bytes = static_cast<unsigned char *>(buffer);
        while (size) {
            ssize_t count = recv(socketFd, bytes, size, 0);
            assert(count > 0);
            bytes += count;
            size -= count;
        }
    };
    uint32_t networkSize;
    receive(&networkSize, sizeof(networkSize));
    uint32_t size = ntohl(networkSize);
    assert(size && size < 64U * 1024U * 1024U);
    std::vector<unsigned char> bytes(size);
    receive(bytes.data(), size);
    assert(close(socketFd) == 0);
    return bytes;
#else
    std::ifstream file(root + "/" + path, std::ios::binary);
    assert(file.good());
    return {std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};
#endif
}
