#include "Escapes.hpp"

using namespace protocol;

static bool isEscapedChar(Byte byte, const EscapeConfig& config)
{
    if(byte == config.escapeChar)
        return true;
    else
        return etl::find(config.escapedCharacters.begin(), 
            config.escapedCharacters.end(), 
            byte) != config.escapedCharacters.end();
}

bool protocol::addEscapes(etl::span<const Byte> input, etl::ivector<Byte>& output, const EscapeConfig &config)
{
    for(auto& byte : input)
    {
        if(output.available() < 2)
            return false;

        if (isEscapedChar(byte, config))
            output.push_back(config.escapeChar);

        output.push_back(byte);
    }

    return true;
}

bool protocol::removeEscapes(etl::span<const Byte> input, etl::ivector<Byte> &output, const EscapeConfig &config)
{
    bool escaped = false;

    for(auto& byte : input)
    {
        if (byte == config.escapeChar and not escaped)
        {
            escaped = true;
            continue;
        }

        if(output.full())
            return false;
        
        output.push_back(byte);
        escaped = false;
    }

    return true;
}
