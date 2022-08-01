#include "state.h"

State::State()
{
    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        setSelectedPattern(layerIdx, layerIdx, true);
        setLayerParam(LayerParams::MixOpacity, layerIdx, 0);
    }

    setSelectedPattern(0, 3, true);
    setLayerParam(LayerParams::MixOpacity, 0, 255);
    setSelectedPattern(1, 4, true);
    setLayerParam(LayerParams::MixOpacity, 1, 255);

    for (int stripIdx = 0; stripIdx < NUM_STRIPS; stripIdx++)
    {
        setPhysicalStripParam(PhysicalStripParams::ColorOrder, stripIdx, GRB);
        setPhysicalStripParam(PhysicalStripParams::Length, stripIdx, 60);
        setPhysicalStripParam(PhysicalStripParams::Direction, stripIdx, 0); // FWD
    }

    setGlobalParam(GlobalParams::MasterOpacity, 255);
    setGlobalParam(GlobalParams::FXOpacity, 0);

    paramMetadata colorParam[NUM_PARAMS_SIX];
    colorParam[0] = {CIRCULAR, 240, "HUE"};
    colorParam[1] = {NORMAL, 183, "SPD"};
    colorParam[2] = {NORMAL, 255, "SAT"};
    colorParam[3] = {NORMAL, 255, "RNG"};
    colorParam[4] = {NORMAL, 0, "BLK"};

    paramMetadata lfoParam[NUM_PARAMS_SIX];
    lfoParam[0] = {OCTAVE_SLOW, 96, "MUL"};
    lfoParam[1] = {NORMAL, 255, "DPT"};
    lfoParam[2] = {DISCRETE, 0, "WAV"};
    lfoParam[3] = {DISCRETE, 0, "TRG"};
    lfoParam[4] = {DISCRETE, 224, "PRM"};

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        registerParamBank(paramBankIdx(layerIdx, UISections::Color), "Color", colorParam);
        registerParamBank(paramBankIdx(layerIdx, UISections::LFO), "LFO", lfoParam);
    }

    resetTouched();

#ifdef HW_STANDARD
    enterGreenMode();
#else
    // loadScene(0);
#endif
}

