#include <vector>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <cstdint>

#ifndef _MESSAGE_HPP_
#define _MESSAGE_HPP_

#include "interface/IClient.hpp"
#include "Protocol.hpp"

template<typename T>
concept isLayoutStd = std::is_standard_layout<T>::value;

template<typename T>
struct MessageHeader {
    uint8_t magic1;
    uint8_t magic2;
    T code{};
    uint32_t size = 0;
};

template<typename T>
class Message {
    public:
        explicit Message() = default;
        explicit Message(const uint8_t m1, const uint8_t m2) {
            head.magic1 = m1;
            head.magic2 = m2;
        }
        ~Message() {};

        void empty() {
            head = {};
            body.clear();
        }
        size_t size()const {
            return body.size();
        }
        bool validMagic(const uint8_t &m1, const uint8_t &m2)const {
            return this->head.magic1 == m1 && this->head.magic2 == m2;
        }
        bool validMagic(const std::pair<uint8_t, uint8_t> &p)const {
            return this->validMagic(p.first, p.second);
        }

        template<isLayoutStd D>
        void insert(const D &data) {
            this->insert(&data);
        };
        template<isLayoutStd D>
        void insert(const D *data) {
            size_t i = body.size();
            body.resize(i + sizeof(D));
            std::memcpy(body.data() + i, data, sizeof(D));
            head.size = body.size();
        };

        template<isLayoutStd D>
        void insert(const std::vector<D> &data) {
            for (const auto &i: data) {
                this->insert(i);
            }
            head.size = body.size();
        };

        // extract data from the end of the buffer
        template<isLayoutStd D>
        void extract(D &data) {
            size_t i = body.size() - sizeof(D);
            std::memcpy(&data, body.data() + i, sizeof(D));
            body.resize(i);
            head.size = body.size();
        };

    public:
        std::shared_ptr<ecs::IClient<T>> remote = nullptr;
        MessageHeader<T> head{};
        std::vector<std::byte> body;
};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Message<T> &msg) {
    os << "Message(head: " << msg.head << ", body: " << std::hex;
    for (const auto &i: msg.body) {
        os << "0x" << static_cast<unsigned>(i) << ":";
    }
    os << std::dec << ")";
    return os;
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const MessageHeader<T> &head) {
    os << std::hex << "0x" << static_cast<unsigned>(head.magic1)
                   << ":0x" << static_cast<unsigned>(head.magic2)
                   << ":0x" << static_cast<unsigned>(head.code)
                   << ":0x" << static_cast<unsigned>(head.size) << std::dec;
    return os;
}

#endif //_MESSAGE_HPP_
