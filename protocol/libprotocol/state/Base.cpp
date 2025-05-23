#include "Base.hpp"

using namespace protocol;
using namespace protocol::state;

Base::Base(Type type, 
    Controller& controller, 
    Timer& timer, 
    StatusSender& statusSender)
    : type(type)
    , controller(controller)
    , timer(timer)
    , statusSender(statusSender)
{
}

void Base::processFrame(const Frame& frame)
{
    switch (frame.getType())
    {
    case Frame::Control::Type::CONNECT:
        return processFrameConnect(frame);
    case Frame::Control::Type::CONNECT_ACK:
        return processFrameConnectAck(frame);
    case Frame::Control::Type::DISCONNECT:
        return processFrameDisconnect(frame);
    case Frame::Control::Type::DATA:
        return processFrameData(frame);
    case Frame::Control::Type::RECEIVE_READY:
        return processFrameReceiveReady(frame);
    case Frame::Control::Type::RECEIVE_NOT_READY:
        return processFrameReceiveNotReady(frame);
    default:
        return;
    }
}