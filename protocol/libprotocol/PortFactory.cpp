#include "PortFactory.hpp"
#include "PortImpl.hpp"
#include "PortImplDebug.hpp"

using namespace protocol;

std::shared_ptr<Port> PortFactory::createPort(const std::shared_ptr<Transciever>& transciever, 
    const std::shared_ptr<Observer>& observer, 
    etl::span<Byte> packetSendPool, 
    etl::span<Byte> packetReceivePool, 
    etl::span<Byte> frameReceivePool, 
    const Timeouts& timeouts, 
    int maxFrameCount)
{
    if(maxFrameCount > Port::MAX_FRAME_COUNT)
        maxFrameCount = Port::MAX_FRAME_COUNT;

    return std::make_shared<PortImpl>(transciever, observer, packetSendPool, packetReceivePool, frameReceivePool, timeouts, maxFrameCount);
}

std::shared_ptr<Port> PortFactory::createDebugPort(const std::shared_ptr<DebugObserver>& debugObserver, 
    const std::shared_ptr<Transciever>& transciever, 
    const std::shared_ptr<Observer>& observer, 
    etl::span<Byte> packetSendPool, 
    etl::span<Byte> packetReceivePool, 
    etl::span<Byte> frameReceivePool, 
    const Timeouts& timeouts, 
    int maxFrameCount)
{
    if(maxFrameCount > Port::MAX_FRAME_COUNT)
        maxFrameCount = Port::MAX_FRAME_COUNT;

    return std::make_shared<PortImplDebug>(debugObserver, transciever, observer, packetSendPool, packetReceivePool, frameReceivePool, timeouts, maxFrameCount);
}