#pragma once

/**
 * @file PortUser.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <vector>
#include <memory>
#include <iostream>

#include <etl/array.h>

#include <libprotocol/PortFactory.hpp>

namespace protocol::mock
{
    class PortUser : public Transciever, 
        public Observer,
        public DebugObserver,
        public std::enable_shared_from_this<PortUser>
    {
    public:
        explicit PortUser(std::string_view name, std::ostream& = std::cout);
        
        void init(Port*);
        void sendTo(PortUser* recipent); 

        void sendPacket(const Packet& packet);
        void receive(etl::span<const uint8_t> data);

        // Transciever
        void send(etl::span<const uint8_t> data) override;

        // Observer
        void receivedPacketCallback(const Packet&) override;
        void receivePoolOverflowCallback() override {}
        void allDataSentCallback() override;

        // DebugObserver
        void receivedFrameCallback(state::Type, const Frame&) override;
        void sentFrameCallback(state::Type, const Frame&) override;
        void stateChangedCallback(state::Type previous, state::Type actual, bool wasError) override;
        void allDataSentCallback(state::Type) override;
        

        std::vector<uint8_t> resultData;
        Packet packet;
        bool isAllDataSent = false;
        bool isPacketReceived = false;

        etl::array<uint8_t, Packet::MAX_PACKET_SIZE> sendPool;
        etl::array<uint8_t, Packet::MAX_PACKET_SIZE> receivePool;
        etl::array<uint8_t, Port::MAX_FRAME_COUNT * Frame::MAX_FRAME_SIZE> frameReceivePool;

    private:
        std::string name;
        std::ostream& out;
        std::vector<uint8_t> sendBuffer;
        Port* port = nullptr;
        
    };
}