#include "ControllerImplDebug.hpp"

using namespace protocol;
using namespace protocol::state;

ControllerImplDebug::ControllerImplDebug(DebugObserver& debugObserver,
    PhysicalLayer& physical,
    Observer& observer,
    etl::span<Byte> sendPool,
    etl::span<Byte> receivePool,
    const Timeouts& timeouts,
    int maxFrameCount)
    : ControllerImpl(physical, observer, sendPool, receivePool, timeouts, maxFrameCount)
    , debugObserver(debugObserver)
{
}

void ControllerImplDebug::processFrame(const Frame& frame)
{
    debugObserver.receivedFrameCallback(getCurrentState()->getType(), frame);
    ControllerImpl::processFrame(frame);
}

void ControllerImplDebug::setState(state::Type type, bool wasError)
{
    debugObserver.stateChangedCallback(getCurrentState()->getType(), type, wasError);
    ControllerImpl::setState(type, wasError);
}

void ControllerImplDebug::notifyAllDataSent()
{
    debugObserver.allDataSentCallback(getCurrentState()->getType());
    ControllerImpl::notifyAllDataSent();
}