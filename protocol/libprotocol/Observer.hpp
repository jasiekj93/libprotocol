#pragma once

#include <libprotocol/Packet.hpp>

namespace protocol
{
    class Observer
    {
    public:
        virtual ~Observer() = default;

        virtual void receivedPacketCallback(const Packet& packet) = 0;
        virtual void receivePoolOverflowCallback() = 0;
        virtual void allDataSentCallback() = 0;
    };
}