#include <iostream>

#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"
#include "scala_file.hpp"

struct EScaler : Module {
    enum ParamId {
        P_ROW_MULT_PARAM,
        P_COL_MULT_PARAM,
        P_KEYPBR_PARAM,
        P_GLOBALPBR_PARAM,
        P_REF_NOTE_PARAM,
        P_LED1_IDX_PARAM,
        P_LED2_IDX_PARAM,
        P_LED3_IDX_PARAM,
        P_LED1_COLOUR_PARAM,
        P_LED2_COLOUR_PARAM,
        P_LED3_COLOUR_PARAM,
        PARAMS_LEN
    };
    enum InputId { IN_K_INPUT, IN_NOTE_PB_INPUT, IN_G_PB_INPUT, IN_KG_INPUT, INPUTS_LEN };

    enum OutputId { OUT_VOCT_OUTPUT, OUT_LIGHTS_OUTPUT, OUTPUTS_LEN };
    enum LightId { LIGHTS_LEN };

    EScaler() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(P_ROW_MULT_PARAM, -24.f, 24.f, 1.f, "x Row");
        configParam(P_COL_MULT_PARAM, -24.0f, 24.f, 4.f, "x Col");
        configParam(P_REF_NOTE_PARAM, -60.f, 60.f, 0.f, "Ref Note");
        configParam(P_KEYPBR_PARAM, 0.f, 48.f, 1.f, "Key Bend");
        configParam(P_GLOBALPBR_PARAM, 0.f, 48.f, 12.f, "Global Bend");

        configParam(P_LED1_IDX_PARAM, 0.f, 24.f, 0.f, "Degree 1");
        configParam(P_LED2_IDX_PARAM, 0.f, 24.f, 0.f, "Degree 2");
        configParam(P_LED3_IDX_PARAM, 0.f, 24.f, 0.f, "Degree 3");
        configSwitch(P_LED1_COLOUR_PARAM, 0.f, 3.f, 1.f, "Colour 1", { "Off", "Green", "Red", "Orange" });
        configSwitch(P_LED2_COLOUR_PARAM, 0.f, 3.f, 0.f, "Colour 2", { "Off", "Green", "Red", "Orange" });
        configSwitch(P_LED3_COLOUR_PARAM, 0.f, 3.f, 0.f, "Colour 3", { "Off", "Green", "Red", "Orange" });

        configInput(IN_K_INPUT, "Key");
        configInput(IN_NOTE_PB_INPUT, "Note Bend");
        configInput(IN_G_PB_INPUT, "Global Bend");
        configInput(IN_KG_INPUT, "KG");

        configOutput(OUT_VOCT_OUTPUT, "v/oct");
        configOutput(OUT_LIGHTS_OUTPUT, "LED");

        paramQuantities[P_ROW_MULT_PARAM]->snapEnabled = true;
        paramQuantities[P_COL_MULT_PARAM]->snapEnabled = true;
        paramQuantities[P_REF_NOTE_PARAM]->snapEnabled = true;

        paramQuantities[P_KEYPBR_PARAM]->snapEnabled = true;
        paramQuantities[P_GLOBALPBR_PARAM]->snapEnabled = true;

        paramQuantities[P_LED1_IDX_PARAM]->snapEnabled = true;
        paramQuantities[P_LED2_IDX_PARAM]->snapEnabled = true;
        paramQuantities[P_LED3_IDX_PARAM]->snapEnabled = true;
        paramQuantities[P_LED1_COLOUR_PARAM]->snapEnabled = true;
        paramQuantities[P_LED2_COLOUR_PARAM]->snapEnabled = true;
        paramQuantities[P_LED3_COLOUR_PARAM]->snapEnabled = true;


        scala::scale chromaticScale;
        for (int i = 0; i < 12; i++) { chromaticScale.add_degree(scala::degree((i + 1) * 100.f)); }

