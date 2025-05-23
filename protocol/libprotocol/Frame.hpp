#pragma once

/**
 * @file Frame.hpp
 * @author Adrian Szczepanski
 * @date 22-05-2025
 */

#include <etl/vector.h>
#include <etl/span.h>
#include <etl/cyclic_value.h>
#include <etl/optional.h>

#include <libprotocol/Byte.hpp>

namespace protocol
{
    struct Frame
    {
    public:
        // START, CONTROL, DATA, CRC (2 bytes), END
        static constexpr size_t MIN_FRAME_SIZE = 5; 
        static constexpr size_t MAX_DATA_SIZE = 256; 
        static constexpr size_t MAX_FRAME_SIZE = MAX_DATA_SIZE + MIN_FRAME_SIZE; 
        static constexpr size_t MAX_ESCAPED_FRAME_SIZE = (2 * (MAX_DATA_SIZE + 3)) + 2; 
        static constexpr Byte START = 0x01;
        static constexpr Byte END = 0x02;
        static constexpr Byte ESC = 0x7E;
        static constexpr Byte FLAGS[] = { START, END };

        struct Control
        {
            enum class Type : Byte
            {
                DATA = 0x00,
                RECEIVE_READY = 0x01,
                RECEIVE_NOT_READY = 0x02,
                CONNECT = 0x03,
                DISCONNECT = 0x04,
                CONNECT_ACK = 0x05,
            };

            using Id = etl::cyclic_value<int, 0, 15>;

            Type type;
            Id id;
            bool isFinal;

            Control() = default;
            Control(Byte);
            Control(Type, Id, bool isFinal);

            Byte toByte() const;
            bool operator==(const Control& other) const;
        };

        static std::pair<etl::optional<Frame>, etl::span<Byte>::iterator> find(etl::span<Byte> buffer);

        Frame() = default;
        Frame(Control, etl::span<const Byte> data);
        Frame(Control::Type, Control::Id = Control::Id(0), etl::span<const Byte> data = {}, bool isFinal = true);

        size_t serialize(etl::ivector<Byte>& buffer) const;
        bool operator==(const Frame& other) const;

        inline auto getType() const { return control.type; }
        inline auto getId() const { return control.id; }
        inline auto isFinal() const { return control.isFinal; }

        Control control;
        etl::vector<Byte, MAX_DATA_SIZE> data;
    };
}