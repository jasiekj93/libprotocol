#pragma once

#include <libprotocol/Packet.hpp>

enum class PacketType : protocol::Packet::Type
{
    FILE_HEADER = 0x01,
    FILE_DATA = 0x02,
    FILE_END = 0x03
};

class FileHeader : public protocol::Packet
{
public:
    FileHeader(const std::string& fileName)
        : Packet(static_cast<protocol::Packet::Type>(PacketType::FILE_HEADER))
        , fileName(fileName)
    {
        setData(etl::span<const uint8_t>(reinterpret_cast<const uint8_t*>(fileName.data()), fileName.size()));
    }

    FileHeader(const protocol::Packet& packet)
        : Packet(packet)
    {
        if (getType() != static_cast<protocol::Packet::Type>(PacketType::FILE_HEADER))
            throw std::runtime_error("Invalid packet type for FileHeader");
        
        auto data = getData();
        fileName.assign(reinterpret_cast<const char*>(data.data()), data.size());
    }

    inline auto& getFileName() const
    {
        return fileName;
    }

private:
    std::string fileName;
};


class FileData : public protocol::Packet
{
public:
    FileData(etl::span<const uint8_t> data)
        : Packet(static_cast<protocol::Packet::Type>(PacketType::FILE_DATA))
    {
        if(data.size() > protocol::Packet::MAX_DATA_SIZE)
            throw std::runtime_error("Data size exceeds maximum allowed size");
        setData(data);
    }

    FileData(const protocol::Packet& packet)
        : Packet(packet)
    {
        if (getType() != static_cast<protocol::Packet::Type>(PacketType::FILE_DATA))
            throw std::runtime_error("Invalid packet type for FileData");
        
        auto data = getData();
        if (data.size() > protocol::Packet::MAX_DATA_SIZE)
            throw std::runtime_error("Data size exceeds maximum allowed size");
        
        buffer.assign(data.begin(), data.end());
    }

private:
    etl::array<uint8_t, protocol::Packet::MAX_DATA_SIZE> buffer;
};


class FileEnd : public protocol::Packet
{
public:
    FileEnd()
        : Packet(static_cast<protocol::Packet::Type>(PacketType::FILE_END))
    {
    }

    FileEnd(const protocol::Packet& packet)
        : Packet(packet)
    {
        if (getType() != static_cast<protocol::Packet::Type>(PacketType::FILE_END))
            throw std::runtime_error("Invalid packet type for FileEnd");
    }
};