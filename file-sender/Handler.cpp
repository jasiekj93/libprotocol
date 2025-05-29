#include "Handler.hpp"

#include <iostream>
#include <chrono>

using namespace protocol;

Handler::Handler(std::string_view path, std::string_view fileName)
    : serial(std::make_shared<Serial>(path))
    , presenter(std::make_shared<Presenter>())
    , port(nullptr)
    , fileName(fileName)
{
    if (not fileName.empty())
    {
        file.open(fileName.data(), std::ios::binary | std::ios::in);
        if (not file.is_open())
            throw std::runtime_error("Error opening file: " + std::string(fileName));
    }
}

void Handler::run()
{
    createPort();
    (file.is_open() ? runAsSender() : runAsReceiver());
}

void Handler::receivedPacketCallback(const Packet& packet)
{
    switch (packet.getType())
    {
        case static_cast<Packet::Type>(PacketType::FILE_HEADER):
        {
            processFileHeader(FileHeader(packet));
            break;
        }
        case static_cast<Packet::Type>(PacketType::FILE_DATA):
        {
            processFileData(FileData(packet));
            break;
        }
        case static_cast<Packet::Type>(PacketType::FILE_END):
        {
            processFileEnd(FileEnd(packet));
            break;
        }
        default:
            throw std::runtime_error("Unknown packet type received: " + std::to_string(static_cast<int>(packet.getType())));
    }
}

void Handler::receivePoolOverflowCallback()
{
    throw std::runtime_error("Receive pool overflow occurred");
}

void Handler::allDataSentCallback()
{
    static etl::array<uint8_t, Packet::MAX_DATA_SIZE> buffer;
    fileSize += packetSize;

    if(isEndSent)
    {
        isDone = true;
        return;
    }

    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    packetSize = static_cast<int>(file.gcount());

    if(packetSize <= 0)
    {
        file.close();
        port->sendPacket(FileEnd());
        isEndSent = true;
        std::cout << "File transfer completed, total size: " << fileSize << " bytes." << std::endl;
    }
    else
    {
        port->sendPacket(FileData(etl::span<const uint8_t>(buffer.data(), packetSize)));
        std::cout << "Packet sent, transferred size: " << fileSize << " bytes." << std::endl;
    }

}

void Handler::createPort()
{
    port = PortFactory::createDebugPort(
        presenter,
        serial,
        shared_from_this(),
        sendPool,
        receivePool,
        frameReceivePool,
        protocol::Timeouts()
    );
}

void Handler::runAsSender()
{

    if (not file.is_open())
        throw std::runtime_error("File is not open for writing");

    std::cout << "Starting file transfer as sender..." << std::endl;

    if (not port->sendPacket(FileHeader(fileName)))
        throw std::runtime_error("Failed to send file header");

    while (!isDone)
    {
        port->process(serial->read());

        if(auto newTick = getTick(); newTick > tick + 10)
        {
            tick = newTick;
            port->incrementTimer();
        }
    }
}

void Handler::runAsReceiver()
{
    std::cout << "Starting file transfer as receiver..." << std::endl;

    while (!isDone)
    {
        port->process(serial->read());

        if(auto newTick = getTick(); newTick > tick + 10)
        {
            tick = newTick;
            port->incrementTimer();
        }
    }
}

void Handler::processFileHeader(const FileHeader& header)
{
    std::cout << "Received file header: " << header.getFileName() << std::endl;

    if(file.is_open())
        file.close();

    file.open(header.getFileName(), std::ios::binary | std::ios::out | std::ios::trunc);
    if (!file)
        throw std::runtime_error("Error opening file: " + header.getFileName());
}

void Handler::processFileData(const FileData& data)
{
    if (file.is_open())
    {
        file.write(reinterpret_cast<const char*>(data.getData().data()), data.getData().size());
        if (!file)
            throw std::runtime_error("Error writing to file");
        else
            fileSize += data.getData().size();
    }
    else
        std::cerr << "File not open for writing." << std::endl;
}

void Handler::processFileEnd(const FileEnd& end)
{
    if (file.is_open())
    {
        file.close();
        std::cout << "File transfer completed." << std::endl;
        isDone = true;
    }
    else
        std::cerr << "File not open for writing." << std::endl;
}

unsigned long Handler::getTick() 
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().
                time_since_epoch()).count();
}