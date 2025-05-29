#include "Packet.hpp"

using namespace protocol;

std::pair<etl::optional<Packet>, etl::span<const Byte>::iterator> Packet::find(etl::span<const Byte> data)
{
    auto start = data.begin();

    if(etl::distance(start, data.end()) < MIN_SIZE)
        return { etl::nullopt, data.end() };
    
    auto type = *start;
    auto length = *(etl::next(start)) | ((Length)(*(etl::next(start, 2))) << 8);

    if(length <= etl::distance(etl::next(start, 3), data.end()))
    {
        auto end = etl::next(start, 3 + length);
        return { Packet(type, etl::span<const Byte>(etl::next(start, 3), end)), end };
    }
    else
        return { etl::nullopt, data.end() };
}

Packet::Packet(Type type, Payload data)
    : type(type)
{
   this->payload = (data.size() > MAX_DATA_SIZE) ?
        Payload{} :
        data;
}

bool Packet::serialize(etl::ivector<Byte>& input) const
{
    if(input.available() < (MIN_SIZE + getLength()))
        return false;

    input.push_back((Byte)type);
    input.push_back((Byte)getLength());
    input.push_back((Byte)(getLength() >> 8));
    input.insert(input.end(), payload.begin(), payload.end());

    return true;
}

bool Packet::operator==(const Packet& other) const
{
    return (type == other.type) and etl::equal(payload, other.payload);
}