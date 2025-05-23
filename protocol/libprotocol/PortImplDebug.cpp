#include "PortImplDebug.hpp"
#include <libprotocol/state/ControllerImplDebug.hpp>

using namespace protocol;

PortImplDebug::PortImplDebug(const std::shared_ptr<DebugObserver>& debugObserver,
    const std::shared_ptr<Transciever>& transciever,
    const std::shared_ptr<Observer>& observer,
    etl::span<Byte> sendPool,
    etl::span<Byte> receivePool,
    etl::span<Byte> frameReceivePool,
    const Timeouts& timeouts, 
    int maxFrameCount)
    : PortImpl(transciever, observer, sendPool, receivePool, frameReceivePool, timeouts, maxFrameCount)
    , debugObserver(debugObserver)
{
    controller.reset(new state::ControllerImplDebug(*debugObserver, *this, *observer, sendPool, receivePool, timeouts, maxFrameCount));
}

void PortImplDebug::send(const protocol::Frame& frame)
{
    debugObserver->sentFrameCallback(controller->getState(), frame);
    PortImpl::send(frame);
}