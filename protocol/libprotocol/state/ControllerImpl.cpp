#include "ControllerImpl.hpp"
#include <libprotocol/state/Primary.hpp>
#include <libprotocol/state/Secondary.hpp>
#include <libprotocol/state/Idle.hpp>

#include <iostream>

using namespace protocol;
using namespace protocol::state;

ControllerImpl::ControllerImpl(PhysicalLayer& physical, 
    Observer& observer, 
    etl::span<uint8_t> sendPool, 
    etl::span<uint8_t> receivePool, 
    const Timeouts& timeouts, 
    int maxFrameCount)
    : physical(physical)
    , observer(observer)
    , sendBuffer(sendPool.data(), sendPool.size())
    , receiveBuffer(receivePool.data(), receivePool.size())
    , timeouts(timeouts)
    , maxFrameCount(maxFrameCount)
    , id(0)
    , isTimerStarted(false)
    , isBusy(false)
    , timerTick(0)
{
    states[state::Type::IDLE] = std::make_unique<state::Idle>(*this, *this, *this);
    states[state::Type::PRIMARY] = std::make_unique<state::Primary>(*this, *this, *this);
    states[state::Type::SECONDARY] = std::make_unique<state::Secondary>(*this, *this, *this);

    currentState = states[state::Type::IDLE].get();
}

bool ControllerImpl::enqueuePacket(const Packet& packet)
{
    if(packet.serialize(sendBuffer) == false)
        return false;

    currentState->notifyPacketEnqueued();
    return true;
}

void ControllerImpl::processFrame(const Frame& frame)
{
    stop();
    currentState->processFrame(frame);
}

void ControllerImpl::incrementTimer()
{
    if(isTimerStarted)
        timerTick++;

    currentState->processTimer(timerTick);
}

void ControllerImpl::setState(state::Type type, bool wasError)
{
    auto previousType = currentState->getType();
    currentState = states[type].get();
    currentState->notifyStateChangedFrom(previousType, wasError);
}

void ControllerImpl::analyzeForPacket()
{
    while(true)
    {
        auto span = etl::span<uint8_t>(receiveBuffer.begin(), receiveBuffer.end());
        auto [result, iterator] = Packet::find(span);

        if(result.has_value())
        {
            Packet packet(result.value());
            observer.receivedPacketCallback(packet);

            receiveBuffer.pop(etl::distance(span.cbegin(), iterator));
        }
        else
        {
            if(receiveBuffer.empty())
                receiveBuffer.clear();
            return;
        }
    }
}

void ControllerImpl::sendEnqueuedFrames()
{
    for(auto& frame : framesToSend)
        physical.send(frame);

    start();
}

bool ControllerImpl::enqueueReceivedFrames()
{
    if(receivedFrames.empty())
        return false;

    if(receivedFrames.back().isFinal() == false)
        return false;

    for(auto& frame : receivedFrames)
    {
        if(receiveBuffer.available() < frame.data.size())
        {
            observer.receivePoolOverflowCallback();
            return false;
        }
        else
            receiveBuffer.push(frame.data.begin(), frame.data.end());
    }

    receivedFrames.clear();
    return true;
}

void ControllerImpl::pushAndSendNextData()
{
    while(not sendBuffer.empty() and
          framesToSend.size() < maxFrameCount)
    {
        auto size = Frame::MAX_DATA_SIZE;
        bool isFinal = (framesToSend.size() == (maxFrameCount - 1));

        if(sendBuffer.size() <= size)
        {
            size = sendBuffer.size();
            isFinal = true;
        }

        framesToSend.push_back(Frame(
            Frame::Control::Type::DATA,
            id,
            etl::span<uint8_t>(sendBuffer.begin(), size),
            isFinal));
            
        sendBuffer.pop(size);
        id++;
    }

    if(not framesToSend.empty())
        sendEnqueuedFrames(); 
}

bool ControllerImpl::tryEnqueueFrame(const Frame& frame)
{
    if(frame.getId() != id)
        return false;

    if(receivedFrames.size() == maxFrameCount)
        return false;

    receivedFrames.push_back(frame);
    id++;
    return true;
}

bool ControllerImpl::popConfirmedSentFrames(Frame::Control::Id id)
{
    if(framesToSend.empty())
        return true;

    auto lastConfirmed = id;
    lastConfirmed--;

    size_t index = 0;

    while(index < framesToSend.size() and
        framesToSend[index].getId() != lastConfirmed)
        index++;

    if(index == framesToSend.size())
        return false;

    for(size_t i = 0; i <= index; i++)
        framesToSend.pop_front();

    return framesToSend.empty();
}

void ControllerImpl::clearFramesAndIds()
{
    framesToSend.clear();
    receivedFrames.clear();
    id = 0;
}

void ControllerImpl::resetFramesAndIds()
{
    id = 0;

    for(auto& frame : framesToSend)
    {
        frame.control.id = id;
        id++;
    }
}

void ControllerImpl::sendReceiveStatus()
{
    physical.send(Frame(
        (isBusy ? Frame::Control::Type::RECEIVE_NOT_READY : Frame::Control::Type::RECEIVE_READY),
        id));
    start();
}

void ControllerImpl::sendConnect()
{
    physical.send(Frame(Frame::Control::Type::CONNECT));

    start();
}

void ControllerImpl::sendDisconnect()
{
    physical.send(Frame(Frame::Control::Type::DISCONNECT));
    start();
}

void ControllerImpl::sendConnectAck()
{
    physical.send(Frame(Frame::Control::Type::CONNECT_ACK));
    start();
}

void ControllerImpl::start()
{
    isTimerStarted = true;
    timerTick = 0;
}

void ControllerImpl::stop()
{
    isTimerStarted = false;
    timerTick = 0;
}

void ControllerImpl::pause()
{
    isTimerStarted = false;
}
