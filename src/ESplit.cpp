#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct ESplit : Module {
    enum ParamId {
        P_S1_POLY_PARAM,
        P_S1_START_R_PARAM,
        P_S1_START_C_PARAM,
        P_S1_SIZE_R_PARAM,
        P_S1_SIZE_C_PARAM,
        P_S2_POLY_PARAM,
        P_S2_START_R_PARAM,
        P_S2_START_C_PARAM,
        P_S2_SIZE_R_PARAM,
        P_S2_SIZE_C_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        IN_K_INPUT,
        IN_X_INPUT,
        IN_Y_INPUT,
        IN_Z_INPUT,
        IN_KG_INPUT,
        IN1_LIGHTS_INPUT,
        IN2_LIGHTS_INPUT,
        IN_DISABLE_INPUT,
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
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    ESplit() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(P_S1_POLY_PARAM, 1.f, 16.f, 8.f, "Polyphony of split");
        configParam(P_S1_START_R_PARAM, 1.f, 24.f, 1.f, "Start row of split");
        configParam(P_S1_START_C_PARAM, 1.f, 24.f, 1.f, "Start column of split");
        configParam(P_S1_SIZE_R_PARAM, 0.f, 24.f, 4.f, "Size row of split");
        configParam(P_S1_SIZE_C_PARAM, 0.f, 24.f, 5.f, "Size column of split");
        configParam(P_S2_POLY_PARAM, 1.f, 16.f, 8.f, "Polyphony of split");
        configParam(P_S2_START_R_PARAM, 1.f, 24.f, 5.f, "Start row of split");
        configParam(P_S2_START_C_PARAM, 1.f, 24.f, 1.f, "Start column of split");
        configParam(P_S2_SIZE_R_PARAM, 0.f, 24.f, 0.f, "Size row of split");
        configParam(P_S2_SIZE_C_PARAM, 0.f, 24.f, 0.f, "Size column of split");
        configInput(IN_K_INPUT, "Key");
        configInput(IN_X_INPUT, "X");
        configInput(IN_Y_INPUT, "Y");
        configInput(IN_Z_INPUT, "Z");
        configInput(IN_KG_INPUT, "Keygroup");
        configInput(IN1_LIGHTS_INPUT, "Lights");
        configInput(IN2_LIGHTS_INPUT, "Lights");
        configInput(IN_DISABLE_INPUT, "Disable");
        configOutput(OUT_LIGHTS_OUTPUT, "Lights");
        configOutput(OUT1_K_OUTPUT, "Key");
        configOutput(OUT1_X_OUTPUT, "X");
        configOutput(OUT1_Y_OUTPUT, "Y");
        configOutput(OUT1_Z_OUTPUT, "Z");
        configOutput(OUT1_KG_OUTPUT, "Keygoup");
        configOutput(OUT2_K_OUTPUT, "Key");
        configOutput(OUT2_X_OUTPUT, "X");
        configOutput(OUT2_Y_OUTPUT, "Y");
        configOutput(OUT2_Z_OUTPUT, "Z");
        configOutput(OUT2_KG_OUTPUT, "Keygroup");

        paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S1_START_R_PARAM]->snapEnabled = true;
        paramQuantities[P_S1_START_C_PARAM]->snapEnabled = true;
        paramQuantities[P_S1_SIZE_R_PARAM]->snapEnabled = true;
        paramQuantities[P_S1_SIZE_C_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_START_R_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_START_C_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_SIZE_R_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_SIZE_C_PARAM]->snapEnabled = true;
    }

    void processBypass(const ProcessArgs &args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs &args) override { doProcess(args); }

    void onPortChange(const PortChangeEvent &e) override {
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
                        case IN2_LIGHTS_INPUT: {
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

    void doProcessBypass(const ProcessArgs &args) {}

    void doProcess(const ProcessArgs &args);

    struct SplitVoice : public Voice {
        unsigned r_ = 0;
        unsigned c_ = 0;
        float x_ = 0.f;
        float y_ = 0.f;
        float z_ = 0.f;

        void updateVoice(long key, unsigned r, unsigned c, bool a, float x, float y,
                         float z) {
            active_ = a;
            key_ = key;  // inbound channel
            r_ = r;
            c_ = c;
            x_ = x;
            y_ = y;
            z_ = z;
        }

        void freeVoice() override { updateVoice(0xff, false, 0, 0, 0.f, 0.f, 0.f); }
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
    static constexpr unsigned MAX_SPLIT = 2;
    Split splits_[MAX_SPLIT];
    Voices<SplitVoice> voices_[MAX_SPLIT];
    MsgQueue<float> ledQueue_;
};

struct ESplitWidget : ModuleWidget {
    ESplitWidget(ESplit *module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/ESplit.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.109, 30.159)), module, ESplit::P_S1_START_R_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.852, 30.159)), module, ESplit::P_S1_START_C_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.17, 30.017)), module, ESplit::P_S1_SIZE_R_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(69.343, 30.017)), module, ESplit::P_S1_SIZE_C_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.109, 49.738)), module, ESplit::P_S2_START_R_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(35.852, 49.738)), module, ESplit::P_S2_START_C_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(57.17, 49.596)), module, ESplit::P_S2_SIZE_R_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(69.343, 49.596)), module, ESplit::P_S2_SIZE_C_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.199, 93.09)), module, ESplit::P_S1_POLY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.199, 117.562)), module, ESplit::P_S2_POLY_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.987, 70.18)), module, ESplit::IN_DISABLE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.167, 70.18)), module, ESplit::IN_K_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.245, 70.18)), module, ESplit::IN_X_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.323, 70.18)), module, ESplit::IN_Y_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.401, 70.18)), module, ESplit::IN_Z_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(72.007, 70.18)), module, ESplit::IN_KG_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.581, 93.09)), module, ESplit::IN1_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.581, 117.663)), module, ESplit::IN2_LIGHTS_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.098, 70.18)), module, ESplit::OUT_LIGHTS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.209, 93.09)), module, ESplit::OUT1_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.286, 93.09)), module, ESplit::OUT1_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.364, 93.09)), module, ESplit::OUT1_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.442, 93.09)), module, ESplit::OUT1_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(71.169, 93.09)), module, ESplit::OUT1_KG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.209, 117.83)), module, ESplit::OUT2_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.286, 117.83)), module, ESplit::OUT2_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(71.454, 117.568)), module, ESplit::OUT2_KG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.364, 117.83)), module, ESplit::OUT2_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.442, 117.83)), module, ESplit::OUT2_Z_OUTPUT));
    }
};