        scaleNames_.push_back("None");
        scales_.push_back(chromaticScale);
        scaleIdx_ = 0;


        {
            // user scales
            auto dir = asset::user("MetaMorph/scales");
            auto files = system::getEntries(dir);
            for (auto& f : files) {
                std::ifstream str(f);
                if (str.is_open()) {
                    std::string name = system::getStem(f);
                    scala::scale scale = scala::read_scl(str);
                    scaleNames_.push_back(name);
                    scales_.push_back(scale);
                }
            }
        }

        {
            // plugin scales
            auto dir = asset::plugin(pluginInstance, "res/scales");
            auto files = system::getEntries(dir);
            for (auto& f : files) {
                std::ifstream str(f);
                if (str.is_open()) {
                    std::string name = system::getStem(f);
                    scala::scale scale = scala::read_scl(str);
                    scaleNames_.push_back(name);
                    scales_.push_back(scale);
                }
            }
        }


        // for (unsigned i = 0; i < scaleNames_.size(); i++) {
        //     auto& scalename = scaleNames_[i];
        //     auto& scale = scales_[i];
        //     std::cerr << "scale : " << scalename << std::endl;
        //     for (unsigned i = 0; i < scale.get_scale_length(); i++) {
        //         std::cerr << "Degree:  " << i << " Ratio: " << scale.get_ratio(i) << std::endl;
        //     };
        // }
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
    }

    void doProcessBypass(const ProcessArgs& args) {}

    void doProcess(const ProcessArgs& args);

    json_t* dataToJson() override {
        json_t* root = json_object();
        std::string name = scaleNames_[scaleIdx_];
        json_object_set_new(root, "scaleName", json_string(name.c_str()));
        return root;
    }
    void dataFromJson(json_t* rootJ) override {
        json_t* scaleNameJ = json_object_get(rootJ, "scaleName");
        if (scaleNameJ) {
            std::string name = json_string_value(scaleNameJ);
            int idx = 0;
            for (auto s : scaleNames_) {
                if (s == name) {
                    setScaleIdx(idx);
                    return;
                }
                idx++;
            }
            setScaleIdx(0);
        }
    }

    void setScaleIdx(int idx) {
        refreshLeds_ |= scaleIdx_ != idx;
        scaleIdx_ = idx;
    }

    int getScaleIdx() { return scaleIdx_; }

    bool layoutChanged_ = false;
    bool refreshLeds_ = false;
    unsigned kg_c_ = 0, kg_r_ = 0;

    int rowM_ = 0, colM_ = 0;

    MsgQueue<float> ledQueue_;
    std::vector<std::string> scaleNames_;
    std::vector<scala::scale> scales_;
    int scaleIdx_ = 0;

    static constexpr unsigned MAX_LED_DEG = 3;
    struct {
        int deg_ = 0;
        int colour_ = 0;
    } scaleLeds_[MAX_LED_DEG];

    struct {
        int ref_note_ = -1;
        int scaleIdx_ = -1;
        int note_ = -1;
        float voct_ = 0;
    } freqCache_[16];

    int ref_note_ = 3;
    float baseFreq_ = 440.f;
};

struct LedSwitch : app::SvgSwitch {
    LedSwitch() {
        auto plugin = pluginInstance;

        shadow->opacity = 0.0;
        addFrame(Svg::load(asset::plugin(plugin, "res/components/led/LED_OFF.svg")));
        addFrame(Svg::load(asset::plugin(plugin, "res/components/led/LED_GREEN.svg")));
        addFrame(Svg::load(asset::plugin(plugin, "res/components/led/LED_RED.svg")));
        addFrame(Svg::load(asset::plugin(plugin, "res/components/led/LED_ORANGE.svg")));
    }
};

