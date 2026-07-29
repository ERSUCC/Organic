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
        Step,
        Linear
    };

    enum Round
    {
        Nearest,
        Up,
        Down
    };
};
