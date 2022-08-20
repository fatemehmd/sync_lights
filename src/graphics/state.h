    #ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <FastLED.h>

#define STRAND_LENGTH 144


#define NUM_PARAMS_SIX 6

#define FX_INDEX_OFFSET 16
#define LAYER_PARAM_BANKS_OFFSET 0

#define TMP_NUM_PATTERNS 11

#define NUM_PARAM_BANKS 20

#define NUM_LAYERS 4
#define NUM_LAYER_PARAMS 8

#define NUM_SECTIONS 3

#define NUM_STRIPS 1
#define NUM_STRIP_PARAMS 8
#define ACTIVE_STRIP_PARAMS 3

#define NUM_GLOBAL_PARAMS 8

#define NUM_SNAPSHOTS 2

#define MIN_STRIP_LENGTH 8
#define MAX_STRIP_LENGTH 144

#define NUM_SIGNALS 2

#ifndef PPQN
#define PPQN 480
#endif

enum paramType
{
    NONE,
    NORMAL,
    CIRCULAR,
    OCTAVE,
    OCTAVE_SLOW,
    DISCRETE
};

struct paramMetadata
{
    paramType type;
    uint8_t defaultValue;
    char label[4] = {};

    paramMetadata() : type(NONE), defaultValue(0), label{} {}
    paramMetadata(paramType _type, uint8_t _defaultValue) : type(_type), defaultValue(_defaultValue), label{0, 0, 0, 0} {}
    paramMetadata(paramType _type, uint8_t _defaultValue, const char *_label) : type(_type), defaultValue(_defaultValue)
    {
        strcpy(label, _label);
    }
};

typedef struct ParamBank
{
    const char *label;
    uint8_t params[NUM_PARAMS_SIX];
    paramMetadata metadata[NUM_PARAMS_SIX];
} ParamBank;

enum class LayerParams : uint8_t
{
    // first two bits indicate whether to load, and whether to load default values
    // next six bits indicate index of pattern to load
    SelectedPattern = 0,
    MixOpacity = 1,
    Bypass = 2,
    Solo = 3
};

enum class PhysicalStripParams
{
    ColorOrder = 0,
    Length = 1,
    Direction = 2
};

enum class GlobalParams
{
    MasterOpacity = 0,
    FXOpacity = 1,
    PreviewNode = 2,
    FrameDelay = 7
};

enum class EnvelopePhase
{
    REST = 0,
    ATTACK = 1,
    DECAY = 2,
    SUSTAIN = 3,
    RELEASE = 4
};

enum class UISections
{
    Main = 0,
    Color = 1,
    LFO = 2
};

struct Signal
{
    uint8_t intensity;
    uint8_t velocity;
    uint8_t note;
    EnvelopePhase phase;
};

typedef struct Snapshot
{
    uint8_t paramBanks[NUM_PARAM_BANKS][NUM_PARAMS_SIX];
    uint8_t levels[NUM_LAYERS + 2];
} Snapshot;

typedef struct Scene
{
    uint8_t globalParams[NUM_GLOBAL_PARAMS];
    uint8_t paramBanks[NUM_PARAM_BANKS][NUM_PARAMS_SIX];
    uint8_t layerParams[NUM_LAYERS][NUM_LAYER_PARAMS];
    
    Snapshot snapshots[NUM_SNAPSHOTS];
} Scene;

class State
{
public:

    int notes[256];

    // ctor and init fns
    State();

    int registerFX(const char *label, paramMetadata *params);
    void registerGenerator(uint8_t layerIdx, const char *label, paramMetadata *params, bool setDefaults = true);

    // ui related
    int uiSelectedPageIndex();
    int uiSelectedFieldIndex();

    UISections uiSelectedSectionIdx();
    void setUISelectedSectionIdx(UISections sectionIdx);

    void setUISelectedPageIndex(int value);
    void prevLayer();

    void updateUISelectedField(int8_t amount);


    // the six params
    uint8_t visibleParam(int idx);
    paramType visibleParamType(int idx);
    char *visibleParamLabel(int idx);
    void tryChangeVisibleParam(int idx, int amount);
    void changePatternParam(int layerIdx, int paramIdx, int amount);
    uint8_t paramRecentlyTouched(int idx);
    const char *visibleParamBankLabel();