struct EScalerWidget : ModuleWidget {
    EScalerWidget(EScaler* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EScaler.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(22.285, 19.164)), module, EScaler::P_ROW_MULT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.819, 19.164)), module, EScaler::P_COL_MULT_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.993, 41.514)), module, EScaler::P_KEYPBR_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(24.527, 41.514)), module, EScaler::P_GLOBALPBR_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.289, 41.514)), module, EScaler::P_REF_NOTE_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(21.667, 63.84)), module, EScaler::P_LED1_IDX_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(34.202, 63.84)), module, EScaler::P_LED2_IDX_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(46.736, 63.84)), module, EScaler::P_LED3_IDX_PARAM));
        addParam(createParam<LedSwitch>(mm2px(Vec(17.603, 69.502)), module, EScaler::P_LED1_COLOUR_PARAM));
        addParam(createParam<LedSwitch>(mm2px(Vec(30.138, 69.502)), module, EScaler::P_LED2_COLOUR_PARAM));
        addParam(createParam<LedSwitch>(mm2px(Vec(42.672, 69.54)), module, EScaler::P_LED3_COLOUR_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.133, 87.98)), module, EScaler::IN_K_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.667, 87.98)), module, EScaler::IN_NOTE_PB_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.202, 87.98)), module, EScaler::IN_G_PB_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.736, 87.98)), module, EScaler::IN_KG_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.133, 107.454)), module, EScaler::OUT_VOCT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(46.705, 107.454)), module, EScaler::OUT_LIGHTS_OUTPUT));
    }

    void appendContextMenu(Menu* menu) override {
        EScaler* module = getModule<EScaler>();

        menu->addChild(new MenuSeparator);

        menu->addChild(createIndexSubmenuItem(
            "Scales", module->scaleNames_, [=]() { return module->getScaleIdx(); },
            [=](int idx) { module->setScaleIdx(idx); }));
    }
};

Model* modelEScaler = createModel<EScaler, EScalerWidget>("Scaler");

////////////////////////////////////////////////////////////////////////

