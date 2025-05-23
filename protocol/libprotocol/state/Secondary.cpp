#include "Secondary.hpp"

using namespace protocol;
using namespace protocol::state;

Secondary::Secondary(Controller& controller, Timer& timer, StatusSender& statusSender)
    : Base(Type::SECONDARY, controller, timer, statusSender)
{
}

void Secondary::processTimer(int tick)
{
    if(tick <= timer.getTimeouts().secondary)
        return;

    controller.setState(Type::IDLE, true);
}

void Secondary::notifyStateChangedFrom(Type type, bool transmissionError)
{
    if (type != Type::IDLE)
		return;

    controller.resetFramesAndIds();
}

void Secondary::processFrameDisconnect(const Frame& frame)
{
    statusSender.sendConnectAck();
    controller.clearFramesAndIds();
    controller.setState(Type::IDLE, false);
}

void Secondary::processFrameData(const Frame& frame)
{
    auto success = controller.tryEnqueueFrame(frame);

    if(not frame.isFinal())
    {
        timer.start();
    }
    else
    {
        if(success)
            controller.enqueueReceivedFrames();

        statusSender.sendReceiveStatus();
    }
}
  
void Secondary::processFrameReceiveReady(const Frame&)
{
    statusSender.sendReceiveStatus();
}

void Secondary::processFrameReceiveNotReady(const Frame&)
{
    statusSender.sendReceiveStatus();
}