void State::updateUISelectedField(int8_t amount)
{
    switch (_uiSelectedFieldIdx)
    {
    case 0: // general
        switch (_uiSelectedPageIdx)
        {
        case 0 ... 3: // layers
            (amount > 0) ? nextPattern(_uiSelectedPageIdx) : prevPattern(_uiSelectedPageIdx);
            break;
        case 4: // fx
            _uiSelectedFXIndex = addmod8(_uiSelectedFXIndex, (amount > 0) ? 1 : _fxCount - 1, _fxCount);
            break;
        default:
            break;
        }
        break;
    case 1 ... 6: // one of the 6 general knobs
    {
        uint8_t paramIdx = _uiSelectedFieldIdx - 1;
        tryChangeVisibleParam(paramIdx, amount);
        break;
    }
    break;
    case 7:
        switch (_uiSelectedPageIdx)
        {
        case 0 ... 3:
        {
            int currentOpacity = layerParam(LayerParams::MixOpacity, _uiSelectedPageIdx);
            setLayerParam(LayerParams::MixOpacity, _uiSelectedPageIdx, constrain(currentOpacity + amount, 0, 255));
        }
        break;
        case 4: // fx
        {
            int currentOpacity = globalParam(GlobalParams::FXOpacity);
            setGlobalParam(GlobalParams::FXOpacity, constrain(currentOpacity + amount, 0, 255));
        }
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void State::prevLayer()
{
    _uiSelectedPageIdx = addmod8(_uiSelectedPageIdx, NUM_LAYERS + 1 - 1, NUM_LAYERS + 1); // global + param
    _uiSelectedFieldIdx = 0;
}

UISections State::uiSelectedSectionIdx()
{
    return static_cast<UISections>(_uiSelectedSectionIdx);
}

void State::setUISelectedSectionIdx(UISections sectionIdx)
{
    _uiSelectedSectionIdx = static_cast<uint8_t>(sectionIdx);
}

uint8_t State::paramBankIdx(uint8_t layerIdx, UISections section)
{
    return LAYER_PARAM_BANKS_OFFSET + layerIdx * NUM_SECTIONS + static_cast<uint8_t>(section);
}

uint8_t State::visibleParamBank()
{
    switch (_uiSelectedPageIdx)
    {
    case 0 ... 3:
        return paramBankIdx(_uiSelectedPageIdx, uiSelectedSectionIdx());
    case 4:
        return FX_INDEX_OFFSET + _uiSelectedFXIndex;
    default:
        return 0; // should never
    }
}

uint8_t State::visibleParam(int idx)
{
    return _paramBanks[visibleParamBank()].params[idx];
}

paramType State::visibleParamType(int idx)
{
    return _paramBanks[visibleParamBank()].metadata[idx].type;
}

char *State::visibleParamLabel(int idx)
{
    return _paramBanks[visibleParamBank()].metadata[idx].label;
}

const char *State::visibleParamBankLabel()
{
    return _paramBanks[visibleParamBank()].label;
}

void State::advance(uint16_t pulses)
{
    updateDyanmicSignals(pulses);
    updateLFOs(pulses);
}

void State::registerGenerator(uint8_t layerIdx, const char *label, paramMetadata *params, bool setDefaults)
{
    int bankIdx = paramBankIdx(layerIdx, UISections::Main);
    registerParamBank(bankIdx, label, params, setDefaults);
}

void State::registerParamBank(int bankIdx, const char *label, paramMetadata *params, bool setDefaults)
{
    _paramBanks[bankIdx].label = label;
    for (int i = 0; i < NUM_PARAMS_SIX; i++)
    {
        _paramBanks[bankIdx].metadata[i] = params[i];
        if (_paramBanks[bankIdx].metadata[i].type != NONE && setDefaults)
            _paramBanks[bankIdx].params[i] = params[i].defaultValue;
    }
}

int State::registerFX(const char *label, paramMetadata *params)
{
    int overallIndex = _fxCount + FX_INDEX_OFFSET;
    _fxCount++;

    registerParamBank(overallIndex, label, params, true);

    return overallIndex;
}

const char *State::getLayerPatternLabel(int layerIdx)
{
    return _paramBanks[paramBankIdx(layerIdx, UISections::Main)].label;
}

void State::setupPalette()
{
    for (int layerIdx = 0; layerIdx < 4; layerIdx++)
    {
        int idx = paramBankIdx(layerIdx, UISections::Color);
        uint8_t hueStart = _paramBanks[idx].params[0];
        int hueSpan = _paramBanks[idx].params[1] - 127;
        uint8_t sat = _paramBanks[idx].params[2];

        int16_t hue2 = hueStart + 256 + hueSpan * 2 / 3.;
        int16_t hue3 = hueStart + 256 + hueSpan * 4 / 3.;
        int16_t hue4 = hueStart + 256 + hueSpan * 2;

        CHSV c1 = CHSV(hueStart, sat, 255);
        CHSV c2 = CHSV(hue2 % 256, sat, 255);
        CHSV c3 = CHSV(hue3 % 256, sat, 255);
        CHSV c4 = CHSV(hue4 % 256, sat, 255);

        _palette[layerIdx] = CHSVPalette16(c4, c3, c2, c1);
    }
}

CRGB State::getColor(uint8_t layerIdx, uint8_t colIdx)
{
    int idx = paramBankIdx(layerIdx, UISections::Color);
    uint8_t dynamicRange = patternParam(3, idx);
    float slope = 1 + patternParam(4, idx) / 50.;

    uint8_t cIndex = colIdx;
    // add black to the bottom
    cIndex = constrain(slope * (int16_t)cIndex - 256 * (slope - 1), 0, 255);
    // avoid wrap around behaviour
    cIndex = map8(cIndex, 0, 240);
    return ColorFromPalette(this->palette(layerIdx), cIndex, lerp8by8(255, cIndex, dynamicRange)); // not sure this is right. too bright?
}

CHSVPalette16 State::palette(int layerIdx)
{
    return _palette[layerIdx];
}

void State::tryChangePatternParam(int bgIdx, int paramIdx, int amount)
{
    int curValue = _paramBanks[bgIdx].params[paramIdx];

    _paramLastTouch[paramIdx] = millis();

    switch (_paramBanks[bgIdx].metadata[paramIdx].type)
    {
    case CIRCULAR:
        _paramBanks[bgIdx].params[paramIdx] += amount;
        break;
    case NORMAL:
        _paramBanks[bgIdx].params[paramIdx] = constrain(curValue + amount, 0, 255);
        break;
    case OCTAVE:
    case OCTAVE_SLOW:
        if (amount > 0)
            _paramBanks[bgIdx].params[paramIdx] = constrain(curValue + 32, 0, 224);
        else
            _paramBanks[bgIdx].params[paramIdx] = constrain(curValue - 32, 0, 224);
        break;
    case DISCRETE:
        if (amount > 0)
            _paramBanks[bgIdx].params[paramIdx] += 32;
        else
            _paramBanks[bgIdx].params[paramIdx] -= 32;
        break;
    case NONE:
        break;
    }
}

void State::tryChangeVisibleParam(int idx, int amount)
{
    tryChangePatternParam(visibleParamBank(), idx, amount);
}

void State::nextPattern(int layerIdx)
{
    uint8_t newPatternIdx = addmod8(layerParam(LayerParams::SelectedPattern, layerIdx), 1, TMP_NUM_PATTERNS);
    setSelectedPattern(layerIdx, newPatternIdx, true);
    resetTouched();
}

void State::prevPattern(int layerIdx)
{
    uint8_t newPatternIdx = addmod8(layerParam(LayerParams::SelectedPattern, layerIdx), TMP_NUM_PATTERNS - 1, TMP_NUM_PATTERNS);
    setSelectedPattern(layerIdx, newPatternIdx, true);
    resetTouched();
}

void State::setSelectedPattern(uint8_t layerIdx, uint8_t patternIdx, bool setDefaults)
{
    // corresponds with render loading behaviour
    setLayerParam(LayerParams::SelectedPattern, layerIdx, patternIdx | (setDefaults ? 0x40 : 0x80));
}

uint8_t State::layerParam(LayerParams param, uint8_t layerIdx)
{
    return _layerParams[layerIdx][(uint8_t)param];
}

void State::setLayerParam(LayerParams param, uint8_t layerIdx, uint8_t value)
{
    _layerParams[layerIdx][(uint8_t)param] = value;
}

void State::toggleSolo(uint8_t layerIdx)
{
    uint8_t current = layerParam(LayerParams::Solo, layerIdx);

    for (int i = 0; i < NUM_LAYERS; i++)
    {
        setLayerParam(LayerParams::Solo, i, 0);
    }

    if (current == 0)
        setLayerParam(LayerParams::Solo, layerIdx, 1);
}

bool State::anySolo()
{
    bool any = false;
    for (int i = 0; i < NUM_LAYERS; i++)
    {
        if (layerParam(LayerParams::Solo, i) > 0)
            any = true;
    }
    return any;
}

uint8_t State::physicalStripParam(PhysicalStripParams param, uint8_t stripIdx)
{
    return _stripParams[stripIdx][(uint8_t)param];
}

void State::setPhysicalStripParam(PhysicalStripParams param, uint8_t stripIdx, uint8_t value)
{
    _stripParams[stripIdx][(uint8_t)param] = value;
}

uint8_t State::globalParam(GlobalParams param)
{
    return _globalParams[(uint8_t)param];
}

void State::setGlobalParam(GlobalParams param, uint8_t value)
{
    _globalParams[(uint8_t)param] = value;
}

Scene State::currentScene()
{
    Scene scene;

    for (int i = 0; i < NUM_GLOBAL_PARAMS; i++)
        scene.globalParams[i] = _globalParams[i];

    for (int i = 0; i < NUM_PARAM_BANKS; i++)
        for (int j = 0; j < NUM_PARAMS_SIX; j++)
            scene.paramBanks[i][j] = _paramBanks[i].params[j];

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
        for (int i = 0; i < NUM_LAYER_PARAMS; i++)
            scene.layerParams[layerIdx][i] = _layerParams[layerIdx][i];

    for (int snapshotIdx = 0; snapshotIdx < NUM_SNAPSHOTS; snapshotIdx++)
        scene.snapshots[snapshotIdx] = _snapshots[snapshotIdx];

    return scene;
}

Snapshot State::currentSnapshot()
{
    Snapshot snapshot;

    for (int bankIdx = 0; bankIdx < NUM_PARAM_BANKS; bankIdx++)
        for (int i = 0; i < NUM_PARAMS_SIX; i++)
            snapshot.paramBanks[bankIdx][i] = _paramBanks[bankIdx].params[i];
    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
        snapshot.levels[layerIdx] = layerParam(LayerParams::MixOpacity, layerIdx);
    snapshot.levels[NUM_LAYERS + 0] = globalParam(GlobalParams::MasterOpacity);
    snapshot.levels[NUM_LAYERS + 1] = globalParam(GlobalParams::FXOpacity);

    return snapshot;
}

void State::setSnapshot(Snapshot &snapshot)
{

    for (int bankIdx = 0; bankIdx < NUM_PARAM_BANKS; bankIdx++)
        for (int i = 0; i < NUM_PARAMS_SIX; i++)
            _paramBanks[bankIdx].params[i] = snapshot.paramBanks[bankIdx][i];
    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
        setLayerParam(LayerParams::MixOpacity, layerIdx, snapshot.levels[layerIdx]);
    setGlobalParam(GlobalParams::MasterOpacity, snapshot.levels[NUM_LAYERS + 0]);
    setGlobalParam(GlobalParams::FXOpacity, snapshot.levels[NUM_LAYERS + 1]);
}

void State::loadSnapshot(uint8_t idx)
{
    setSnapshot(_snapshots[idx]);
}

void State::syncSnapshot()
{
    bool crossFaderAtHome = ((_currentSnapshot) == 0 && (_crossFader == 0)) ||
                            ((_currentSnapshot) == 1 && (_crossFader == 127 * 2));
    if (crossFaderAtHome)
        _snapshots[_currentSnapshot] = currentSnapshot();
}

uint8_t State::currentSnapshotIdx()
{
    return _currentSnapshot;
}

uint8_t State::paramRecentlyTouched(int idx)
{
    switch (millis() - _paramLastTouch[idx])
    {
    case 0 ... 999:
        return 6;
    case 1000 ... 1199:
        return 5;
    case 1200 ... 1399:
        return 4;
    case 1400 ... 1599:
        return 3;
    case 1600 ... 1799:
        return 2;
    case 1800 ... 1999:
        return 1;
    default:
        return 0;
    }
    return millis() - _paramLastTouch[idx] < 2000;
}

int State::uiSelectedPageIndex()
{
    return _uiSelectedPageIdx;
}

void State::setUISelectedPageIndex(int value)
{
    if (_uiSelectedPageIdx != value)
    {
        _uiSelectedPageIdx = value;
        _uiSelectedSectionIdx = 0;
        resetTouched();
    }
}

void State::resetTouched()
{
    for (int i = 0; i < NUM_PARAMS_SIX; i++)
        _paramLastTouch[i] = -10000;
}

uint8_t State::patternParam(uint8_t paramIdx, uint8_t bankIdx)
{
    return _paramBanks[bankIdx].params[paramIdx];
}

uint8_t State::effectiveParam(uint8_t layerIdx, uint8_t paramIdx)
{
    int16_t paramValue = _paramBanks[paramBankIdx(layerIdx, UISections::Main)].params[paramIdx];
    int lfoBankIdx = paramBankIdx(layerIdx, UISections::LFO);
    if (paramIdx == discreteParam(lfoBankIdx, 4))
        paramValue += lfoValue(layerIdx) - 127;
    return constrain(paramValue, 0, 255);
}

float State::octaveParam(uint8_t bankIdx, uint8_t paramIdx)
{
    switch (_paramBanks[bankIdx].params[paramIdx])
    {
    case 0 ... 31:
        return 1 / 8.0;
    case 32 ... 63:
        return 1 / 4.0;
    case 64 ... 95:
        return 1 / 2.0;
    case 96 ... 127:
        return 1;
    case 128 ... 159:
        return 2;
    case 160 ... 191:
        return 4;
    case 192 ... 223:
        return 8;
    case 224 ... 255:
        return 16;
    }
}

uint8_t State::discreteParam(uint8_t bankIdx, uint8_t paramIdx)
{
    return _paramBanks[bankIdx].params[paramIdx] / 32;
}

int State::uiSelectedFieldIndex()
{
    return _uiSelectedFieldIdx;
}

uint8_t State::crossFader()
{
    return _crossFader;
}

void State::setCrossFader(uint8_t value)
{
    _crossFader = value;

    Snapshot interpolated;

    for (int bankIdx = 0; bankIdx < NUM_PARAM_BANKS; bankIdx++)
    {
        for (int paramIdx = 0; paramIdx < NUM_PARAMS_SIX; paramIdx++)
            interpolated.paramBanks[bankIdx][paramIdx] =
                lerp8by8(_snapshots[0].paramBanks[bankIdx][paramIdx],
                         _snapshots[1].paramBanks[bankIdx][paramIdx],
                         value);
    }

    for (int i = 0; i < NUM_LAYERS + 2; i++)
        interpolated.levels[i] = lerp8by8(_snapshots[0].levels[i], _snapshots[1].levels[i], value);

    setSnapshot(interpolated);
}

void State::pinSnapshot(uint8_t snapshotIdx)
{
    _snapshots[snapshotIdx] = currentSnapshot();
}

uint8_t State::dynamicSignal(uint8_t sigIdx)
{
    return _signal[sigIdx].intensity;
}

void State::triggerDynamicSignal(uint8_t sigIdx, bool on, uint8_t velocity)
{
    if (on)
    {
        _signal[sigIdx].velocity = velocity;
        _signal[sigIdx].phase = EnvelopePhase::ATTACK;
    }
    else
    {
        _signal[sigIdx].velocity = 0;
        _signal[sigIdx].phase = EnvelopePhase::RELEASE;
    }
}

void State::updateDyanmicSignals(uint32_t dt)
{
    float attackRate = 1;
    float releaseRate = 0.25;
    for (uint8_t sigIdx = 0; sigIdx < NUM_SIGNALS; sigIdx++)
    {
        switch (_signal[sigIdx].phase)
        {
        case EnvelopePhase::ATTACK:
            _signal[sigIdx].intensity = constrain(_signal[sigIdx].intensity + dt * attackRate, 0, _signal[sigIdx].velocity);
            break;
        case EnvelopePhase::RELEASE:
            _signal[sigIdx].intensity = constrain(_signal[sigIdx].intensity - dt * releaseRate, 0, 255);
            break;
        default:
            break;
        }
    }
}

uint8_t State::lfoValue(uint8_t layerIdx)
{
    return _lfoValue[layerIdx];
}

void State::triggerLFO(uint8_t layerIdx)
{
    _lfoTrigTime[layerIdx] = 0;
}

void State::updateLFOs(uint32_t pulses)
{
    _lfoFreeTrigTime += pulses;

    if (_lfoFreeTrigTime > PPQN * 256) // 256 notes
        _lfoFreeTrigTime -= PPQN * 256;

    for (int layerIdx = 0; layerIdx < NUM_LAYERS; layerIdx++)
    {
        int bankIdx = paramBankIdx(layerIdx, UISections::LFO);
        float mult = octaveParam(bankIdx, 0) / (PPQN * 16);
        float depth = _paramBanks[bankIdx].params[1] / 255.;
        int waveform = discreteParam(bankIdx, 2);
        int trigMode = discreteParam(bankIdx, 3);

        if (trigMode > 0 && _lfoTrigTime[layerIdx] != UINT16_MAX)
            _lfoTrigTime[layerIdx] += pulses;

        float x;

        if (trigMode == 0)
            x = (_lfoFreeTrigTime * mult) - floor(_lfoFreeTrigTime * mult);
        else
        {
            if (_lfoTrigTime[layerIdx] > 1. / mult)
            {
                _lfoTrigTime[layerIdx] = UINT16_MAX;
                x = 0;
            }
            else
                x = _lfoTrigTime[layerIdx] * mult;
        }

        int8_t val = 0;

        switch (waveform)
        {
        case 0: // sine
            val = sin(x * 2 * PI) * 127 * depth;
            break;
        case 1: // square
            val = (x < 0.5) ? 128 * depth : -127 * depth;
            break;
        case 2: // triangle
            if (x < 0.25)
                val = x * 2 * depth * 256;
            else if (x < 0.75)
                val = (0.5 - x) * 2 * depth * 256;
            else
                val = (x - 1.0) * 2 * depth * 256;
            break;
        case 3: // saw, rise
            if (x < 0.5)
                val = x * depth * 256;
            else
                val = (x - 1) * depth * 256;
            break;
        case 4: // saw, fall
            if (x < 0.5)
                val = -x * depth * 256;
            else
                val = (1 - x) * depth * 256;
            break;
        case 5: // exp decay
            val = exp(-5 * x) * depth * 128;
            break;
        case 6: // exp reversed
            val = exp(-5 * (1 - x)) * depth * 128;
            break;
        default:
            val = 0;
        }

        if (trigMode > 0 && _lfoTrigTime[layerIdx] == UINT16_MAX)
            val = 0;

        _lfoValue[layerIdx] = val + 127;
    }
}
