#ifndef PARTICLES_H
#define PARTICLES_H

#include "pattern.h"
#include "graphics/state.h"

#define MAX_PARTICLES 16

struct particle{
    float location;
    float velocity;
};

class Particles : public Pattern
{
public:
    Particles();
    void fill(uint8_t *output, long dt, State &state);

private:
    particle particles[MAX_PARTICLES];
    void updateParticles(long dt, State &state);
};

#endif