    const char *getLayerPatternLabel(int layerIdx);

    // update to time state
    void advance(uint16_t pulses);

    void setupPalette();
    void setHue(int layerIdx, int hue);
    int getHue(int layerIdx);

    CRGB getColor(uint8_t layerIdx, uint8_t colIdx);
    CHSVPalette16 palette(int layerIdx);

    void nextPattern(int layerIdx);
    void prevPattern(int layerIdx);
    void setSelectedPattern(uint8_t layerIdx, uint8_t patternIdx, bool setDefaults);
    uint8_t getSelectedPattern(uint8_t layerIdx);

    uint8_t effectiveParam(uint8_t layerIdx, uint8_t paramIdx);

    uint8_t patternParam(uint8_t paramIdx, uint8_t bankIdx);

    uint8_t layerParam(LayerParams param, uint8_t layerIdx);
    void setLayerParam(LayerParams param, uint8_t layerIdx, uint8_t value);

    void toggleSolo(uint8_t layerIdx);
    bool anySolo();

    uint8_t physicalStripParam(PhysicalStripParams param, uint8_t stripIdx);
    void setPhysicalStripParam(PhysicalStripParams param, uint8_t stripIdx, uint8_t value);

    uint8_t globalParam(GlobalParams param);
    void setGlobalParam(GlobalParams param, uint8_t value);

    uint8_t crossFader();
    void setCrossFader(uint8_t value);

    void pinSnapshot(uint8_t snapshotIdx);

    Scene currentScene();
    Snapshot currentSnapshot();

    void loadSnapshot(uint8_t idx);
    void setSnapshot(Snapshot &snapshot);
    void syncSnapshot();

    void copyScene();
    void pasteScene();
    void setScene(Scene &scene);

    void nextScene();
    uint8_t currentSceneIdx();
    uint8_t currentSnapshotIdx();

    uint8_t dynamicSignal(uint8_t sigIdx);
    void triggerDynamicSignal(uint8_t sigIdx, bool on, uint8_t velocity);

    uint8_t lfoValue(uint8_t layerIdx);
    void triggerLFO(uint8_t layerIdx);
    

private:
    int _uiSelectedPageIdx = 0; // rename page to just layers, probably

    uint8_t _uiSelectedSectionIdx = 0;
    int _uiSelectedFXIndex = 0;
    // what's shown on the UI. 1 - 6 are params. 0 varies
    int _uiSelectedFieldIdx = 0;

    ParamBank _paramBanks[NUM_PARAM_BANKS];
    Snapshot _snapshots[NUM_SNAPSHOTS];

   
    uint8_t visibleParamBank();
    void registerParamBank(int bankIdx, const char *label, paramMetadata *params, bool setDefaults = true);
    void tryChangePatternParam(int bankIdx, int paramIdx, int amount);

    uint8_t paramBankIdx(uint8_t layerIdx, UISections section);

    float octaveParam(uint8_t bankIdx, uint8_t paramIdx);
    uint8_t discreteParam(uint8_t bankIdx, uint8_t paramIdx);

    uint8_t _layerParams[NUM_LAYERS][NUM_LAYER_PARAMS];
    uint8_t _stripParams[NUM_STRIPS][NUM_STRIP_PARAMS];
    uint8_t _globalParams[NUM_GLOBAL_PARAMS];
    uint8_t _crossFader;

    CHSVPalette16 _palette[NUM_LAYERS];

    long _paramLastTouch[NUM_PARAMS_SIX];

    void resetTouched();

    uint8_t _currentSnapshot = 0;

    int _fxCount = 0;

    Signal _signal[NUM_SIGNALS];

    void updateDyanmicSignals(uint32_t dt);

    uint16_t _lfoFreeTrigTime;
    uint16_t _lfoTrigTime[NUM_LAYERS];
    uint8_t _lfoValue[NUM_LAYERS];

    void updateLFOs(uint32_t pulses);
};

#endif
