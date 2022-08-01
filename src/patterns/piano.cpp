#include "piano.h"

Piano::Piano() : Pattern("Piano")
{
    pMetadata[0] = {NORMAL, 255};
    strcpy(pMetadata[0].label, "ATK");
    pMetadata[1] = {NORMAL, 128};
    strcpy(pMetadata[1].label, "DCY");
    pMetadata[2] = {NORMAL, 128};
    strcpy(pMetadata[2].label, "RLS");
    pMetadata[3] = {NORMAL, 127};
    pMetadata[4] = {OCTAVE, 96};
    pMetadata[5] = {NONE, 0};

    for (int i = 0; i < 256; i++)
    {
        keys[i] = {0, 0, REST};
    }

    timeSinceLastFrame = 0;

    for (int noteIdx = 0; noteIdx < 128; noteIdx++)
        _freqLut[noteIdx] = 440 * pow(2, (noteIdx - 69) / 12.); // 69 is middle C @ 440Hz
}

void Piano::processEnvelopes(State &state)
{
    int attackRate = patternParam(state, 0) * 0.5;
    int decayRate = patternParam(state, 1) * 1.25 / 20;
    float sustain = 0.5;
    int releaseRate = patternParam(state, 2) * 0.5;

    for (int i = 0; i < 256; i++)
    {
        switch (keys[i].phase)
        {
        case REST:
            if (state.notes[i] > 0)
            {
                keys[i].phase = ATTACK;
                keys[i].velocity = state.notes[i];
            }
            break;
        case ATTACK:
            if (keys[i].intensity < keys[i].velocity)
            {
                keys[i].intensity = constrain(keys[i].intensity + attackRate, 0, keys[i].velocity);
            }
            else
            {
                keys[i].phase = DECAY;
            }
            break;
        case DECAY:
            if (keys[i].intensity > keys[i].velocity * sustain)
            {
                keys[i].intensity = constrain(keys[i].intensity - decayRate, keys[i].velocity * sustain, 255);
            }
            else
            {
                keys[i].phase = SUSTAIN;
            }
            break;
        case SUSTAIN:
            if (state.notes[i] == 0)
            {
                keys[i].phase = RELEASE;
            }
            break;
        case RELEASE:
            if (state.notes[i] > 0)
            {
                keys[i].phase = ATTACK;
                keys[i].velocity = state.notes[i];
            }
            else if (keys[i].intensity > 0)
            {
                keys[i].intensity = constrain(keys[i].intensity - releaseRate, 0, 255);
            }
            else
            {
                keys[i].phase = REST;
                keys[i].velocity = 0;
            }
            break;
        }
    }
}

uint8_t Piano::valueForNote(uint8_t note, float location, long t, State &state)
{
    if (keys[note].intensity > 0)
    {
        float amplitude = patternParam(state, 3) / (1024. * 256);
        float octaveShift = octave(state, 4) / 32.; // 5 octaves built in

        float center = 0.2 + (note - 36) / 28.0;

        float freq = _freqLut[note] * octaveShift;

        uint8_t x = ((long)(t * freq / 1000. * 256)) % 256;

        float oscillation = (sin8(x) - 127) * amplitude;
        center += oscillation;

        float dropOff = constrain(0.3 - (note - 36) / 64.0, 0.05, 1);
        uint8_t value = keys[note].intensity * constrain(1 - abs(location - center) / dropOff, 0, 1);

        uint8_t hueIndex = (note % 12) / 12. * 255;

        return value; // (state.currentPalette, hueIndex, value);
    } 
    else
        return 0;
}

void Piano::fill(uint8_t *outputs, long dt, State &state)
{
    timeSinceLastFrame += dt;
    unsigned long t = millis();

    while (timeSinceLastFrame > 20)
    {
        processEnvelopes(state);
        timeSinceLastFrame -= 20;
    }

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        // int noteIdx = 36 + i / 10;
        // leds[i] = CHSV(0, 0, keys[noteIdx].intensity);

        // cool effect, but..
        // float dist = abs(i - STRAND_LENGTH / 2.) / 72.;
        // leds[i] = CHSV(0, 0,  255 * keys[36].intensity * dist);

        // collection of effects.

        // float mid = sin(t / 10) * 5 + 72.;
        // float kick = keys[36].intensity * constrain(1 - abs(i - mid) / 48., 0, 1);
        // float sideStick = keys[37].intensity * constrain(1 - abs(i - 108.) / 36., 0, 1);
        // float clap = keys[39].intensity * constrain(1 - abs(i - 36.) / 36., 0, 1);
        // float acousticSnare = keys[38].intensity * constrain(1 - abs(i % 16 - 8.) / 4., 0, 1);

        // if (kick > 32)
        //     kick += random8(8) - 4;

        outputs[i] = 0;

        // all the (drum?) pads on BeatStep's default config
        for (int noteIdx = 0; noteIdx < 128; noteIdx++)
            outputs[i] = constrain(outputs[i] + valueForNote(noteIdx, (float)i / STRAND_LENGTH, t, state), 0, 255);

        // leds[i] = colorForNote(36, (float) i / STRAND_LENGTH, state) +
        //           colorForNote(37, (float) i / STRAND_LENGTH, state) +
        //           CHSV(32 + state.globalParams[0], 192, clap) +
        //           // CHSV(64 + state.globalParams[0], 192, sideStick) +
        //           CHSV(96 + state.globalParams[0], 192, acousticSnare);
    }
}