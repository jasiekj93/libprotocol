#pragma once

/**
 * @file ControllerImpl.hpp
 * @author Adrian Szczepanski
 * @date 15-05-2025
 */

#include <etl/map.h>
#include <etl/vector.h>
#include <etl/deque.h>

#include <libprotocol/Port.hpp>
#include <libprotocol/Packet.hpp>
#include <libprotocol/Observer.hpp>
#include <libprotocol/state/Controller.hpp>
#include <libprotocol/state/StatusSender.hpp>
#include <libprotocol/state/PhysicalLayer.hpp>
#include <libprotocol/state/Timer.hpp>
#include <libprotocol/state/Base.hpp>

namespace protocol::state
{
    class ControllerImpl : public Controller, public StatusSender, public Timer
    {
    public:
        ControllerImpl(PhysicalLayer&,
            Observer&,
            etl::span<Byte> sendPool,
            etl::span<Byte> receivePool,
            const Timeouts& = Timeouts(),
            int maxFrameCount = Port::MAX_FRAME_COUNT); 

        bool enqueuePacket(const Packet&);

        virtual void processFrame(const Frame&);
        void incrementTimer();

        //Controller
        void setState(state::Type type, bool wasError = false) override;
        inline state::Type getState() const override { return currentState->getType(); }

        inline void clearInputBuffer() override { physical.clearReceiveBuffer(); }
        void clearFramesAndIds() override;
        void resetFramesAndIds() override;

        void pushAndSendNextData() override;
        void sendEnqueuedFrames() override;
        bool popConfirmedSentFrames(Frame::Control::Id) override;

        inline int framesToSendCount() const override { return framesToSend.size(); }
        inline bool hasFramesToSend() const override { return not framesToSend.empty(); }
        inline bool hasDataToSend() const override { return not sendBuffer.empty(); }

        inline void notifyAllDataSent() override { return observer.allDataSentCallback(); }

        bool enqueueReceivedFrames() override;
        bool tryEnqueueFrame(const Frame& frame) override;
        
        void analyzeForPacket() override;

        //StatusSender
        void sendReceiveStatus() override;
        void sendConnect() override;
        void sendDisconnect() override;
        void sendConnectAck() override;

        //Timer
        void start() override;
        void stop() override;
        void pause() override;
        inline Timeouts getTimeouts() const override { return timeouts; }

    protected:
        inline auto getCurrentState() const { return currentState; }

    private:
        PhysicalLayer& physical;
        Observer& observer;
        etl::vector_ext<Byte> sendBuffer;
        etl::vector_ext<Byte> receiveBuffer;

        etl::map<state::Type, std::unique_ptr<Base>, state::TYPE_COUNT> states;
        Base* currentState;

        etl::deque<Frame, Port::MAX_FRAME_COUNT> framesToSend;
        etl::deque<Frame, Port::MAX_FRAME_COUNT> receivedFrames;

        int maxFrameCount; 
        Frame::Control::Id id;
        bool isBusy = false;

        Timeouts timeouts;
        int timerTick = 0;
        bool isTimerStarted = false;
    };
}