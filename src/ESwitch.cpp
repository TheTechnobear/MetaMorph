#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

#include "EComponents.h"

struct ESwitch : Module {
    enum ParamId {
        P_S1_POLY_PARAM,
        P_S2_POLY_PARAM,
        P_S3_POLY_PARAM,
        P_S4_POLY_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        IN_SELECT_INPUT,
        IN_DISABLE_INPUT,
        IN_K_INPUT,
        IN_X_INPUT,
        IN_Y_INPUT,
        IN_Z_INPUT,
        IN_KG_INPUT,
        IN1_LIGHTS_INPUT,
        IN2_LIGHTS_INPUT,
        IN3_LIGHTS_INPUT,
        IN4_LIGHTS_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        OUT_LIGHTS_OUTPUT,
        OUT1_K_OUTPUT,
        OUT1_X_OUTPUT,
        OUT1_Y_OUTPUT,
        OUT1_Z_OUTPUT,
        OUT1_KG_OUTPUT,
        OUT2_K_OUTPUT,
        OUT2_X_OUTPUT,
        OUT2_Y_OUTPUT,
        OUT2_Z_OUTPUT,
        OUT2_KG_OUTPUT,
        OUT3_K_OUTPUT,
        OUT3_X_OUTPUT,
        OUT3_Y_OUTPUT,
        OUT3_Z_OUTPUT,
        OUT3_KG_OUTPUT,
        OUT4_K_OUTPUT,
        OUT4_X_OUTPUT,
        OUT4_Y_OUTPUT,
        OUT4_Z_OUTPUT,
        OUT4_KG_OUTPUT,
        OUTPUTS_LEN
    };
	enum LightId {
		LED_S1_LIGHT,
		LED_S2_LIGHT,
		LED_S3_LIGHT,
		LED_S4_LIGHT,
		LIGHTS_LEN
	};

    ESwitch() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(P_S1_POLY_PARAM, 0.f, 16.f, 8.f, "");
        configParam(P_S2_POLY_PARAM, 0.f, 16.f, 8.f, "");
        configParam(P_S3_POLY_PARAM, 0.f, 16.f, 0.f, "");
        configParam(P_S4_POLY_PARAM, 0.f, 16.f, 0.f, "");
        configInput(IN_SELECT_INPUT, "");
        configInput(IN_DISABLE_INPUT, "");
        configInput(IN_K_INPUT, "");
        configInput(IN_X_INPUT, "");
        configInput(IN_Y_INPUT, "");
        configInput(IN_Z_INPUT, "");
        configInput(IN_KG_INPUT, "");
        configInput(IN1_LIGHTS_INPUT, "");
        configInput(IN2_LIGHTS_INPUT, "");
        configInput(IN3_LIGHTS_INPUT, "");
        configInput(IN4_LIGHTS_INPUT, "");
        configOutput(OUT_LIGHTS_OUTPUT, "");
        configOutput(OUT1_K_OUTPUT, "");
        configOutput(OUT1_X_OUTPUT, "");
        configOutput(OUT1_Y_OUTPUT, "");
        configOutput(OUT1_Z_OUTPUT, "");
        configOutput(OUT1_KG_OUTPUT, "");
        configOutput(OUT2_K_OUTPUT, "");
        configOutput(OUT2_X_OUTPUT, "");
        configOutput(OUT2_Y_OUTPUT, "");
        configOutput(OUT2_Z_OUTPUT, "");
        configOutput(OUT2_KG_OUTPUT, "");
        configOutput(OUT3_K_OUTPUT, "");
        configOutput(OUT3_X_OUTPUT, "");
        configOutput(OUT3_Y_OUTPUT, "");
        configOutput(OUT3_Z_OUTPUT, "");
        configOutput(OUT3_KG_OUTPUT, "");
        configOutput(OUT4_K_OUTPUT, "");
        configOutput(OUT4_X_OUTPUT, "");
        configOutput(OUT4_Y_OUTPUT, "");
        configOutput(OUT4_Z_OUTPUT, "");
        configOutput(OUT4_KG_OUTPUT, "");

        paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S3_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S4_POLY_PARAM]->snapEnabled = true;
    }

    void processBypass(const ProcessArgs& args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs& args) override {
        doProcess(args);
    }

    void onPortChange(const PortChangeEvent& e) override {
        if (e.connecting) {
            switch (e.type) {
                case Port::INPUT: {
                    switch (e.portId) {
                        case IN_KG_INPUT: {
                            layoutChanged_ = true;
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                case Port::OUTPUT: {
                    switch (e.portId) {
                        case OUT_LIGHTS_OUTPUT: {
                            refreshLeds_ = true;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        } else {
            switch (e.type) {
                case Port::INPUT: {
                    switch (e.portId) {
                        case IN1_LIGHTS_INPUT:
                        case IN2_LIGHTS_INPUT:
                        case IN3_LIGHTS_INPUT:
                        case IN4_LIGHTS_INPUT: {
                            splits_[e.portId - IN1_LIGHTS_INPUT].clearLeds();
                            refreshLeds_ = true;
                            break;
                        }
                        default:
                            break;
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    void doProcessBypass(const ProcessArgs& args) {
    }

    void doProcess(const ProcessArgs& args);

    struct SplitVoice : public Voice {
        unsigned r_ = 0;
        unsigned c_ = 0;
        float x_ = 0.f;
        float y_ = 0.f;
        float z_ = 0.f;

        void updateVoice(long key, unsigned r, unsigned c, bool a, float x, float y, float z) {
            active_ = a;
            key_ = key;  // inbound channel
            r_ = r;
            c_ = c;
            x_ = x;
            y_ = y;
            z_ = z;
        }

        void freeVoice() override {
            updateVoice(0xff, false, 0, 0, 0.f, 0.f, 0.f);
        }
    };

    struct Split {
        unsigned startR_ = 0, startC_ = 0;
        unsigned sizeR_ = 0, sizeC_ = 0;
        bool active_ = false;

        Split() {
            clearLeds();
        }

        void active(bool a) { active_ = a; }

        void setStart(unsigned r, unsigned c) {
            startR_ = r < (MAX_R - 1) ? r : MAX_R - 1;
            startC_ = c < (MAX_C - 1) ? c : MAX_C - 1;
        }
        void setSize(unsigned r, unsigned c) {
            sizeR_ = r < MAX_R ? r : MAX_R;
            sizeC_ = c < MAX_C ? c : MAX_C;
        }

        void setLed(LedMsgType t, unsigned startr, unsigned startc, unsigned sizer, unsigned sizec) {
            unsigned r = 0, c = 0;
            for (r = startr; r < sizeR_ && r < (startr + sizer); r++) {
                for (c = startc; c < sizeC_ && c < (startc + sizec); c++) {
                    ledState_[r][c] = t;
                }
            }
        }

        void clearLeds() {
            for (unsigned r = 0; r < MAX_R; r++) {
                for (unsigned c = 0; c < MAX_R; c++) {
                    ledState_[r][c] = LED_SET_OFF;
                }
            }
        }

        static constexpr unsigned MAX_R = 24;
        static constexpr unsigned MAX_C = 5;
        LedMsgType ledState_[MAX_R][MAX_C];
    };

    bool refreshLeds_ = false;
    bool layoutChanged_ = false;
    static constexpr unsigned MAX_SPLIT = 4;
    Split splits_[MAX_SPLIT];
    Voices<SplitVoice> voices_[MAX_SPLIT];
    MsgQueue<float> ledQueue_;
    unsigned activeSplit_ = MAX_SPLIT;  // invalid
};

struct ESwitchWidget : ModuleWidget {
    ESwitchWidget(ESwitch* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/ESwitch.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


		addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.181, 52.554)), module, ESwitch::P_S1_POLY_PARAM));
		addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.181, 72.133)), module, ESwitch::P_S2_POLY_PARAM));
		addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.181, 91.712)), module, ESwitch::P_S3_POLY_PARAM));
		addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.181, 111.106)), module, ESwitch::P_S4_POLY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.721, 15.809)), module, ESwitch::IN_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.181, 29.567)), module, ESwitch::IN_DISABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.336, 29.567)), module, ESwitch::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(33.523, 29.567)), module, ESwitch::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.71, 29.567)), module, ESwitch::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.6, 29.567)), module, ESwitch::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(70.104, 29.567)), module, ESwitch::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.296, 49.043)), module, ESwitch::IN1_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.296, 68.54)), module, ESwitch::IN2_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.296, 88.041)), module, ESwitch::IN3_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.296, 107.555)), module, ESwitch::IN4_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.297, 29.567)), module, ESwitch::OUT_LIGHTS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.336, 49.043)), module, ESwitch::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.523, 49.043)), module, ESwitch::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.71, 49.043)), module, ESwitch::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(58.156, 49.043)), module, ESwitch::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.104, 49.043)), module, ESwitch::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.336, 68.54)), module, ESwitch::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.523, 68.54)), module, ESwitch::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.71, 68.54)), module, ESwitch::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(58.156, 68.54)), module, ESwitch::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.104, 68.54)), module, ESwitch::OUT2_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.336, 88.041)), module, ESwitch::OUT3_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.523, 88.041)), module, ESwitch::OUT3_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.71, 88.041)), module, ESwitch::OUT3_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(58.156, 88.041)), module, ESwitch::OUT3_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.104, 88.041)), module, ESwitch::OUT3_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.336, 107.555)), module, ESwitch::OUT4_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.523, 107.555)), module, ESwitch::OUT4_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.71, 107.555)), module, ESwitch::OUT4_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.82, 107.555)), module, ESwitch::OUT4_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.104, 107.555)), module, ESwitch::OUT4_KG_OUTPUT));

		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.389, 46.068)), module, ESwitch::LED_S1_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.389, 65.564)), module, ESwitch::LED_S2_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.389, 85.066)), module, ESwitch::LED_S3_LIGHT));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.389, 104.634)), module, ESwitch::LED_S4_LIGHT));
    }
};

