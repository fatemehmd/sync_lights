#include "variablepulse.h"

VariablePulse::VariablePulse() : Pattern("V Pulse")
{
    pMetadata[0] = {NORMAL, 255};
    strcpy(pMetadata[0].label, "LVL");
    pMetadata[1] = {OCTAVE, 96};
    strcpy(pMetadata[1].label, "SPD");
    pMetadata[2] = {NORMAL, 30}; // period (s)
    strcpy(pMetadata[2].label, "PRD");
    pMetadata[3] = {NORMAL, 15}; // min width
    strcpy(pMetadata[3].label, "MIN");
    pMetadata[4] = {NORMAL, 255}; // max width
    strcpy(pMetadata[4].label, "MAX");
    pMetadata[5] = {NORMAL, 255}; // palette range
    strcpy(pMetadata[5].label, "RNG");
}

// return a cyclical (sine wave) value between min and max
float VariablePulse::cycle(float t, float period, float min, float max)
{
    return .5 * (min + max) - .5 * (max - min) * cos(t / period * (2 * PI));
}

void VariablePulse::fill(uint8_t *output, long dt, State &state)
{
    unsigned long t = millis(); // hack
    float clock = t / 1000.0 * octave(state, 1);

    float period = patternParam(state, 2); // s
    float peakedness = 3;
    float min_pulse_width = STRAND_LENGTH * constrain(patternParam(state, 3), 10, 255) / 100.0;
    float max_pulse_width = STRAND_LENGTH * patternParam(state, 4) / 100.0;
    float crawl_speed_factor = 1; // around 1 is the sweet spot; changing this too much seems to look much worse
    float min_brightness = .05;

    // cycle in the inverse space to balance short vs. long pulses better
    float pulse_width = 1. / cycle(clock, period, 1. / min_pulse_width, 1. / max_pulse_width);
    float crawl_offset = crawl_speed_factor * clock;
    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        float brightness = cycle(STRAND_LENGTH - i + crawl_offset * pulse_width, pulse_width, 0, 1);
        brightness = pow(brightness, peakedness);
        int value = constrain(brightness, min_brightness, 1) * 255; // brightness_to_value(brightness, min_brightness);

        value = constrain((value * 1.25) - 64, 0, 255);
        if (value > 8)
            output[i] = value;
        else
            output[i] = 0;

        output[i] = scale8(output[i], patternParam(state, 0));
    }
}