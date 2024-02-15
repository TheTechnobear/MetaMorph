#include "plugin.hpp"


struct Octave : Module {
    enum ParamId { P_OFFSET_PARAM, PARAMS_LEN };
    enum InputId { IN_DEC_INPUT, IN_INC_INPUT, IN_IN_INPUT, INPUTS_LEN };
    enum OutputId { OUT_OUT_OUTPUT, OUTPUTS_LEN };
    enum LightId {
        LED_1_LIGHT,
        LED_2_LIGHT,
        LED_3_LIGHT,
        LED_4_LIGHT,
        LED_5_LIGHT,
        LED_6_LIGHT,
        LED_7_LIGHT,
        LED_8_LIGHT,
        LED_9_LIGHT,
        LIGHTS_LEN
    };

    Octave() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(P_OFFSET_PARAM, -8.f, 8.f, 0.f, "Base offset");
        configInput(IN_INC_INPUT, "Inc trig");
        configInput(IN_DEC_INPUT, "Dec trig");
        configInput(IN_IN_INPUT, "In");
        configOutput(OUT_OUT_OUTPUT, "Out");

        paramQuantities[P_OFFSET_PARAM]->snapEnabled = true;
    }

    void process(const ProcessArgs& args) override { doProcess(args); }
    void doProcess(const ProcessArgs& args);

    float trigHigh_ = 2.0f;
    float trigLow_ = 0.2f;
    bool incState_ = false;
    bool decState_ = false;
    int octave_ = 0;
};


struct OctaveWidget : ModuleWidget {
    OctaveWidget(Octave* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EOctave.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.269, 72.175)), module, Octave::P_OFFSET_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.937, 87.69)), module, Octave::IN_DEC_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(28.987, 87.69)), module, Octave::IN_INC_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.937, 105.682)), module, Octave::IN_IN_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(29.114, 105.417)), module, Octave::OUT_OUT_OUTPUT));

        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(20.218, 16.776)), module, Octave::LED_1_LIGHT));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(20.218, 22.45)), module, Octave::LED_2_LIGHT));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(20.218, 28.124)), module, Octave::LED_3_LIGHT));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(20.218, 33.798)), module, Octave::LED_4_LIGHT));
        addChild(
            createLightCentered<MediumLight<YellowLight>>(mm2px(Vec(20.218, 39.472)), module, Octave::LED_5_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.218, 45.146)), module, Octave::LED_6_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.218, 50.819)), module, Octave::LED_7_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.218, 56.493)), module, Octave::LED_8_LIGHT));
        addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(20.218, 62.167)), module, Octave::LED_9_LIGHT));
    }
};


Model* modelOctave = createModel<Octave, OctaveWidget>("Octave");


void Octave::doProcess(const ProcessArgs& args) {
    unsigned nChannels = inputs[IN_IN_INPUT].getChannels();
    outputs[OUT_OUT_OUTPUT].setChannels(nChannels);
    float base_offset = params[P_OFFSET_PARAM].getValue();

    // use hysteresis
    bool pState = incState_;
    bool nState = false;
    if (pState) {
        nState = (inputs[IN_INC_INPUT].getVoltage() <= trigLow_) ? false : pState;
    } else {
        nState = (inputs[IN_INC_INPUT].getVoltage() >= trigHigh_) ? true : pState;
    }
    if (nState && nState != pState) {
        if (octave_ < 4) octave_++;
    }
    incState_ = nState;


    pState = decState_;
    if (pState) {
        nState = (inputs[IN_DEC_INPUT].getVoltage() <= trigLow_) ? false : pState;
    } else {
        nState = (inputs[IN_DEC_INPUT].getVoltage() >= trigHigh_) ? true : pState;
    }
    if (nState && nState != pState) {
        if (octave_ > -4) octave_--;
    }
    decState_ = nState;

    lights[LED_1_LIGHT].setBrightness(octave_ >= 4 ? 1.f : 0.f);
    lights[LED_2_LIGHT].setBrightness(octave_ >= 3 ? 1.f : 0.f);
    lights[LED_3_LIGHT].setBrightness(octave_ >= 2 ? 1.f : 0.f);
    lights[LED_4_LIGHT].setBrightness(octave_ >= 1 ? 1.f : 0.f);
    lights[LED_5_LIGHT].setBrightness(1.f);
    lights[LED_6_LIGHT].setBrightness(octave_ <= -1 ? 1.f : 0.f);
    lights[LED_7_LIGHT].setBrightness(octave_ <= -2 ? 1.f : 0.f);
    lights[LED_8_LIGHT].setBrightness(octave_ <= -3 ? 1.f : 0.f);
    lights[LED_9_LIGHT].setBrightness(octave_ <= -4 ? 1.f : 0.f);


    for (unsigned ch = 0; ch < nChannels; ch++) {
        auto in = inputs[IN_IN_INPUT].getVoltage(ch);
        float offset = base_offset + float(octave_);
        outputs[OUT_OUT_OUTPUT].setVoltage(in + offset, ch);
    }
}