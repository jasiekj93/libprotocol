#pragma once

/**
 * @file Primary.hpp
 * @author Adrian Szczepanski
 * @date 19-05-2025
 */

#include <libprotocol/state/Base.hpp>

namespace protocol::state
{
    class Primary : public Base
    {
    public:
        static constexpr int DEFAULT_RETRANSMISSION_COUNT = 3;

        Primary(Controller&, Timer&, StatusSender&,
            int maxRetransmissionCount = DEFAULT_RETRANSMISSION_COUNT);

        void processTimer(int) override;

        void notifyPacketEnqueued() override;
        void notifyStateChangedFrom(Type, bool transmissionError) override;

        void reset() override;

    protected:
        void processFrameConnectAck(const Frame&) override;
        void processFrameReceiveReady(const Frame&) override;
        void processFrameReceiveNotReady(const Frame&) override;

        void disconnect();
        void processRetransmissionOverload();
        bool wasRetransmissionOverload() const;
        void processTransmissionError(int framesCount);
        void processRetransmissionOnTineout();

    private:
        bool wasDisconnectSent = false;
        bool wasRRSent = false;
        int maxRetransmissionCount;
        int retransmissionCount = 0;
    };
}