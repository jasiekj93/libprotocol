#include "Primary.hpp"

using namespace protocol;
using namespace protocol::state;

Primary::Primary(Controller& controller, 
    Timer& timer, 
    StatusSender& statusSender, 
    int maxRetransmissionCount)
    : Base(Type::PRIMARY, controller, timer, statusSender)
    , maxRetransmissionCount(maxRetransmissionCount)
{
}

void Primary::processTimer(int tick)
{
    if(tick <= timer.getTimeouts().primary)
        return;

    if (wasDisconnectSent)
		disconnect();
	else
        processRetransmissionOnTineout();
}

void Primary::notifyPacketEnqueued()
{
    // if(not controller.hasFramesToSend())
	// 	controller.pushAndSendNextData();
}

void Primary::notifyStateChangedFrom(Type type, bool transmissionError)
{
    if (type != Type::IDLE)
		return;

    reset();

	if (wasRetransmissionOverload())
	{
        controller.resetFramesAndIds();
        controller.sendEnqueuedFrames();
	}
	else if (controller.hasDataToSend())
		controller.pushAndSendNextData();
	else
	{
		wasRRSent = true;
        statusSender.sendReceiveStatus();
	}
}

void Primary::reset()
{
    retransmissionCount = 0;
    wasRRSent = false;
    wasDisconnectSent = false;
}

void Primary::processFrameConnectAck(const protocol::Frame& frame)
{
    if(not wasDisconnectSent)
        return;

    wasDisconnectSent = false;

    controller.clearFramesAndIds();
    controller.setState(Type::IDLE);
}

void Primary::processFrameReceiveReady(const protocol::Frame& frame)
{
	auto framesCount = controller.framesToSendCount();
	
    if(not controller.popConfirmedSentFrames(frame.getId()))
        processTransmissionError(framesCount);
	else
	{
        retransmissionCount = 0;

		if (controller.hasDataToSend())
		{
			if(not controller.hasFramesToSend())
                controller.pushAndSendNextData();
		}
		else
		{
            controller.notifyAllDataSent();
            disconnect();
		}
	}
}

void Primary::processFrameReceiveNotReady(const protocol::Frame& frame)
{
    controller.popConfirmedSentFrames(frame.getId()); //jeżeli nie odebrał wszystkich ramek to w RNR nie jest to błąd
    wasRRSent = true;
    statusSender.sendReceiveStatus();
}

void Primary::disconnect()
{
    wasDisconnectSent = true;
    statusSender.sendDisconnect();
}

void Primary::processRetransmissionOverload()
{
    retransmissionCount = 0;
    wasRRSent = false;
    controller.setState(Type::IDLE, true);
}

bool Primary::wasRetransmissionOverload() const
{
    return (controller.hasFramesToSend());
}

void Primary::processTransmissionError(int framesCount)
{
    if (framesCount == controller.framesToSendCount())	//nic nie zdjęto z kolejki	
	{	
		retransmissionCount++;
		if (retransmissionCount > maxRetransmissionCount)
			processRetransmissionOverload();
	}
	else
        retransmissionCount = 0;

    controller.sendEnqueuedFrames();
}

void Primary::processRetransmissionOnTineout()
{
    retransmissionCount++;

    if (retransmissionCount > maxRetransmissionCount)
        processRetransmissionOverload();
    else if (wasRRSent)
        statusSender.sendReceiveStatus();
    else
        controller.sendEnqueuedFrames();
}
