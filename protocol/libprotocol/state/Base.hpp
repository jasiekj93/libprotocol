#pragma once

/**
 * @file Base.hpp
 * @author Adrian Szczepanski
 * @date 15-05-2025
 */

#include <memory>

#include <libprotocol/Frame.hpp>

#include <libprotocol/state/Type.hpp>
#include <libprotocol/state/StatusSender.hpp>
#include <libprotocol/state/Controller.hpp>
#include <libprotocol/state/Timer.hpp>

namespace protocol::state
{
    class Base 
    {
    public:
        Base(Type type, 
            Controller& controller,
            Timer& timer,
            StatusSender& statusSender);
            
        void processFrame(const Frame&);
        virtual void processTimer(int) = 0;

        virtual void notifyStateChangedFrom(Type, bool transmissionError) = 0;
        virtual void notifyPacketEnqueued() = 0;

        virtual void reset() = 0;

        inline auto getType() const { return type; }

    protected:
        virtual void processFrameData(const Frame&) {}
        virtual void processFrameConnectAck(const Frame&) {}
        virtual void processFrameReceiveReady(const Frame&) {}
        virtual void processFrameReceiveNotReady(const Frame&) {}
        virtual void processFrameDisconnect(const Frame&) {}
        virtual void processFrameConnect(const Frame&) {}

        Controller& controller;
        Timer& timer;
        StatusSender& statusSender;

    private:
        Type type;
    };
}