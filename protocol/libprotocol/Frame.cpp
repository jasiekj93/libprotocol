#include "Frame.hpp"

#include <etl/byte.h>
#include <etl/crc16.h>

#include <libprotocol/Escapes.hpp>

using namespace protocol;

static etl::span<Byte>::iterator findEnd(etl::span<Byte>::iterator start, etl::span<Byte>::iterator end)
{
    while(true)
    {
        auto it = std::find(start, end, Frame::END);

        if(it == end or it == start)
            return end;

        if(*(etl::prev(it)) == Frame::ESC)
        {
            start = etl::next(it);
            continue;
        }
        else 
            return it;
    }

    return end;
}

Frame::Control::Control(Byte value) 
{
    auto byte = etl::byte(value);

    isFinal = etl::to_integer<bool>(byte & etl::byte(0x01));
    type = static_cast<Type>((byte >> 1) & etl::byte(0x07));
    id = etl::to_integer<int>(byte >> 4);
}

Frame::Control::Control(Type type, Id id, bool isFinal)
    : type(type), id(id), isFinal(isFinal)
{
}

Byte Frame::Control::toByte() const
{
    auto byte = etl::byte(isFinal);
    byte |= (etl::byte(type) << 1);
    byte |= (etl::byte(id.get()) << 4);

    return etl::to_integer<Byte>(byte);
}

bool Frame::Control::operator==(const Control& other) const
{
    return (isFinal == other.isFinal) and (type == other.type) and (id == other.id);
}

std::pair<etl::optional<Frame>, etl::span<Byte>::iterator> Frame::find(etl::span<Byte> buffer)
{
    static const EscapeConfig config = { ESC, FLAGS }; 

    auto start = std::find(buffer.begin(), buffer.end(), START);
    if(start == buffer.end())
        return { etl::nullopt, buffer.end() };

    auto end = findEnd(start, buffer.end());

    if(end == buffer.end())
        return { etl::nullopt, buffer.end() };

    etl::vector<Byte, MAX_FRAME_SIZE> frameBuffer;

    if(not removeEscapes({ start, etl::next(end) }, frameBuffer, config))
        return { etl::nullopt, etl::next(end) };
    
    auto calculatedCrc = etl::crc16(etl::next(frameBuffer.begin()), etl::prev(frameBuffer.end(),3)); 
    auto crc = *((etl::crc16*)((std::prev(frameBuffer.end(), 3))));

    if(calculatedCrc != crc)
        return { etl::nullopt, etl::next(end) };

    auto control = *etl::next(frameBuffer.begin());
    auto data = etl::span<Byte>(etl::next(frameBuffer.begin(), 2), etl::prev(frameBuffer.end(), 3));

    return { Frame(Control(control), data), etl::next(end) };
}

Frame::Frame(Control control, etl::span<const Byte> data) 
    : control(control)
{
    if(data.size() <= MAX_DATA_SIZE)
        this->data.insert(this->data.end(), data.begin(), data.end());
}

Frame::Frame(Control::Type type, Control::Id id, etl::span<const Byte> data, bool isFinal)
    : control(type, id, isFinal)
{
    if(data.size() <= MAX_DATA_SIZE)
        this->data.insert(this->data.end(), data.begin(), data.end());
}

size_t Frame::serialize(etl::ivector<Byte>& buffer) const
{
    static const EscapeConfig config = { ESC, FLAGS }; 

    if(buffer.available() < MAX_ESCAPED_FRAME_SIZE)
        return 0;
    
    auto frameStart = buffer.end();

    buffer.push_back(START);

    auto controlByte = control.toByte();
    
    if(not addEscapes({ &controlByte, 1 }, buffer, config))
        return 0;

    if(not addEscapes(data, buffer, config))
        return 0;

    etl::crc16 crc;
    crc.add(control.toByte()); 
    crc.add(data.begin(), data.end());

    if(not addEscapes({ (Byte*)(&crc), 2 }, buffer, config))
        return 0;

    buffer.push_back(END);

    return etl::distance(frameStart, buffer.end());
}

bool Frame::operator==(const Frame& other) const
{
    return (control == other.control) and (data == other.data);
}