#ifndef PIANO_H
#define PIANO_H

#include "pattern.h"
#include "graphics/state.h"

enum envelopePhase {REST, ATTACK, DECAY, SUSTAIN, RELEASE};

struct pianoKey {
    int intensity;
    int velocity;
    envelopePhase phase;
};

class Piano : public Pattern
{
public:
    Piano();
    void fill(uint8_t *outputs, long dt, State &state);

private:
    pianoKey keys[256];
    int timeSinceLastFrame;
    float _freqLut[128];

    uint8_t valueForNote(uint8_t note, float location, long t, State &state);
    void processEnvelopes(State &state);
};

#endif