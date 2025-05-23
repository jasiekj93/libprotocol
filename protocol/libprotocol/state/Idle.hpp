#pragma once

/**
 * @file Idle.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

#include <libprotocol/state/Base.hpp>

namespace protocol::state
{
    class Idle : public Base
    {
    public:
        Idle(Controller&, Timer&, StatusSender&);

        void processTimer(int) override;

        void notifyPacketEnqueued() override;
        void notifyStateChangedFrom(Type, bool transmissionError) override;

        void reset() override;

    protected:
        void processFrameConnect(const Frame&) override;
        void processFrameConnectAck(const Frame&) override;
        void processFrameDisconnect(const Frame&) override;

        void connect();

    private:
        bool wasConnectSent = false;
    };
}