Model *modelESplit = createModel<ESplit, ESplitWidget>("ESplit");

////////////////////////////////////////////////////////////////////////////////////////////////

void ESplit::doProcess(const ProcessArgs &args) {
    unsigned in_kg_r, in_kg_c;
    float kgMsg = inputs[IN_KG_INPUT].getVoltage();
    decodeKeyGroup(kgMsg, in_kg_r, in_kg_c);

    unsigned nChannels = inputs[IN_K_INPUT].getChannels();
    bool enabled = !(inputs[IN_DISABLE_INPUT].getVoltage() >= 1.5f);

    static constexpr unsigned OUT_N = OUT2_K_OUTPUT - OUT1_K_OUTPUT;
    static constexpr unsigned PARAM_N = P_S2_POLY_PARAM - P_S1_POLY_PARAM;

    for (unsigned splitId = 0; splitId < MAX_SPLIT; splitId++) {
        auto &split = splits_[splitId];
        auto &voices = voices_[splitId];

        unsigned maxVoices = params[P_S1_POLY_PARAM + (splitId * PARAM_N)].getValue();
        if (maxVoices > nChannels) maxVoices = nChannels;

        unsigned pStartR = params[P_S1_START_R_PARAM + (splitId * PARAM_N)].getValue() - 1;
        unsigned pStartC = params[P_S1_START_C_PARAM + (splitId * PARAM_N)].getValue() - 1;
        unsigned pSizeR = params[P_S1_SIZE_R_PARAM + (splitId * PARAM_N)].getValue();
        unsigned pSizeC = params[P_S1_SIZE_C_PARAM + (splitId * PARAM_N)].getValue();

        if (pSizeR > in_kg_r) pSizeR = in_kg_r;
        if (pSizeC > in_kg_c) pSizeC = in_kg_c;

        // just check startR since no others change
        if (pStartR != split.startR_ || pStartC != split.startC_ || pSizeR != split.sizeR_ || pSizeC != split.sizeC_) {
            split.setStart(pStartR, pStartC);
            split.setSize(pSizeR, pSizeC);
            layoutChanged_ = true;
        }

        bool splitValid = enabled && maxVoices > 0 && pSizeC > 0 && pSizeR > 0;

        if (splitValid) {
            outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
            outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);

            unsigned startR, startC, endR, endC;
            startR = split.startR_;
            startC = split.startC_;
            endR = split.startR_ + split.sizeR_ - 1;
            endC = split.startC_ + split.sizeC_ - 1;

            // for each channel
            for (unsigned ch = 0; ch < nChannels; ch++) {
                unsigned in_r = 0, in_c = 0;
                bool valid = enabled;
                if (valid) {
                    decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c);
                }

                float inX = inputs[IN_X_INPUT].getVoltage(ch);
                float inY = inputs[IN_Y_INPUT].getVoltage(ch);
                float inZ = inputs[IN_Z_INPUT].getVoltage(ch);

                auto v = voices.findVoice(ch);
                bool inSplit = (in_r >= startR && in_r <= endR) && (in_c >= startC && in_c <= endC);
                if (valid && inSplit) {
                    unsigned r = in_r - startR;
                    unsigned c = in_c - startC;

                    if (!v) {
                        v = voices.findFreeVoice(maxVoices);
                    }

                    if (v) {
                        v->updateVoice(ch, r, c, valid, inX, inY, inZ);
                        outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setVoltage(encodeKey(r, c), v->voiceId_);
                        outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setVoltage(inX, v->voiceId_);
                        outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setVoltage(inY, v->voiceId_);
                        outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setVoltage(inZ, v->voiceId_);
                    }
                } else {
                    // was in this split, but now key is outside
                    if (v) {
                        voices.freeVoice(v);
                        outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                        outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setVoltage(0.f, v->voiceId_);
                    }
                }
            }  // for channel

            // forward kg output
            if (kgMsg > 0.f) {
                outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(encodeKeyGroup(split.sizeR_, split.sizeC_));
            } else {
                outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(0);
            }

            // forward led msgs
            float ledmsg = inputs[IN1_LIGHTS_INPUT + splitId].getVoltage();
            if (ledmsg != 0.0f) {
                unsigned startr, startc, sizer, sizec;
                LedMsgType t;
                decodeLedMsg(ledmsg, t, startr, startc, sizer, sizec);
                split.setLed(t, startr, startc, sizer, sizec);

                float msg = encodeLedMsg(t, startr + startR, startc + startC, sizer, sizec);
                ledQueue_.write(msg);
            }
        } else {
            // ignore, we are not using this split
            outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setChannels(0);
            outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setChannels(0);

            outputs[OUT1_KG_OUTPUT + (splitId * OUT_N)].setVoltage(0);
        }
    }  // for each split

    if (layoutChanged_) {
        layoutChanged_ = false;
        refreshLeds_ = true;
    }

    if (refreshLeds_) {
        float msg = encodeLedMsg(LED_SET_OFF, 0, 0, in_kg_r, in_kg_c);
        ledQueue_.write(msg);
        for (unsigned splitId = 0; splitId < MAX_SPLIT; splitId++) {
            auto &split = splits_[splitId];
            for (unsigned r = 0; r < split.sizeR_; r++) {
                for (unsigned c = 0; c < split.sizeC_; c++) {
                    LedMsgType t = split.ledState_[r][c];
                    if (t != LED_SET_OFF) {
                        msg = encodeLedMsg(t, split.startR_ + r, split.startC_ + c, 1, 1);
                        ledQueue_.write(msg);
                    }
                }
            }
        }
        refreshLeds_ = false;
    }

    // dont really need this check as empty queue leaves msg untouched.
    float msg = 0.0f;
    if (ledQueue_.read(msg)) {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
    }
}