void EScaler::doProcess(const ProcessArgs& args) {
    static constexpr float maxVoltPbV = 5.f;

    unsigned nChannels = std::max(1, inputs[IN_K_INPUT].getChannels());

    int rowM = params[P_ROW_MULT_PARAM].getValue();
    int colM = params[P_COL_MULT_PARAM].getValue();
    int ref_note = params[P_REF_NOTE_PARAM].getValue();

    static constexpr unsigned P_LED_N = P_LED2_IDX_PARAM - P_LED1_IDX_PARAM;

    auto& scale = scales_[scaleIdx_];
    int nDeg = scale.get_scale_length() - 1;

    for (unsigned i = 0; i < MAX_LED_DEG; i++) {
        int deg = params[P_LED1_IDX_PARAM + (P_LED_N * i)].getValue();
        int colour = params[P_LED1_COLOUR_PARAM + (P_LED_N * i)].getValue();
        refreshLeds_ |= (scaleLeds_[i].deg_ != deg) || (scaleLeds_[i].colour_ != colour);
        scaleLeds_[i].deg_ = deg;
        scaleLeds_[i].colour_ = colour;
    }

    if (colM_ != colM || rowM_ != rowM || ref_note != ref_note_) {
        layoutChanged_ = true;
        colM_ = colM;
        rowM_ = rowM;
        if (ref_note_ != ref_note) {
            ref_note_ = ref_note;
            const float A440_REF_FREQ = 440.f;
            // offset - = C, since this C=0v in VCV
            baseFreq_ = A440_REF_FREQ * pow(2.f, ((ref_note - 9) / 12.0));
        }
    }

    float xPBR = params[P_KEYPBR_PARAM].getValue() / 12.0f;  // v/oct
    float gPBR = params[P_GLOBALPBR_PARAM].getValue() / 12.0f;

    // assume a unipolar 10v scale?
    float global = inputs[IN_G_PB_INPUT].getVoltage() / maxVoltPbV;
    float globalPb = global * gPBR;

    float kg = inputs[IN_KG_INPUT].getVoltage();
    unsigned kg_r = 0, kg_c = 0;
    decodeKeyGroup(kg, kg_r, kg_c);
    if ((kg_c_ != kg_c) || (kg_r_ != kg_r)) {
        layoutChanged_ = true;
        kg_c_ = kg_c;
        kg_r_ = kg_r;
    }

    for (unsigned ch = 0; ch < nChannels; ch++) {
        float inKey = inputs[IN_K_INPUT].getVoltage(ch);

        unsigned r, c;
        bool valid = false;
        decodeKey(inKey, valid, r, c);

        if (valid) {
            int note = (r * rowM) + (c * colM);
            auto& freqCache = freqCache_[ch];

            float voct = 0.0f;
            if (freqCache.scaleIdx_ != scaleIdx_ || freqCache.ref_note_ != ref_note_ || freqCache.note_ != note) {
                int deg = 0;
                int oct = 0.0f;
                if (note >= 0) {
                    deg = note % nDeg;
                    oct = note / nDeg;
                } else {
                    oct = ((note + 1) / nDeg) - 1;
                    deg = abs(oct) * nDeg + note;
                }

                float baseNoteRatio = 1.0f;
                float rootFreq = pow(scale.get_ratio(nDeg), oct) * baseFreq_;
                float fracFreq = (scale.get_ratio(deg) - baseNoteRatio) * rootFreq;
                float freq = rootFreq + fracFreq;

                const float VOCT_C_0V_REF_FREQ = 261.626f;
                voct = log2(freq / VOCT_C_0V_REF_FREQ);

                freqCache.note_ = note;
                freqCache.scaleIdx_ = scaleIdx_;
                freqCache.ref_note_ = ref_note_;
                freqCache.voct_ = voct;

                // std::cerr << "midi " << note + 69 << " note " << note
                //           << " oct " << oct << " deg " << deg
                //           << " freq = " << freq << " voct " << voct
                //           << std::endl;
            } else {
                voct = freqCache.voct_;
            }
            float inX = inputs[IN_NOTE_PB_INPUT].getVoltage(ch) / maxVoltPbV;  // +-5v
            float xPb = (inX * abs(inX)) * xPBR;
            voct += xPb + globalPb;

            outputs[OUT_VOCT_OUTPUT].setVoltage(voct, ch);
        } else {
            // do not reset voltage on note off
        }
    }

    outputs[OUT_VOCT_OUTPUT].setChannels(nChannels);

    if (layoutChanged_) {
        layoutChanged_ = false;
        refreshLeds_ = true;
    }

    // lights handling..
    if (refreshLeds_) {
        // note: im not checking for queue overflow as not much I can do ;)
        ledQueue_.clear();
        float msg = encodeLedMsg(LED_SET_OFF, 0, 0, kg_r_, kg_c_);
        ledQueue_.write(msg);

        for (unsigned r = 0; r < kg_r_; r++) {
            for (unsigned c = 0; c < kg_c_; c++) {
                int note = (r * rowM) + (c * colM);
                int oct = 0;
                int deg = 0;
                if (note >= 0) {
                    deg = note % nDeg;
                    oct = note / nDeg;
                } else {
                    oct = ((note + 1) / nDeg) - 1;
                    deg = abs(oct) * nDeg + note;
                }

                for (unsigned i = 0; i < MAX_LED_DEG; i++) {
                    if (scaleLeds_[i].colour_ != LED_SET_OFF) {
                        if ((deg % nDeg) == scaleLeds_[i].deg_) {
                            float msg = encodeLedMsg((LedMsgType)scaleLeds_[i].colour_, r, c, 1, 1);
                            ledQueue_.write(msg);
                        }
                    }  // not off
                }      // check each deg
            }
        }
        refreshLeds_ = false;
    }

    float msg = 0.0f;
    // dont really need this check as empty queue leaves msg untouched.
    if (ledQueue_.read(msg)) {
        // DEBUG_LIGHT_MSG("scaler led ", msg);
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.0f);
    }
}