#pragma once

namespace tuc
{

    template <typename Output, typename Input, typename MapFunction>
    Output map(Input const& input, MapFunction function)
    {
        Output output;
        std::transform(input.begin(), input.end(), std::back_inserter(output), function);
        return output;
    }

}