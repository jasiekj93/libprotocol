#pragma once

/**
 * @file Secondary.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

#include <libprotocol/state/Base.hpp>

namespace protocol::state
{
    class Secondary : public Base
    {
    public:
        Secondary(Controller&, Timer&, StatusSender&);

        void processTimer(int) override;

        void notifyPacketEnqueued() override {}
        void notifyStateChangedFrom(Type, bool transmissionError) override;

        void reset() override {}

    protected:
        void processFrameDisconnect(const Frame&) override;
        void processFrameData(const Frame&) override;
        void processFrameReceiveReady(const Frame&) override;
        void processFrameReceiveNotReady(const Frame&) override;
    };
}