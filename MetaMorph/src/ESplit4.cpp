#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct ESplit4 : Module {
    enum ParamId {
        P_S1_NROW_PARAM,
        P_S1_POLY_PARAM,
        P_S2_NROW_PARAM,
        P_S2_POLY_PARAM,
        P_S3_NROW_PARAM,
        P_S3_POLY_PARAM,
        P_S4_NROW_PARAM,
        P_S4_POLY_PARAM,
        PARAMS_LEN
    };
    enum InputId {
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
    enum LightId { LIGHTS_LEN };

    ESplit4() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(P_S1_NROW_PARAM, 0.f, 24.f, 0.f, "");
        configParam(P_S1_POLY_PARAM, 0.f, 16.f, 0.f, "");
        configParam(P_S2_NROW_PARAM, 0.f, 24.f, 0.f, "");
        configParam(P_S2_POLY_PARAM, 0.f, 16.f, 0.f, "");
        configParam(P_S3_NROW_PARAM, 0.f, 24.f, 0.f, "");
        configParam(P_S3_POLY_PARAM, 0.f, 16.f, 0.f, "");
        configParam(P_S4_NROW_PARAM, 0.f, 24.f, 0.f, "");
        configParam(P_S4_POLY_PARAM, 0.f, 16.f, 0.f, "");
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

        paramQuantities[P_S1_NROW_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_NROW_PARAM]->snapEnabled = true;
        paramQuantities[P_S3_NROW_PARAM]->snapEnabled = true;
        paramQuantities[P_S4_NROW_PARAM]->snapEnabled = true;
        paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S3_POLY_PARAM]->snapEnabled = true;
        paramQuantities[P_S4_POLY_PARAM]->snapEnabled = true;
    }

    void processBypass(const ProcessArgs &args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs &args) override { doProcess(args); }

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

    static constexpr unsigned MAX_SPLIT = 4;

    Split splits_[MAX_SPLIT];
    Voices<SplitVoice> voices_[MAX_SPLIT];
    MsgQueue<float> ledQueue_;
};

struct ESplit4Widget : ModuleWidget {
    ESplit4Widget(ESplit4 *module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/ESplit4.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 58.215)), module, ESplit4::P_S1_NROW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 58.215)), module, ESplit4::P_S1_POLY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 77.265)), module, ESplit4::P_S2_NROW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 77.794)), module, ESplit4::P_S2_POLY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 96.844)), module, ESplit4::P_S3_NROW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 97.373)), module, ESplit4::P_S3_POLY_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 115.894)), module, ESplit4::P_S4_NROW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 116.423)), module, ESplit4::P_S4_POLY_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.763, 32.772)), module, ESplit4::IN_DISABLE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.355, 32.772)), module, ESplit4::IN_K_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.433, 32.772)), module, ESplit4::IN_X_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61.51, 32.772)), module, ESplit4::IN_Y_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(73.588, 32.772)), module, ESplit4::IN_Z_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.195, 32.772)), module, ESplit4::IN_KG_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 58.215)), module, ESplit4::IN1_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 77.794)), module, ESplit4::IN2_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 97.373)), module, ESplit4::IN3_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 116.423)), module, ESplit4::IN4_LIGHTS_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(98.285, 32.772)), module, ESplit4::OUT_LIGHTS_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 58.215)), module, ESplit4::OUT1_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 58.215)), module, ESplit4::OUT1_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 58.215)), module, ESplit4::OUT1_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 58.215)), module, ESplit4::OUT1_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 58.215)), module, ESplit4::OUT1_KG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 77.794)), module, ESplit4::OUT2_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 77.794)), module, ESplit4::OUT2_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 77.794)), module, ESplit4::OUT2_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 77.794)), module, ESplit4::OUT2_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 77.794)), module, ESplit4::OUT2_KG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 97.373)), module, ESplit4::OUT3_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 97.373)), module, ESplit4::OUT3_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 97.373)), module, ESplit4::OUT3_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 97.373)), module, ESplit4::OUT3_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 97.373)), module, ESplit4::OUT3_KG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 116.423)), module, ESplit4::OUT4_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 116.423)), module, ESplit4::OUT4_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 116.423)), module, ESplit4::OUT4_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 116.423)), module, ESplit4::OUT4_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 116.423)), module, ESplit4::OUT4_KG_OUTPUT));
    }
};

Model *modelESplit4 = createModel<ESplit4, ESplit4Widget>("ESplit4");

////////////////////////////////////////////////////////////////////////////////////////////////

void ESplit4::doProcess(const ProcessArgs &args) {
    unsigned in_kg_r, in_kg_c;
    float kgMsg = inputs[IN_KG_INPUT].getVoltage();
    decodeKeyGroup(kgMsg, in_kg_r, in_kg_c);

    unsigned nChannels = inputs[IN_K_INPUT].getChannels();

    unsigned splitRow = 0;
    // enums are auto sorted, not from svg
    static constexpr unsigned OUT_N = OUT2_K_OUTPUT - OUT1_K_OUTPUT;
    static constexpr unsigned PARAM_N = P_S2_NROW_PARAM - P_S1_NROW_PARAM;

    bool refreshLeds = false;

    for (unsigned splitId = 0; splitId < MAX_SPLIT; splitId++) {
        unsigned nRow = params[P_S1_NROW_PARAM + (splitId * PARAM_N)].getValue();
        unsigned maxVoices = params[P_S1_POLY_PARAM + (splitId * PARAM_N)].getValue();

        auto &split = splits_[splitId];
        auto &voices = voices_[splitId];

        // just check startR since no others change
        if (in_kg_c != split.sizeC_ || nRow != split.sizeR_ || splitRow != split.startR_) {
            split.setStart(splitRow, 0);
            split.setSize(nRow, in_kg_c);
            refreshLeds = true;
        }
        splitRow += nRow;

        bool splitValid = maxVoices > 0 && nRow > 0;

        outputs[OUT1_K_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
        outputs[OUT1_X_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
        outputs[OUT1_Y_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);
        outputs[OUT1_Z_OUTPUT + (splitId * OUT_N)].setChannels(maxVoices);

        if (splitValid) {
            unsigned startR, startC, endR, endC;
            startR = split.startR_;
            startC = split.startC_;
            endR = split.startR_ + split.sizeR_ - 1;
            endC = split.startC_ + split.sizeC_ - 1;

            // for each channel
            for (unsigned ch = 0; ch < nChannels; ch++) {
                unsigned in_r = 0, in_c = 0;
                bool valid = !(inputs[IN_DISABLE_INPUT].getVoltage() >= 1.5f);
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

    if (refreshLeds) {
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
    }

    // dont really need this check as empty queue leaves msg untouched.
    float msg = 0.0f;
    if (ledQueue_.read(msg)) {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
    }
}
