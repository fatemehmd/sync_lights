#include "sparkle.h"

Sparkle::Sparkle() : Pattern("Sparkle")
{
    pMetadata[0] = {NORMAL, 255};
    strcpy(pMetadata[0].label, "LVL");
    pMetadata[1] = {NORMAL, 64};
    strcpy(pMetadata[1].label, "FDE");
    pMetadata[2] = {NORMAL, 128};
    strcpy(pMetadata[2].label, "FRQ");
    pMetadata[3] = {NONE, 0};
    pMetadata[4] = {NONE, 0};
    pMetadata[5] = {NONE, 0};

    timeSinceLastFade = 0;
    timeSinceLastSpark = 0;

    fill_solid(_leds, STRAND_LENGTH, CRGB::Black);
}

void Sparkle::fill(uint8_t *outputs, long dt, State &state)
{
    int level = patternParam(state, 0);
    int fade = patternParam(state, 1);
    int freq = patternParam(state, 2);

    int msPerSpark = 2;
    int msPerFade = 50;

    timeSinceLastFade += dt;
    timeSinceLastSpark += dt;

    while (timeSinceLastSpark > msPerSpark)
    {
        int spot = random(STRAND_LENGTH * pow(1.03, (255 - freq)));

        if (spot < STRAND_LENGTH)
            heat[spot] = level;
        timeSinceLastSpark -= msPerSpark;
    }

    while (timeSinceLastFade > msPerFade)
    {
        for (int i = 0; i < STRAND_LENGTH; i++)
        {
            if (fade > 0)
                heat[i] = scale8(heat[i], 255 - fade);
            // fadeToBlack often goes ultra saturated at the end. Stop that.
            if (heat[i] < 10)
                heat[i] = 0;
        }
        timeSinceLastFade -= msPerFade;
    }

    for (int i = 0; i < STRAND_LENGTH; i++)
        outputs[i] = heat[i];
}