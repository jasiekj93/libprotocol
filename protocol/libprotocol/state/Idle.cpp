#include "Idle.hpp"

using namespace protocol;
using namespace protocol::state;

Idle::Idle(Controller& controller, 
    Timer& timer, 
    StatusSender& statusSender)
    : Base(Type::IDLE, controller, timer, statusSender)
{
}

void Idle::processTimer(int tick)
{
    if(tick > timer.getTimeouts().idle and
       wasConnectSent)
       statusSender.sendConnect();
}

void Idle::notifyPacketEnqueued()
{
    connect();
}

void Idle::notifyStateChangedFrom(Type type, bool transmissionError)
{
    if(type == Type::IDLE)
        return;

    controller.clearInputBuffer();

    if (type == Type::SECONDARY and
        not transmissionError)
        controller.analyzeForPacket();

    if(controller.hasFramesToSend() or 
       controller.hasDataToSend())
        connect();
}

void Idle::reset()
{
    wasConnectSent = false;
}

void Idle::processFrameConnect(const Frame& frame)
{
    statusSender.sendConnectAck();
    controller.setState(Type::SECONDARY);
}

void Idle::processFrameConnectAck(const Frame& frame)
{
    if(wasConnectSent == false)
        return;

    wasConnectSent = false;
    controller.setState(Type::PRIMARY);
}

void Idle::processFrameDisconnect(const Frame& frame)
{
    statusSender.sendConnectAck();
}

void Idle::connect()
{
    if(wasConnectSent)
        return;
    
    wasConnectSent = true;
    statusSender.sendConnect();
}
