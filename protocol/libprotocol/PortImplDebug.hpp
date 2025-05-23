#pragma once

/**
 * @file PortImpl.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <memory>

#include <libprotocol/PortImpl.hpp>
#include <libprotocol/DebugObserver.hpp>

namespace protocol
{
    class PortImplDebug : public PortImpl
    {
    public:
        PortImplDebug(const std::shared_ptr<DebugObserver>& debugObserver,
            const std::shared_ptr<Transciever>& transciever,
            const std::shared_ptr<Observer>& observer,
            etl::span<Byte> packetSendPool,
            etl::span<Byte> packetReceivePool,
            etl::span<Byte> frameReceivePool,
            const Timeouts& timeouts,
            int maxFrameCount);

        void send(const Frame& frame) override;

    private:
        std::shared_ptr<DebugObserver> debugObserver;
    };
}