Model* modelESwitch = createModel<ESwitch, ESwitchWidget>("ESwitch");

/////////////////////////////////////////////////////////////////////////////////////

void ESwitch::doProcess(const ProcessArgs& args) {
    unsigned in_kg_r, in_kg_c;
    float kgMsg = inputs[IN_KG_INPUT].getVoltage();
    decodeKeyGroup(kgMsg, in_kg_r, in_kg_c);

    static constexpr unsigned OUT_N = OUT2_K_OUTPUT - OUT1_K_OUTPUT;
    static constexpr unsigned PARAM_N = P_S2_POLY_PARAM - P_S1_POLY_PARAM;

    unsigned nChannels = inputs[IN_K_INPUT].getChannels();
    bool enabled = !(inputs[IN_DISABLE_INPUT].getVoltage() >= 1.5f);

    float selector = inputs[IN_SELECT_INPUT].getVoltage();
    unsigned activeSplit = (int)selector;

    if (activeSplit > (MAX_SPLIT - 1)) activeSplit = (MAX_SPLIT - 1);

    refreshLeds_ |= (activeSplit_ != activeSplit);

    activeSplit_ = activeSplit;

    bool activityLed[MAX_SPLIT] = {false,false,false,false};
    for (unsigned splitId = 0; splitId < MAX_SPLIT; splitId++) {
        auto& voices = voices_[splitId];
        auto& split = splits_[splitId];

        unsigned maxVoices = params[P_S1_POLY_PARAM + (splitId * PARAM_N)].getValue();
        if (maxVoices > nChannels) maxVoices = nChannels;

        bool inSplit = (splitId == activeSplit_);

        if (in_kg_r != split.sizeR_ || in_kg_c != split.sizeC_) {
            split.setStart(0, 0);
            split.setSize(in_kg_r, in_kg_c);
            layoutChanged_ |= inSplit;
        }

        // forward led msgs
        float ledmsg = inputs[IN1_LIGHTS_INPUT + splitId].getVoltage();
        if (ledmsg != 0.0f) {
            unsigned startr, startc, sizer, sizec;
            LedMsgType t;
            decodeLedMsg(ledmsg, t, startr, startc, sizer, sizec);
            split.setLed(t, startr, startc, sizer, sizec);

            if (inSplit) {
                ledQueue_.write(ledmsg);
            }
        }

        outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(kgMsg);

        bool splitValid = enabled && maxVoices > 0;
        if (splitValid) {
            outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            // outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(kgMsg);

            for (unsigned ch = 0; ch < nChannels; ch++) {
                auto v = voices.findVoice(ch);
                unsigned in_r = 0, in_c = 0;
                bool valid = enabled;
                if (valid) {
                    decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c);
                }

                float inX = inputs[IN_X_INPUT].getVoltage(ch);
                float inY = inputs[IN_Y_INPUT].getVoltage(ch);
                float inZ = inputs[IN_Z_INPUT].getVoltage(ch);

                if (valid && inSplit) {
                    unsigned r = in_r;
                    unsigned c = in_c;
                    if (!v) {
                        v = voices.findFreeVoice(maxVoices);
                    }

                    if (v) {
                        v->updateVoice(ch, r, c, valid, inX, inY, inZ);
                        outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setVoltage(encodeKey(r, c), v->voiceId_);
                        outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setVoltage(inX, v->voiceId_);
                        outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setVoltage(inY, v->voiceId_);
                        outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setVoltage(inZ, v->voiceId_);
                        activityLed[splitId] = activityLed[splitId]  |=true;
                    }
                } else {
                    if (v) {
                        // was in this split, but now key is outside
                        voices.freeVoice(v);
                        outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                    }
                }
            }  // for channel
        } else {
            outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setChannels(0);
            // outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(0);
        }

        lights[LED_S1_LIGHT + splitId].setBrightness(activityLed[splitId] ? 1.0f : 0.0f);

        if (layoutChanged_) {
            layoutChanged_ = false;
            refreshLeds_ = true;
        }

        if (refreshLeds_) {
            float msg = encodeLedMsg(LED_SET_OFF, 0, 0, in_kg_r, in_kg_c);
            ledQueue_.write(msg);
            unsigned splitId = activeSplit_;
            auto& split = splits_[splitId];
            for (unsigned r = 0; r < split.sizeR_; r++) {
                for (unsigned c = 0; c < split.sizeC_; c++) {
                    LedMsgType t = split.ledState_[r][c];
                    if (t != LED_SET_OFF) {
                        msg = encodeLedMsg(t, split.startR_ + r, split.startC_ + c, 1, 1);
                        ledQueue_.write(msg);
                    }
                }
            }
            refreshLeds_ = false;
        }
    }
    // dont really need this check as empty queue leaves msg untouched.
    float msg = 0.0f;
    if (ledQueue_.read(msg)) {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
    }
}