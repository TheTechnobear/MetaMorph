#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct EFunction12 : Module {
    enum ParamId { P_TYPE_PARAM, PARAMS_LEN };
    enum InputId { IN_K_INPUT, IN_GATE_INPUT, IN_KG_INPUT, IN_ENABLE_INPUT, INPUTS_LEN };
    enum OutputId {
        OUT_F1_OUTPUT,
        OUT_F2_OUTPUT,
        OUT_F3_OUTPUT,
        OUT_F4_OUTPUT,
        OUT_F5_OUTPUT,
        OUT_F6_OUTPUT,
        OUT_F7_OUTPUT,
        OUT_F8_OUTPUT,
        OUT_F9_OUTPUT,
        OUT_F10_OUTPUT,
        OUT_F11_OUTPUT,
        OUT_F12_OUTPUT,
        OUT_LIGHTS_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId { LIGHTS_LEN };

    EFunction12() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configSwitch(P_TYPE_PARAM, 0.f, 2.f, 0.f, "Type", { "Toggle", "Trig", "Gate" });

        configInput(IN_K_INPUT, "Key");
        configInput(IN_GATE_INPUT, "Gate");
        configInput(IN_KG_INPUT, "KG");
        configInput(IN_ENABLE_INPUT, "Disable");

        configOutput(OUT_F1_OUTPUT, "Func 1");
        configOutput(OUT_F2_OUTPUT, "Func 2");
        configOutput(OUT_F3_OUTPUT, "Func 3");
        configOutput(OUT_F4_OUTPUT, "Func 4");
        configOutput(OUT_F5_OUTPUT, "Func 5");
        configOutput(OUT_F6_OUTPUT, "Func 6");
        configOutput(OUT_F7_OUTPUT, "Func 7");
        configOutput(OUT_F8_OUTPUT, "Func 8");
        configOutput(OUT_F9_OUTPUT, "Func 9");
        configOutput(OUT_F10_OUTPUT, "Func 10");
        configOutput(OUT_F11_OUTPUT, "Func 11");
        configOutput(OUT_F12_OUTPUT, "Func 12");

        configOutput(OUT_LIGHTS_OUTPUT, "LED");
    }

    void processBypass(const ProcessArgs& args) override { doProcessBypass(args); }

    void process(const ProcessArgs& args) override { doProcess(args); }

    void onPortChange(const PortChangeEvent& e) override {
        if (e.connecting) {
            switch (e.type) {
                case Port::INPUT: {
                    switch (e.portId) {
                        case IN_KG_INPUT: {
                            layoutChanged_ = true;
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                case Port::OUTPUT: {
                    switch (e.portId) {
                        case OUT_LIGHTS_OUTPUT: {
                            refreshLeds_ = true;
                            break;
                        }
                        default: break;
                    }
                }
            }
        } else {
        }

        if (e.type == Port::OUTPUT && e.portId >= OUT_F1_OUTPUT && e.portId <= OUT_F12_OUTPUT) {
            connectedFuncs_[e.portId - OUT_F1_OUTPUT] = e.connecting;
            refreshLeds_ = true;
        }
    }

    void doProcessBypass(const ProcessArgs& args) {}

    void doProcess(const ProcessArgs& args);


    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "exclusiveMode", json_boolean(exclusiveMode_));
        json_object_set_new(root, "trigOnRelease", json_boolean(trigOnRelease_));
        return root;
    }
    void dataFromJson(json_t* rootJ) override {
        {
            json_t* vJ = json_object_get(rootJ, "exclusiveMode");
            if (vJ) { exclusiveMode_ = json_boolean_value(vJ); }
        }
        {
            json_t* vJ = json_object_get(rootJ, "trigOnRelease");
            if (vJ) { trigOnRelease_ = json_boolean_value(vJ); }
        }
    }


    MsgQueue<float> ledQueue_;

    bool layoutChanged_ = false;
    bool refreshLeds_ = false;
    bool enabled_ = true;
    unsigned kg_r_ = 0, kg_c_ = 0;

    bool exclusiveMode_ = false;
    bool trigOnRelease_ = false;
    float trigHigh_ = 2.0f;
    float trigLow_ = 0.2f;


    static constexpr unsigned MAX_FUNCS = 12;
    bool connectedFuncs_[MAX_FUNCS] = { false, false, false, false, false, false,
                                        false, false, false, false, false, false };

    struct FuncKey {
        bool state_ = false;
        bool last_key_state_ = false;
        bool valid_ = false;
        int r_ = -1, c_ = -1;
        unsigned ch_ = 0xff;

        unsigned trigCount_ = 0;
        const unsigned TRIG_LEN = 48;

        enum { S_TOGGLE, S_TRIG, S_GATE, S_MAX };

        bool valid() { return valid_; }

        bool updateKey(int r, int c) {
            if (r != r_ || c_ != c) {
                r_ = r;
                c_ = c;
                valid_ = r_ >= 0 && c_ >= 0;
                return true;
            }
            return false;
        }

        bool changeState(unsigned switch_type, bool keystate, bool trigOnRelease) {
            bool preState = state_;
            switch (switch_type) {
                case S_GATE: {
                    state_ = keystate;
                    trigCount_ = 0;
                    break;
                }
                case S_TRIG: {
                    if (trigOnRelease) {
                        state_ = last_key_state_ && !keystate;
                        if (state_) { trigCount_ = TRIG_LEN; }
                    } else {
                        state_ = !last_key_state_ && keystate;
                        if (state_) { trigCount_ = TRIG_LEN; }
                    }
                    break;
                }
                case S_TOGGLE: {
                    // then toggle the state
                    if (trigOnRelease) {
                        if (last_key_state_ && !keystate) { state_ = !state_; }
                    } else {
                        if (!last_key_state_ && keystate) { state_ = !state_; }
                    }
                    trigCount_ = 0;
                    break;
                }
                default: {
                    // undefined type, use gate
                    state_ = keystate;
                    break;
                }
            }
            last_key_state_ = keystate;

            return preState != state_;
        }

    } funcs_[MAX_FUNCS];
};

