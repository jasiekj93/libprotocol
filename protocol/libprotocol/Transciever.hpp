#pragma once

/**
 * @file Transciever.hpp
 * @author Adrian Szczepanski
 * @date 22-05-2025
 */

#include <etl/span.h>

#include <libprotocol/Byte.hpp>

namespace protocol
{
    class Transciever
    {
    public:
        virtual ~Transciever() {}

        virtual void send(etl::span<const Byte>) = 0;
    };
}