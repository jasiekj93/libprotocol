#pragma once

/**
 * @file PhysicalLayer.hpp
 * @author Adrian Szczepanski
 * @date 20-05-2025
 */

#include <libprotocol/Frame.hpp>

namespace protocol::state
{
    class PhysicalLayer
    {
    public:
        virtual ~PhysicalLayer() = default;

        virtual void send(const Frame&) = 0;
        virtual void clearReceiveBuffer() = 0;
    };
}
    