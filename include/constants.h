#pragma once

struct Constants
{
    enum Sequence
    {
        Forward,
        Backward,
        Shuffle
    };

    enum Random
    {
        Stay,
        Linear
    };

    enum Round
    {
        Nearest,
        Up,
        Down
    };
};
