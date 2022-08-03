#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <Arduino.h>

#define PPQN 480
#define FPS_FRAMES 8
#define MAX_TAPS 10

class Transport
{
public:
    Transport(float bpm);
    void Reset();
    void Stop();
    void Play();
    void Scrub(uint16_t pulses);

    void SetBpm(float bpm);
    float Bpm();
    void TapTempo();

    uint8_t BeatInMeasure();

    uint16_t Update();
    float Fps();

private:
    float bpm;
    bool playing = true;
    uint32_t lastMicro;
    // accumulate microseconds that didn't make it into a whole pulse. This allows us to stay in sync
    uint32_t accumulatedMicro;
    uint16_t pulsesInMeasure;
    uint16_t manualPulses;

    uint32_t timing[FPS_FRAMES];
    uint8_t timingWritePointer = 0;

    uint32_t taps[MAX_TAPS];
    uint8_t tapsWritePointer = 0;
};

#endif