struct EFunction12Widget : ModuleWidget {
    EFunction12Widget(EFunction12* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EFunction12.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<CKSSThree>(mm2px(Vec(27.94, 17.826)), module, EFunction12::P_TYPE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.103, 30.979)), module, EFunction12::IN_K_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.911, 31.202)), module, EFunction12::IN_GATE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.707, 30.979)), module, EFunction12::IN_KG_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.103, 107.491)), module, EFunction12::IN_ENABLE_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.103, 48.98)), module, EFunction12::OUT_F1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.462, 48.979)), module, EFunction12::OUT_F2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.172, 48.98)), module, EFunction12::OUT_F3_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.707, 48.98)), module, EFunction12::OUT_F4_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.103, 68.48)), module, EFunction12::OUT_F5_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.638, 68.48)), module, EFunction12::OUT_F6_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.172, 68.48)), module, EFunction12::OUT_F7_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.707, 68.48)), module, EFunction12::OUT_F8_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.103, 87.98)), module, EFunction12::OUT_F9_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.638, 87.98)), module, EFunction12::OUT_F10_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.172, 87.98)), module, EFunction12::OUT_F11_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.707, 87.98)), module, EFunction12::OUT_F12_OUTPUT));
        addOutput(
            createOutputCentered<PJ301MPort>(mm2px(Vec(46.707, 107.491)), module, EFunction12::OUT_LIGHTS_OUTPUT));
    }


    void appendContextMenu(Menu* menu) override {
        auto module = getModule<EFunction12>();

        menu->addChild(new MenuSeparator);
        menu->addChild(createBoolMenuItem(
            "Exclusive", "", [=]() { return module->exclusiveMode_; },
            [=](bool value) { module->exclusiveMode_ = value; }));
        menu->addChild(createBoolMenuItem(
            "On Release", "", [=]() { return module->trigOnRelease_; },
            [=](bool value) { module->trigOnRelease_ = value; }));
    }
};


