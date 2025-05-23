#pragma once

/**
 * @file Controller.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

#include <libprotocol/Frame.hpp>
#include <libprotocol/state/Type.hpp>

namespace protocol::state
{
    class Controller
    {
    public:
        virtual ~Controller() = default;

        virtual void setState(Type type, bool wasError = false) = 0;
        virtual Type getState() const = 0;

        virtual void clearInputBuffer() = 0;
        virtual void resetFramesAndIds() = 0;
        virtual void clearFramesAndIds() = 0;

        virtual void pushAndSendNextData() = 0;
        virtual void sendEnqueuedFrames() = 0;
        virtual bool popConfirmedSentFrames(Frame::Control::Id) = 0;

        virtual int framesToSendCount() const = 0;
        virtual bool hasFramesToSend() const = 0;
        virtual bool hasDataToSend() const = 0; 

        virtual void notifyAllDataSent() = 0;

        virtual bool tryEnqueueFrame(const Frame&) = 0;
        virtual bool enqueueReceivedFrames() = 0;

        virtual void analyzeForPacket() = 0;
    };
}