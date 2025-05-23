#pragma once

/**
 * @file ControllerImplDebug.hpp
 * @author Adrian Szczepanski
 * @date 15-05-2025
 */

#include <libprotocol/DebugObserver.hpp>
#include <libprotocol/state/ControllerImpl.hpp>

namespace protocol::state
{
    class ControllerImplDebug : public ControllerImpl
    {
    public:
        ControllerImplDebug(DebugObserver&,
            PhysicalLayer&,
            Observer&,
            etl::span<Byte> sendPool,
            etl::span<Byte> receivePool,
            const Timeouts& = Timeouts(),
            int maxFrameCount = Port::MAX_FRAME_COUNT); 

        void processFrame(const Frame&) override;

        //Controller
        void setState(state::Type type, bool wasError = false) override;
        void notifyAllDataSent() override;

    private:
        DebugObserver& debugObserver;
    };
}