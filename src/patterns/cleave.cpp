#include "cleave.h"

Cleave::Cleave() : Pattern("Cleave")
{
    pMetadata[0] = {NORMAL, 255, "LVL"};
    pMetadata[1] = {NORMAL, 192, "FDE"};
    pMetadata[2] = {OCTAVE, 96, "SPD"};

    accRepeats = 0;
}

void Cleave::fill(uint8_t *outputs, long dt, State &state)
{
    int octaves = (patternParam(state, 2)) / 32 + 1;
    int level = patternParam(state, 0);
    accRepeats += dt * octaves / 16.0;

    bool triggerd = (state.dynamicSignal(0) > 32);

    while (accRepeats > 1)
    {
        if (triggerd)
        {
            values[0] = level;
        }
        else
            values[0] = scale8(values[1], 255 - patternParam(state, 1));

        for (int i = STRAND_LENGTH - 1; i > 0; i--)
        {
            values[i] = values[i - 1];
        }
        accRepeats -= 1;
    }

    for (int i = 0; i < STRAND_LENGTH; i++)
        outputs[i] = values[i];
}