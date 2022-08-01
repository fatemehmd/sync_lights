#include "particles.h"

Particles::Particles() : Pattern("Particles")
{
    pMetadata[0] = {NORMAL, 128};
    pMetadata[1] = {OCTAVE, 96};
    strcpy(pMetadata[1].label, "SPD");
    pMetadata[2] = {NORMAL, 64};
    strcpy(pMetadata[2].label, "WTH");
    pMetadata[3] = {NORMAL, 24};
    strcpy(pMetadata[3].label, "NUM");
    pMetadata[4] = {NORMAL, 128};
    strcpy(pMetadata[4].label, "GVT");

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].location = random8(STRAND_LENGTH);
        particles[i].velocity = 0;
    }
}

void Particles::updateParticles(long dt, State &state)
{
    float scale = 144; // 144 LEDs pm
    float delta = octave(state, 1) * dt / 1000.0;
    float gravity = -9.8 * patternParam(state, 4) / 128.0;

    int numParticles = constrain(patternParam(state, 3) / 8, 0, MAX_PARTICLES);

    // Physics sim
    for (int pIdx = 0; pIdx < numParticles; pIdx++)
    {
        particles[pIdx].location += particles[pIdx].velocity * delta * scale;
        particles[pIdx].velocity += gravity * delta;

        if (particles[pIdx].location < 0)
        {
            particles[pIdx].velocity = -particles[pIdx].velocity * .97;
            // particles[pIdx].location = -particles[pIdx].location;
        }

        // a kick!
        if (particles[pIdx].location < 5 && abs(particles[pIdx].velocity) < 0.02)
        {
            particles[pIdx].velocity = 5;
        }
    }
}

void Particles::fill(uint8_t *output, long dt, State &state)
{
    int width = patternParam(state, 2) / 16;
    int numParticles = constrain(patternParam(state, 3) / 8, 0, MAX_PARTICLES);

    updateParticles(dt, state);

    for (int i = 0; i < STRAND_LENGTH; i++)
    {
        int intensity = 0;
        for (int particleIndex = 0; particleIndex < numParticles; particleIndex++)
        {
            float dist = abs(i - particles[particleIndex].location);
            intensity += map(max(width - dist, 0.0f) * 10, 0, 10 * width, 0, 128);
        }
        int value = constrain(intensity, 0, 255);
        output[i] = value;
    }
}