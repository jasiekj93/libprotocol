#pragma once

#include <libprotocol/Port.hpp>
#include <libprotocol/Observer.hpp>
#include <libprotocol/DebugObserver.hpp>
#include <libprotocol/Transciever.hpp>
#include <libprotocol/Timeouts.hpp>

namespace protocol
{
    class PortFactory
    {
    public:
        static std::shared_ptr<Port> createPort(const std::shared_ptr<Transciever>&,
            const std::shared_ptr<Observer>&,
            etl::span<Byte> packetSendPool,
            etl::span<Byte> packetReceivePool,
            etl::span<Byte> frameReceivePool,
            const Timeouts& timeouts = Timeouts(),
            int maxFrameCount = Port::MAX_FRAME_COUNT);

    static std::shared_ptr<Port> createDebugPort(const std::shared_ptr<DebugObserver>&,
            const std::shared_ptr<Transciever>&,
            const std::shared_ptr<Observer>&,
            etl::span<Byte> packetSendPool,
            etl::span<Byte> packetReceivePool,
            etl::span<Byte> frameReceivePool,
            const Timeouts& timeouts = Timeouts(),
            int maxFrameCount = Port::MAX_FRAME_COUNT);
    };
}