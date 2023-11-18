#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct EIlluminator : Module {
    enum ParamId {
        PARAMS_LEN
    };
    enum InputId {
        IN_DISABLE_INPUT,
        IN_K_INPUT,
        IN_KG_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        OUT_LIGHTS_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    EIlluminator() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configInput(IN_DISABLE_INPUT, "");
        configInput(IN_K_INPUT, "");
        configInput(IN_KG_INPUT, "");
        configOutput(OUT_LIGHTS_OUTPUT, "");

        auto dir = asset::plugin(pluginInstance, "res/patterns");
        auto files = system::getEntries(dir);

        LedPattern emptyPattern;
        emptyPattern.name_ = "None";
        patterns_.push_back(emptyPattern);

        for (auto& f : files) {
            readPattern(f);
        }
        if (patterns_.size() > 0) patternIdx_ = 0;
    }

    void processBypass(const ProcessArgs& args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs& args) override {
        doProcess(args);
    }

    void doProcessBypass(const ProcessArgs& args) {}
    void doProcess(const ProcessArgs& args);

    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "patternIdx", json_integer(patternIdx_));
        return root;
    }
    void dataFromJson(json_t* rootJ) override {
        // panelTheme
        json_t* patternIdx = json_object_get(rootJ, "patternIdx");
        if (patternIdx) setPatternIdx(json_integer_value(patternIdx));
    }

    void readPattern(const std::string& f) {
        FILE* file = std::fopen(f.c_str(), "r");
        if (!file) return;
        DEFER({ std::fclose(file); });

        LedPattern pattern;
        pattern.name_ = system::getStem(f);

        json_error_t error;
        json_t* rootJ = json_loadf(file, 0, &error);
        if (!rootJ) return;
        DEFER({ json_decref(rootJ); });

        json_t* ledsJ = nullptr;
        ledsJ = json_object_get(rootJ, "red");
        if (ledsJ) {
            if (json_is_array(ledsJ)) {
                json_t* rcpair = nullptr;
                size_t index = 0;
                // array or row/column
                json_array_foreach(ledsJ, index, rcpair) {
                    if (json_is_array(rcpair) && json_array_size(rcpair) == 2) {
                        LedMsg ledmsg;
                        ledmsg.row_ = json_integer_value(json_array_get(rcpair, 0));
                        ledmsg.col_ = json_integer_value(json_array_get(rcpair, 1));
                        ledmsg.colour_ = LED_SET_RED;
                        pattern.msgs_.push_back(ledmsg);
                    }
                }
            }
        }
        ledsJ = json_object_get(rootJ, "green");
        if (ledsJ) {
            if (json_is_array(ledsJ)) {
                json_t* rcpair = nullptr;
                size_t index = 0;
                // array or row/column
                json_array_foreach(ledsJ, index, rcpair) {
                    if (json_is_array(rcpair) && json_array_size(rcpair) == 2) {
                        LedMsg ledmsg;
                        ledmsg.row_ = json_integer_value(json_array_get(rcpair, 0));
                        ledmsg.col_ = json_integer_value(json_array_get(rcpair, 1));
                        ledmsg.colour_ = LED_SET_GREEN;
                        pattern.msgs_.push_back(ledmsg);
                    }
                }
            }
        }

        ledsJ = json_object_get(rootJ, "orange");
        if (ledsJ) {
            if (json_is_array(ledsJ)) {
                json_t* rcpair = nullptr;
                size_t index = 0;
                // array or row/column
                json_array_foreach(ledsJ, index, rcpair) {
                    if (json_is_array(rcpair) && json_array_size(rcpair) == 2) {
                        LedMsg ledmsg;
                        ledmsg.row_ = json_integer_value(json_array_get(rcpair, 0));
                        ledmsg.col_ = json_integer_value(json_array_get(rcpair, 1));
                        ledmsg.colour_ = LED_SET_ORANGE;
                        pattern.msgs_.push_back(ledmsg);
                    }
                }
            }
        }

        patterns_.push_back(pattern);
    }

    void setPatternIdx(int idx) {
        refreshLeds_ |= patternIdx_ != idx;
        patternIdx_ = idx;
    }

    int getPatternIdx() {
        return patternIdx_;
    }

    struct LedMsg {
        unsigned row_ = 0;
        unsigned col_ = 0;
        LedMsgType colour_ = LED_SET_OFF;
    };

    struct LedPattern {
        std::string name_;
        std::vector<LedMsg> msgs_;
    };

    bool layoutChanged_ = false;
    bool refreshLeds_ = false;
    bool enabled_ = true;
    unsigned kg_c_ = 0, kg_r_ = 0;

    MsgQueue<float> ledQueue_;

    std::vector<LedPattern> patterns_;
    int patternIdx_ = -1;
};

struct EIlluminatorWidget : ModuleWidget {
    EIlluminatorWidget(EIlluminator* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EIlluminator.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.978, 74.749)), module, EIlluminator::IN_DISABLE_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.416, 93.865)), module, EIlluminator::IN_K_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.826, 93.865)), module, EIlluminator::IN_KG_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.549, 118.115)), module, EIlluminator::OUT_LIGHTS_OUTPUT));
    }

    void appendContextMenu(Menu* menu) override {
        EIlluminator* module = getModule<EIlluminator>();

        menu->addChild(new MenuSeparator);

        std::vector<std::string> names;
        for (auto& pattern : module->patterns_) {
            names.push_back(pattern.name_);
        }

        menu->addChild(
            createIndexSubmenuItem(
                "Patterns",
                names,
                [=]() {
                    return module->getPatternIdx();
                },
                [=](int idx) {
                    module->setPatternIdx(idx);
                }));
    }
};

Model* modelEIlluminator = createModel<EIlluminator, EIlluminatorWidget>("EIlluminator");

/////////////////////////////////////////////////////////////////////////////

void EIlluminator::doProcess(const ProcessArgs& args) {
    bool enabled = !(inputs[IN_DISABLE_INPUT].getVoltage() > 1.5f);
    if (enabled_ != enabled) {
        enabled_ = enabled;
        refreshLeds_ = enabled_;
    }

    float kg = inputs[IN_KG_INPUT].getVoltage();
    unsigned kg_r = 0, kg_c = 0;
    decodeKeyGroup(kg, kg_r, kg_c);
    if ((kg_c_ != kg_c) || (kg_r_ != kg_r)) {
        layoutChanged_ = true;
        kg_c_ = kg_c;
        kg_r_ = kg_r;
    }

    if (!enabled_) return;

    if (layoutChanged_) {
        refreshLeds_ = true;
        layoutChanged_ = false;
    }

    if (refreshLeds_) {
        refreshLeds_ = false;
        ledQueue_.clear();
        float clearmsg = encodeLedMsg(LED_SET_OFF, 0, 0, kg_r_, kg_c_);
        ledQueue_.write(clearmsg);

        if (patternIdx_ > 0 && patternIdx_ < (int)patterns_.size()) {
            auto& pattern = patterns_[patternIdx_];
            for (auto& msg : pattern.msgs_) {
                if (msg.row_ < kg_r_ && msg.col_ < kg_c_) {
                    float ledmsg = encodeLedMsg(msg.colour_, msg.row_, msg.col_, 1, 1);
                    ledQueue_.write(ledmsg);
                }
            }
        }
    }

    float msg = 0.0f;
    // dont really need this check as empty queue leaves msg untouched.
    if (ledQueue_.read(msg)) {
        // DEBUG_LIGHT_MSG("EIlluminator led ", msg);
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.0f);
    }
}