#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct LedMerge : Module {
    enum ParamId { PARAMS_LEN };
    enum InputId { IN1_LIGHTS_INPUT, IN2_LIGHTS_INPUT, IN3_LIGHTS_INPUT, IN4_LIGHTS_INPUT, IN_KG_INPUT, INPUTS_LEN };
    enum OutputId { OUT_LIGHTS_OUTPUT, OUTPUTS_LEN };
    enum LightId { LIGHTS_LEN };

    LedMerge() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configInput(IN1_LIGHTS_INPUT, "");
        configInput(IN2_LIGHTS_INPUT, "");
        configInput(IN3_LIGHTS_INPUT, "");
        configInput(IN4_LIGHTS_INPUT, "");
        configInput(IN_KG_INPUT, "");
        configOutput(OUT_LIGHTS_OUTPUT, "");
    }

    void process(const ProcessArgs& args) override { doProcess(args); }

    void doProcess(const ProcessArgs& args);

    static constexpr unsigned MAX_R = 24;
    static constexpr unsigned MAX_C = 5;
    unsigned sizeR_ = 0, sizeC_ = 0;

    void setLed(LedMsgType t, unsigned startr, unsigned startc, unsigned sizer, unsigned sizec);
    void clearLeds();

    LedMsgType ledState_[MAX_R][MAX_C];
    bool layoutChanged_ = false;
    bool refreshLeds_ = false;

    MsgQueue<float> ledQueue_;
};


struct LedMergeWidget : ModuleWidget {
    LedMergeWidget(LedMerge* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/ELedMerge.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.675, 28.944)), module, LedMerge::IN1_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.675, 44.289)), module, LedMerge::IN2_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.675, 59.635)), module, LedMerge::IN3_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.675, 74.452)), module, LedMerge::IN4_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.636, 104.359)), module, LedMerge::IN_KG_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.41, 104.359)), module, LedMerge::OUT_LIGHTS_OUTPUT));
    }
};


Model* modelLedMerge = createModel<LedMerge, LedMergeWidget>("LedMerge");


void LedMerge::setLed(LedMsgType t, unsigned startr, unsigned startc, unsigned sizer, unsigned sizec) {
    unsigned r = 0, c = 0;
    for (r = startr; r < sizeR_ && r < (startr + sizer); r++) {
        for (c = startc; c < sizeC_ && c < (startc + sizec); c++) { ledState_[r][c] = t; }
    }
}

void LedMerge::clearLeds() {
    for (unsigned r = 0; r < MAX_R; r++) {
        for (unsigned c = 0; c < MAX_R; c++) { ledState_[r][c] = LED_SET_OFF; }
    }
}

void LedMerge::doProcess(const ProcessArgs& args) {
    unsigned in_kg_r, in_kg_c;
    float kgMsg = inputs[IN_KG_INPUT].getVoltage();
    decodeKeyGroup(kgMsg, in_kg_r, in_kg_c);

    in_kg_c = in_kg_c <= MAX_C ? in_kg_c : MAX_C;
    in_kg_r = in_kg_r <= MAX_R ? in_kg_r : MAX_R;

    layoutChanged_ = (in_kg_c != sizeC_) || (in_kg_r != in_kg_r);
    sizeC_ = in_kg_c;
    sizeR_ = in_kg_r;


    int n = (IN4_LIGHTS_INPUT - IN1_LIGHTS_INPUT) + 1;
    for (int i = 0; i < n; i++) {
        float ledmsg = inputs[IN1_LIGHTS_INPUT + i].getVoltage();
        if (ledmsg != 0.0f) {
            unsigned startr, startc, sizer, sizec;
            LedMsgType t;
            decodeLedMsg(ledmsg, t, startr, startc, sizer, sizec);
            setLed(t, startr, startc, sizer, sizec);
            float msg = encodeLedMsg(t, startr, startc, sizer, sizec);
            ledQueue_.write(msg);
        }
    }

    if (layoutChanged_) {
        layoutChanged_ = false;
        refreshLeds_ = true;
    }

    if (refreshLeds_) {
        float msg = encodeLedMsg(LED_SET_OFF, 0, 0, in_kg_r, in_kg_c);
        ledQueue_.write(msg);
        for (unsigned r = 0; r < sizeR_; r++) {
            for (unsigned c = 0; c < sizeC_; c++) {
                LedMsgType t = ledState_[r][c];
                if (t != LED_SET_OFF) {
                    msg = encodeLedMsg(t, r, c, 1, 1);
                    ledQueue_.write(msg);
                }
            }
        }
    }
    refreshLeds_ = false;

    // dont really need this check as empty queue leaves msg untouched.
    float msg = 0.0f;
    if (ledQueue_.read(msg)) {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
    } else {
        outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
    }
}
