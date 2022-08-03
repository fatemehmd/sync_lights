#include "transport.h"

Transport::Transport(float _bpm) : bpm(_bpm)
{
    Reset();
}

void Transport::Reset()
{
    lastMicro = 0;
    accumulatedMicro = 0;
    manualPulses = 0;
}

void Transport::Play()
{
    playing = true;
}

void Transport::Stop()
{
    playing = false;
}

void Transport::SetBpm(float _bpm)
{
    bpm = constrain(_bpm, 60, 180);
}

float Transport::Bpm()
{
    return bpm;
}

void Transport::TapTempo()
{
    taps[tapsWritePointer++] = millis();
    tapsWritePointer %= MAX_TAPS;

    uint8_t validTaps = 0;
    u_int32_t accu = 0;
    for (int i = 0; i < MAX_TAPS; i++)
    {
        uint32_t gap = taps[(i + 1) % MAX_TAPS] - taps[i];
        if (300 < gap && gap < 1200) // only count tap pairs between 300ms and 1200ms apart.
        {
            validTaps++;
            accu += gap;
        }
    }
    if (validTaps > 4)
        SetBpm((int)(2 * 60000. * validTaps / accu) * 0.5);
}

uint8_t Transport::BeatInMeasure()
{
    return pulsesInMeasure / PPQN;
}

void Transport::Scrub(uint16_t pulses)
{
    manualPulses += pulses;
}

float Transport::Fps()
{
    uint32_t accum = 0;
    for (int i = 0; i < FPS_FRAMES; i++)
        accum += timing[i];

    return 1000000. * FPS_FRAMES / accum;
}

u_int16_t Transport::Update()
{
    unsigned long nowMicro = micros();
    unsigned delta = nowMicro - lastMicro; // this should handle overflow
    lastMicro = nowMicro;

    timing[timingWritePointer++] = delta;
    timingWritePointer %= FPS_FRAMES;

    if (!playing)
        delta = 0;

    delta += accumulatedMicro;

    float microsPerPulse = 60000000 / (bpm * PPQN);

    uint16_t wholePulses = delta / microsPerPulse;
    accumulatedMicro = delta - wholePulses * microsPerPulse;

    wholePulses += manualPulses;
    manualPulses = 0;

    pulsesInMeasure += wholePulses;
    pulsesInMeasure %= 4 * PPQN;

    return wholePulses;
}