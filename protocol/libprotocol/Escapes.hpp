#pragma once

#include <etl/vector.h>
#include <etl/span.h>

#include <libprotocol/Byte.hpp>

namespace protocol
{
    struct EscapeConfig
    {
        Byte escapeChar;
        etl::span<const Byte> escapedCharacters;
    };

    bool addEscapes(etl::span<const Byte> input, etl::ivector<Byte>& output, const EscapeConfig& config);
    bool removeEscapes(etl::span<const Byte> input, etl::ivector<Byte>& output, const EscapeConfig& config);
    etl::span<Byte>::iterator findFlag(etl::span<Byte>, Byte flag, Byte escape);
}