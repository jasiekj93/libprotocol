#pragma once

/**
 * @file Packet.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <utility>

#include <etl/span.h>
#include <etl/optional.h>
#include <etl/circular_buffer.h>

#include <libprotocol/Byte.hpp>

namespace protocol
{
    class Packet 
    {
    public:
        static constexpr size_t MIN_SIZE = 3;
        static constexpr size_t MAX_DATA_SIZE = 0xFFFF;
        static constexpr size_t MAX_PACKET_SIZE = 0xFFFF + 3;

        using Type = uint8_t;
        using Length = uint16_t;
        using Payload = etl::span<const Byte>;

        static std::pair<etl::optional<Packet>, etl::span<const Byte>::iterator> find(etl::span<const Byte>);

        Packet(Type, Payload = {});

        bool serialize(etl::icircular_buffer<Byte>& input) const;
        bool operator==(const Packet& other) const;

        inline Payload getData() const { return payload; }
        inline void setData(Payload data) { payload = data; }
        inline Type getType() const { return type; }
        inline Length getLength() const { return payload.size(); }

    private:
        Type type;
        Payload payload;
    };
}