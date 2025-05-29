#pragma once

/**
 * @file PortImpl.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <memory>

#include <libprotocol/Port.hpp>
#include <libprotocol/Observer.hpp>
#include <libprotocol/Transciever.hpp>
#include <libprotocol/state/ControllerImpl.hpp>
#include <libprotocol/state/PhysicalLayer.hpp>

namespace protocol
{
    class PortImpl : public Port, public state::PhysicalLayer 
    {
    public:
        PortImpl(const std::shared_ptr<Transciever>& transciever,
            const std::shared_ptr<Observer>& observer,
            etl::span<Byte> packetSendPool,
            etl::span<Byte> packetReceivePool,
            etl::span<Byte> frameReceivePool,
            const Timeouts& timeouts,
            int maxFrameCount);

        //Port
        void process(etl::span<const Byte>) override;
        inline void incrementTimer() { controller->incrementTimer(); }
        inline bool sendPacket(const Packet& packet) { return controller->enqueuePacket(packet); }

        //PhysicalLayer
        void send(const Frame& frame) override;
        void clearReceiveBuffer() override { receiveBuffer.clear(); }

    protected:
        std::unique_ptr<state::ControllerImpl> controller;

    private:
        std::shared_ptr<Transciever> transciever;
        std::shared_ptr<Observer> observer;
        etl::vector_ext<Byte> receiveBuffer;
    };
}