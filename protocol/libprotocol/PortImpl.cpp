#include "PortImpl.hpp"

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
        receiveBuffer.pop(received.size() - receiveBuffer.available());

    receiveBuffer.push(received.begin(), received.end());

    while(true)
    {
        auto span = etl::span<Byte>(receiveBuffer.begin(), receiveBuffer.end());
        auto [result, iterator] = Frame::find(span);

        if(result.has_value())
        {
            receiveBuffer.pop(etl::distance(span.begin(), iterator));
            controller->processFrame(result.value());
        }
        else
        {
            if(receiveBuffer.empty())
                receiveBuffer.clear();

            return;
        }
    }
}
