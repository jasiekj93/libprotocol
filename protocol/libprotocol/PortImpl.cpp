#include "PortImpl.hpp"
#include <iostream>

using namespace protocol;

PortImpl::PortImpl(const std::shared_ptr<Transciever> &transciever, 
    const std::shared_ptr<Observer> &observer, 
    etl::span<Byte> sendPool, 
    etl::span<Byte> receivePool, 
    etl::span<Byte> frameReceivePool,
    const Timeouts& timeouts, 
    int maxFrameCount)
    : controller(std::make_unique<state::ControllerImpl>(*this, *observer, sendPool, receivePool, timeouts, maxFrameCount))
    , transciever(transciever)
    , observer(observer)
    , receiveBuffer(frameReceivePool.data(), frameReceivePool.size())
{
}

void PortImpl::send(const Frame& frame)
{
    static etl::vector<Byte, Frame::MAX_ESCAPED_FRAME_SIZE> buffer;

    frame.serialize(buffer);
    transciever->send(buffer);
    buffer.clear();
}

void PortImpl::process(etl::span<const Byte> received)
{
    if (received.empty())
        return;
    
    if(received.size() > receiveBuffer.available())
    {
        auto end = etl::next(receiveBuffer.begin(), received.size() - receiveBuffer.available());
        receiveBuffer.erase(receiveBuffer.begin(), end);
    }

    receiveBuffer.insert(receiveBuffer.end(), received.begin(), received.end());

    while(true)
    {
        auto span = etl::span<Byte>(receiveBuffer.begin(), receiveBuffer.end());
        auto [result, iterator] = Frame::find(span);

        if(iterator != span.end() or result.has_value())
            receiveBuffer.erase(receiveBuffer.begin(), iterator);

        if(receiveBuffer.empty())
            receiveBuffer.clear();

        if(result.has_value())
            controller->processFrame(result.value());
        else
            break;
    }
}