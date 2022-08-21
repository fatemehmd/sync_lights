#ifndef PATTERNGENERATOR_H
#define PATTERNGENERATOR_H

#include "patterns/pattern.h"
#include "patterns/cloud.h"
#include "patterns/scarf.h"
#include "patterns/variablepulse.h"
#include "patterns/rainbowblast.h"
#include "patterns/sparkle.h"
#include "patterns/testpattern.h"
#include "patterns/piano.h"
#include "patterns/fire2012.h"
#include "patterns/particles.h"
#include "patterns/cleave.h"
#include "patterns/strobe.h"
#include "patterns/analogfeedback.h"


class PatternGenerator
{
public:
    PatternGenerator(){};
    ~PatternGenerator(){};
    static Pattern *makePattern(uint8_t idx);

private:

};

#endif