Model* modelEFunction12 = createModel<EFunction12, EFunction12Widget>("Function12");

//////////////////////////////////////////////////////////////////////////////

void EFunction12::doProcess(const ProcessArgs& args) {
    unsigned in_kg_r, in_kg_c;
    float kgMsg = inputs[IN_KG_INPUT].getVoltage();
    decodeKeyGroup(kgMsg, in_kg_r, in_kg_c);

    for (unsigned i = 0; i < MAX_FUNCS; i++) {
        int r = -1;
        int c = -1;
        if (in_kg_c > 0) {
            r = i / in_kg_c;
            c = i % in_kg_c;
        }
        layoutChanged_ |= funcs_[i].updateKey(r, c);
    }

    if (in_kg_r != kg_r_ || in_kg_c != kg_c_) {
        layoutChanged_ |= true;
        kg_r_ = in_kg_r;
        kg_c_ = in_kg_c;

        for (unsigned i = 0; i < MAX_FUNCS; i++) {
            int r = -1;
            int c = -1;
            if (in_kg_c > 0) {
                r = i / in_kg_c;
                c = i % in_kg_c;
            }
            funcs_[i].updateKey(r, c);
        }
    }
    bool enabled = !(inputs[IN_ENABLE_INPUT].getVoltage() > 2.0f);

    if (enabled_ != enabled) {
        enabled_ = enabled;
        if (enabled_) {
            refreshLeds_ = true;
        } else {
            // being disabled, nul op?
        }
    }

    if (layoutChanged_) {
        refreshLeds_ = true;
        layoutChanged_ = false;
    }

    if (refreshLeds_) {
        float msg = encodeLedMsg(LED_SET_OFF, 0, 0, kg_r_, kg_c_);
        // std::cout << "layout clear leds " << kg_r_ << "," << kg_c_ << std::endl;
        ledQueue_.write(msg);

        for (unsigned fk = 0; fk < MAX_FUNCS; fk++) {
            auto& func = funcs_[fk];
            unsigned r = func.r_;
            unsigned c = func.c_;
            LedMsgType t = connectedFuncs_[fk] ? (func.state_ ? LED_SET_RED : LED_SET_GREEN) : LED_SET_OFF;
            if (func.valid() && r < kg_r_ && c < kg_c_) {
                // std::cout << "layout led " << r << "," << c << " state" << t << std::endl;
                float msg = encodeLedMsg(t, r, c, 1, 1);
                ledQueue_.write(msg);
            }
        }
        refreshLeds_ = false;
    }

    unsigned switch_type = (unsigned)params[P_TYPE_PARAM].getValue();

    unsigned nChannels = inputs[IN_K_INPUT].getChannels();
    for (unsigned ch = 0; ch < nChannels; ch++) {
        unsigned in_r = 0, in_c = 0;
        bool valid = enabled_;

        if (valid) { decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c); }

        for (unsigned fk = 0; fk < MAX_FUNCS; fk++) {
            auto& func = funcs_[fk];
            if (valid && func.valid()) {
                if (in_r == (unsigned)func.r_ && in_c == (unsigned)func.c_) {
                    bool key_state = func.last_key_state_;

                    // use hysteresis
                    if (func.last_key_state_) {
                        key_state = (inputs[IN_GATE_INPUT].getVoltage(ch) <= trigLow_) ? false : func.last_key_state_;
                    } else {
                        key_state = (inputs[IN_GATE_INPUT].getVoltage(ch) >= trigHigh_) ? true : func.last_key_state_;
                    }

                    if (func.last_key_state_ != key_state) {
                        bool changed = func.changeState(switch_type, key_state, trigOnRelease_);

                        if (exclusiveMode_ && changed && switch_type == FuncKey::S_TOGGLE) {
                            // if exlusive toggles then turn off other functions
                            if (func.state_) {
                                for (unsigned fk2 = 0; fk2 < MAX_FUNCS; fk2++) {
                                    if (fk != fk2) {
                                        auto& func2 = funcs_[fk2];
                                        if (func2.state_) {
                                            func2.state_ = false;
                                            LedMsgType t = connectedFuncs_[fk2]
                                                               ? (func2.state_ ? LED_SET_RED : LED_SET_GREEN)
                                                               : LED_SET_OFF;
                                            float msg = encodeLedMsg(t, func2.r_, func2.c_, 1, 1);
                                            ledQueue_.write(msg);
                                        }
                                    }
                                }
                            }
                        }

                        if (changed) {
                            LedMsgType t =
                                connectedFuncs_[fk] ? (func.state_ ? LED_SET_RED : LED_SET_GREEN) : LED_SET_OFF;
                            // std::cout << "change led " << in_r << "," << in_c << " state" << t << std::endl;
                            float msg = encodeLedMsg(t, in_r, in_c, 1, 1);
                            ledQueue_.write(msg);
                        }
                    }
                    func.ch_ = ch;
                }
            } else {
                if (enabled_) {
                    if (func.ch_ == ch) {
                        // key now not active
                        func.ch_ = 0xff;
                        bool key_state = false;
                        bool changed = false;
                        if (func.last_key_state_ != key_state) {
                            changed = func.changeState(switch_type, key_state, trigOnRelease_);
                        }


                        if (exclusiveMode_ && changed && switch_type == FuncKey::S_TOGGLE) {
                            // if exlusive toggles then turn off other functions
                            if (func.state_) {
                                for (unsigned fk2 = 0; fk2 < MAX_FUNCS; fk2++) {
                                    if (fk != fk2) {
                                        auto& func2 = funcs_[fk2];
                                        if (func2.state_) {
                                            func2.state_ = false;
                                            LedMsgType t = connectedFuncs_[fk2]
                                                               ? (func2.state_ ? LED_SET_RED : LED_SET_GREEN)
                                                               : LED_SET_OFF;
                                            float msg = encodeLedMsg(t, func2.r_, func2.c_, 1, 1);
                                            ledQueue_.write(msg);
                                        }
                                    }
                                }
                            }
                        }

                        unsigned r = func.r_;
                        unsigned c = func.c_;
                        if (r < kg_r_ && c < kg_c_ && func.valid()) {
                            LedMsgType t =
                                connectedFuncs_[fk] ? (func.state_ ? LED_SET_RED : LED_SET_GREEN) : LED_SET_OFF;
                            // std::cout << "change led " << in_r << "," << in_c << " state" << t << std::endl;
                            float msg = encodeLedMsg(t, r, c, 1, 1);
                            ledQueue_.write(msg);
                        }
                    } else {
                        // clear trig state
                        if (switch_type == FuncKey::S_TRIG && func.state_) {
                            if (func.trigCount_ > 0) func.trigCount_--;

                            if (func.trigCount_ == 0) {
                                unsigned r = func.r_;
                                unsigned c = func.c_;
                                func.state_ = false;
                                if (r < kg_r_ && c < kg_c_) {
                                    LedMsgType t =
                                        connectedFuncs_[fk] ? (func.state_ ? LED_SET_RED : LED_SET_GREEN) : LED_SET_OFF;
                                    // std::cout << "change led " << in_r << "," << in_c << " state" << t <<
                                    // std::endl;
                                    float msg = encodeLedMsg(t, r, c, 1, 1);
                                    ledQueue_.write(msg);
                                }
                            }
                        }
                    }
                }
            }
        }
    }  // channel

    for (unsigned fk = 0; fk < MAX_FUNCS; fk++) {
        auto& func = funcs_[fk];
        outputs[OUT_F1_OUTPUT + fk].setVoltage(func.state_ * 10.f);  //  gate @ 10v
    }

    float msg = 0.0f;
    if (ledQueue_.read(msg)) {
        // DEBUG_LIGHT_MSG("write f12", msg);s
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
    }
}
