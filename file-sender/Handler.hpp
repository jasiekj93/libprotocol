#pragma once

/**
 * @file Handler.hpp
 * @author Adrian Szczepanski
 * @date 27-05-2025
 */

#include <fstream>

#include <etl/array.h>

#include <libprotocol/PortFactory.hpp>
#include "Serial.hpp"
#include "Presenter.hpp"
#include "Packet.hpp"

class Handler : public protocol::Observer, public std::enable_shared_from_this<Handler>
{
public:
    Handler(std::string_view path, std::string_view fileName = {});

    void run();

    void receivedPacketCallback(const protocol::Packet& packet) override;
    void receivePoolOverflowCallback() override;
    void allDataSentCallback() override;

protected:
    void createPort();
    void runAsSender();
    void runAsReceiver();

    void processFileHeader(const FileHeader& header);
    void processFileData(const FileData& data);
    void processFileEnd(const FileEnd& end);

    unsigned long getTick();

private:
    std::shared_ptr<Serial> serial;
    std::shared_ptr<Presenter> presenter;
    std::shared_ptr<protocol::Port> port;

    std::fstream file;
    std::string fileName;

    bool isDone = false;
    bool isEndSent = false;
    int fileSize = 0;
    int packetSize = 0;
    unsigned long tick = 0;

    etl::array<uint8_t, protocol::Packet::MAX_PACKET_SIZE> sendPool;
    etl::array<uint8_t, protocol::Packet::MAX_PACKET_SIZE> receivePool;
    etl::array<uint8_t, 20 * protocol::Frame::MAX_ESCAPED_FRAME_SIZE> frameReceivePool;
};