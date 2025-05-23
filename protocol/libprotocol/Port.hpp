#pragma once

/**
 * @file Port.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <libprotocol/Packet.hpp>

namespace protocol
{
    class Port
    {
    public:
        static constexpr int MAX_FRAME_COUNT = 16;

        virtual ~Port() = default;

        virtual void process(etl::span<const Byte> = {}) = 0;
        virtual void incrementTimer() = 0;
        virtual bool sendPacket(const Packet& packet) = 0;
    };
}