#include "../EigenLite/eigenapi/eigenapi.h"
#include "EComponents.h"
#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"


constexpr int MAX_SLEW_IDX = 8;
constexpr float slewRates_[MAX_SLEW_IDX] = { 1.0f, 2.0f, 3.0f, 5.0f, 10.0f, 25.0f, 50.0f, 100.0f };


struct EDevice : Module {
    enum ParamId { P_FUNCPOLY_PARAM, P_BASEPOLY_PARAM, P_PERCPOLY_PARAM, PARAMS_LEN };
    enum InputId { IN_FUNC_LIGHTS_INPUT, IN_MAIN_LIGHTS_INPUT, IN_PERC_LIGHTS_INPUT, INPUTS_LEN };
    enum OutputId {
        OUT_S1A_OUTPUT,
        OUT_S1R_OUTPUT,
        OUT_S1G_OUTPUT,
        OUT_BREATH_OUTPUT,
        OUT_S2A_OUTPUT,
        OUT_S2R_OUTPUT,
        OUT_S2G_OUTPUT,
        OUT_P1_OUTPUT,
        OUT_P2_OUTPUT,
        OUT_FK_OUTPUT,
        OUT_FG_OUTPUT,
        OUT_KG_FUNC_OUTPUT,
        OUT_K_OUTPUT,
        OUT_X_OUTPUT,
        OUT_Y_OUTPUT,
        OUT_Z_OUTPUT,
        OUT_KG_MAIN_OUTPUT,
        OUT_PK_OUTPUT,
        OUT_PX_OUTPUT,
        OUT_PY_OUTPUT,
        OUT_PZ_OUTPUT,
        OUT_KG_PERC_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId { LED_MODE_LIGHT, LED_MAIN_LIGHT, LED_PERC_LIGHT, LIGHTS_LEN };

    EDevice() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configParam(P_BASEPOLY_PARAM, 0.f, 16.f, 16.f, "Main Poly");
        configParam(P_PERCPOLY_PARAM, 0.f, 16.f, 4.f, "Perc Poly");
        configParam(P_FUNCPOLY_PARAM, 0.f, 16.f, 4, "Mode Poly");

        configInput(IN_FUNC_LIGHTS_INPUT, "Mode LED");
        configInput(IN_MAIN_LIGHTS_INPUT, "Main LED");
        configInput(IN_PERC_LIGHTS_INPUT, "Perc LED");

        configOutput(OUT_S1A_OUTPUT, "Strip 1 Abs");
        configOutput(OUT_S1R_OUTPUT, "Strip 1 Rel");
        configOutput(OUT_S1G_OUTPUT, "Strip 1 Gate");

        configOutput(OUT_S2A_OUTPUT, "Strip 2 Abs");
        configOutput(OUT_S2R_OUTPUT, "Strip 2 Rel");
        configOutput(OUT_S2G_OUTPUT, "Strip 2 Gate");

        configOutput(OUT_P1_OUTPUT, "Pedal 1");
        configOutput(OUT_P2_OUTPUT, "Pedal 2");

        configOutput(OUT_BREATH_OUTPUT, "Breath");

        configOutput(OUT_FK_OUTPUT, "Mode Key");
        configOutput(OUT_FG_OUTPUT, "Mode Gate");
        configOutput(OUT_KG_FUNC_OUTPUT, "Mode KG");

        configOutput(OUT_K_OUTPUT, "Main Key");
        configOutput(OUT_X_OUTPUT, "Main X");
        configOutput(OUT_Y_OUTPUT, "Main Y");
        configOutput(OUT_Z_OUTPUT, "Main Z");
        configOutput(OUT_KG_MAIN_OUTPUT, "Main KG");

        configOutput(OUT_PK_OUTPUT, "Perc Key");
        configOutput(OUT_PX_OUTPUT, "Perc X");
        configOutput(OUT_PY_OUTPUT, "Perc Y");
        configOutput(OUT_PZ_OUTPUT, "Perc Z");
        configOutput(OUT_KG_PERC_OUTPUT, "Perg KG");

        paramQuantities[P_BASEPOLY_PARAM]->snapEnabled = true;
        paramQuantities[P_PERCPOLY_PARAM]->snapEnabled = true;
        paramQuantities[P_FUNCPOLY_PARAM]->snapEnabled = true;

        harp_ = std::make_shared<EigenApi::Eigenharp>(&firmware_);
        callback_ = std::make_shared<EHarpCallback>(harpData_);
        harp_->setPollTime(0);
        harp_->start();
        harp_->addCallback(callback_.get());
    }

    ~EDevice() {
        if (harp_) {
            harp_->removeCallback(callback_.get());
            harp_->stop();
        }
    }

    void processBypass(const ProcessArgs& args) override { doProcessBypass(args); }

    void process(const ProcessArgs& args) override { doProcess(args); }

    /////////////////////////////////////////////////////////////////////////////////////////

    void onPortChange(const PortChangeEvent& e) override {
        if (e.connecting) return;  // only interested in disconnecting

        switch (e.type) {
            case Port::INPUT: {
                switch (e.portId) {
                    case IN_FUNC_LIGHTS_INPUT: {
                        auto& q = ledQueue_[EHarp::KeyGroup::KG_FUNC];
                        q.clear();
                        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC];
                        q.write(encodeLedMsg(LED_SET_OFF, 0, 0, kg.r_, kg.c_));
                        break;
                    }
                    case IN_MAIN_LIGHTS_INPUT: {
                        auto& q = ledQueue_[EHarp::KeyGroup::KG_MAIN];
                        q.clear();
                        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN];
                        q.write(encodeLedMsg(LED_SET_OFF, 0, 0, kg.r_, kg.c_));
                        break;
                    }
                    case IN_PERC_LIGHTS_INPUT: {
                        auto& q = ledQueue_[EHarp::KeyGroup::KG_PERC];
                        q.clear();
                        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_PERC];
                        q.write(encodeLedMsg(LED_SET_OFF, 0, 0, kg.r_, kg.c_));
                        break;
                    }
                    default: break;
                }
                case Port::OUTPUT: {
                    break;
                }
            }
        }
    }

    void doProcessBypass(const ProcessArgs& args) {
        static constexpr int ESAMPERATE = 2000;
        int rate = args.sampleRate / ESAMPERATE;  // 48000k = 24
        if ((args.frame % rate) == 0) {
            harp_->process();  // will hit callbacks
        }
    }

    void doProcess(const ProcessArgs& args);

    void handleLedInput(float msg, unsigned kg) {
        if (msg != 0.0f) {
            LedMsgType t;
            unsigned r = 0, c = 0;
            unsigned k = 0, course = 0;

            unsigned startr, startc, sizer, sizec;
            decodeLedMsg(msg, t, startr, startc, sizer, sizec);
            for (r = startr; r < (startr + sizer); r++) {
                for (c = startc; c < (startc + sizec); c++) {
                    if (harpData_.translateRCtoK(kg, r, c, course, k)) {
                        // LED enum matches setLED
                        harp_->setLED(harpData_.lastDevice_.c_str(), course, k, EigenApi::Eigenharp::LedColour(t));
                    }
                }
            }
        }
    }

    json_t* dataToJson() override {
        json_t* root = json_object();
        json_object_set_new(root, "filterDeviceType", json_integer(filterDeviceType_));
        json_object_set_new(root, "filterDeviceNum", json_integer(filterDeviceNum_));
        json_object_set_new(root, "slewRateIdx", json_integer(slewRateIdx_));
        return root;
    }
    void dataFromJson(json_t* rootJ) override {
        auto jType = json_object_get(rootJ, "filterDeviceType");
        if (jType) { filterDeviceType_ = json_integer_value(jType); }

        auto jNum = json_object_get(rootJ, "filterDeviceNum");
        if (jNum) { filterDeviceNum_ = json_integer_value(jNum); }

        auto jSlew = json_object_get(rootJ, "slewRateIdx");
        if (jSlew) { slewRateIdx_ = json_integer_value(jSlew); }
    }

    std::shared_ptr<EigenApi::Eigenharp> harp_;
    EigenApi::FWR_Embedded firmware_;
    EHarp harpData_;
    std::shared_ptr<EHarpCallback> callback_;

    static constexpr unsigned MAX_LED_MSG = MsgQueue<float>::MAX_MSGS * 3;
    static constexpr unsigned LED_THROTTLE = 50;
    MsgQueue<float> ledQueue_[3] = { MAX_LED_MSG, MAX_LED_MSG, MAX_LED_MSG };

    unsigned maxMainVoices_ = EHarp::MAX_VOICE;
    unsigned maxPercVoices_ = EHarp::MAX_VOICE;
    unsigned maxFuncVoices_ = EHarp::MAX_VOICE;
    int filterDeviceType_ = 0;
    int filterDeviceNum_ = 0;
    int slewRateIdx_ = 0;
};

struct EDeviceWidget : ModuleWidget {
    EDeviceWidget(EDevice* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.08, 65.936)), module, EDevice::P_FUNCPOLY_PARAM));
        addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.08, 88.56)), module, EDevice::P_BASEPOLY_PARAM));
        addParam(createParamCentered<PolyCountParam>(mm2px(Vec(10.195, 111.106)), module, EDevice::P_PERCPOLY_PARAM));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.263, 62.57)), module, EDevice::IN_FUNC_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.259, 85.041)), module, EDevice::IN_MAIN_LIGHTS_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.264, 107.56)), module, EDevice::IN_PERC_LIGHTS_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.281, 22.164)), module, EDevice::OUT_S1A_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.462, 22.164)), module, EDevice::OUT_S1R_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.642, 22.164)), module, EDevice::OUT_S1G_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.049, 22.164)), module, EDevice::OUT_BREATH_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.304, 40.177)), module, EDevice::OUT_S2A_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.507, 40.199)), module, EDevice::OUT_S2R_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.665, 40.177)), module, EDevice::OUT_S2G_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.889, 40.177)), module, EDevice::OUT_P1_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.072, 40.177)), module, EDevice::OUT_P2_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.303, 62.571)), module, EDevice::OUT_FK_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.495, 62.571)), module, EDevice::OUT_FG_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.071, 62.57)), module, EDevice::OUT_KG_FUNC_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.299, 85.041)), module, EDevice::OUT_K_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.421, 85.041)), module, EDevice::OUT_X_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.544, 85.041)), module, EDevice::OUT_Y_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.945, 85.041)), module, EDevice::OUT_Z_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.069, 85.041)), module, EDevice::OUT_KG_MAIN_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.303, 107.56)), module, EDevice::OUT_PK_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(33.482, 107.56)), module, EDevice::OUT_PX_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.688, 107.56)), module, EDevice::OUT_PY_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.88, 107.56)), module, EDevice::OUT_PZ_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(70.072, 107.56)), module, EDevice::OUT_KG_PERC_OUTPUT));

        addChild(
            createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.405, 59.634)), module, EDevice::LED_MODE_LIGHT));
        addChild(
            createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.397, 82.128)), module, EDevice::LED_MAIN_LIGHT));
        addChild(
            createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(14.389, 104.634)), module, EDevice::LED_PERC_LIGHT));
    }


    void appendContextMenu(Menu* menu) override {
        auto module = getModule<EDevice>();

        menu->addChild(new MenuSeparator);

        std::vector<std::string> deviceTypes = { "Any", "Alpha/Tau only", "Pico only" };


        menu->addChild(createIndexSubmenuItem(
            "Device Type", deviceTypes, [=]() { return module->filterDeviceType_; },
            [=](int idx) { module->filterDeviceType_ = idx; }));

        std::vector<std::string> deviceNumber = { "All", "1", "2", "3", "4" };

        menu->addChild(createIndexSubmenuItem(
            "Device Number", deviceNumber, [=]() { return module->filterDeviceNum_; },
            [=](int idx) { module->filterDeviceNum_ = idx; }));


        std::vector<std::string> slewRates = { "1x",
                                               "2x",
                                               "3x",
                                               "5x",
                                               "10x",
                                               "25x",
                                               "50x",
                                               "100x" };
        assert(slewRates.size() == MAX_SLEW_IDX);


        menu->addChild(createIndexSubmenuItem(
            "Slew Rate", slewRates, [=]() { return module->slewRateIdx_; },
            [=](int idx) { module->slewRateIdx_ = idx < MAX_SLEW_IDX ? idx : MAX_SLEW_IDX - 1; }));
    }
};


Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EigenHarp");

///////////////////////////////////////////////////////////////////////////////////////////////

void EDevice::doProcess(const ProcessArgs& args) {
    maxMainVoices_ = params[P_BASEPOLY_PARAM].getValue();
    maxPercVoices_ = params[P_PERCPOLY_PARAM].getValue();
    maxFuncVoices_ = params[P_FUNCPOLY_PARAM].getValue();

    harp_->setDeviceFilter(filterDeviceType_, filterDeviceNum_);


    static constexpr int ESAMPERATE = 2000;
    int rate = args.sampleRate / ESAMPERATE;  // 48000k = 24
    float iRate = 1.0f / (float(rate) * slewRates_[slewRateIdx_]);
    if ((args.frame % rate) == 0) {
        harp_->process();  // will hit callbacks
    }

    {  // main voices
        unsigned nChannels = maxMainVoices_ < EHarp::MAX_VOICE ? maxMainVoices_ : EHarp::MAX_VOICE;
        auto& keys = harpData_.mainVoices_;

        bool ledActive = false;
        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];

            if (vdata.active_) {
                ledActive = true;
                float pV = vdata.pV_.next(iRate);
                float key = encodeKeyId(vdata.key_);
                outputs[OUT_K_OUTPUT].setVoltage(key, voice);
                outputs[OUT_X_OUTPUT].setVoltage(vdata.rV_.next(iRate), voice);
                outputs[OUT_Y_OUTPUT].setVoltage(vdata.yV_.next(iRate), voice);
                outputs[OUT_Z_OUTPUT].setVoltage(pV, voice);
            } else {
                outputs[OUT_K_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_X_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_Y_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_Z_OUTPUT].setVoltage(0.f, voice);
            }
        }

        outputs[OUT_K_OUTPUT].setChannels(nChannels);
        outputs[OUT_X_OUTPUT].setChannels(nChannels);
        outputs[OUT_Y_OUTPUT].setChannels(nChannels);
        outputs[OUT_Z_OUTPUT].setChannels(nChannels);

        lights[LED_MAIN_LIGHT].setBrightness(ledActive ? 1.0f : 0.0f);

        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN];
        outputs[OUT_KG_MAIN_OUTPUT].setVoltage(encodeKeyGroup(kg.r_, kg.c_));

        float ledmsg = inputs[IN_MAIN_LIGHTS_INPUT].getVoltage();

        // DEBUG_LIGHT_MSG("read device", ledmsg);
        if (ledmsg != 0.f) ledQueue_[EHarp::KeyGroup::KG_MAIN].write(ledmsg);
        // handleLedInput(ledmsg, EHarp::KeyGroup::KG_MAIN);
    }

    {  // perc voices
        unsigned nChannels = maxPercVoices_ < EHarp::MAX_VOICE ? maxPercVoices_ : EHarp::MAX_VOICE;
        auto& keys = harpData_.percVoices_;

        bool ledActive = false;
        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];

            if (vdata.active_) {
                ledActive = true;
                float key = encodeKeyId(vdata.key_);
                outputs[OUT_PK_OUTPUT].setVoltage(key, voice);
                outputs[OUT_PX_OUTPUT].setVoltage(vdata.rV_.next(iRate), voice);
                outputs[OUT_PY_OUTPUT].setVoltage(vdata.yV_.next(iRate), voice);
                outputs[OUT_PZ_OUTPUT].setVoltage(vdata.pV_.next(iRate), voice);
            } else {
                outputs[OUT_PK_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_PX_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_PY_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_PZ_OUTPUT].setVoltage(0.f, voice);
            }
        }

        outputs[OUT_PK_OUTPUT].setChannels(nChannels);
        outputs[OUT_PX_OUTPUT].setChannels(nChannels);
        outputs[OUT_PY_OUTPUT].setChannels(nChannels);
        outputs[OUT_PZ_OUTPUT].setChannels(nChannels);
        lights[LED_PERC_LIGHT].setBrightness(ledActive ? 1.0f : 0.0f);

        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_PERC];
        outputs[OUT_KG_PERC_OUTPUT].setVoltage(encodeKeyGroup(kg.r_, kg.c_));

        float ledmsg = inputs[IN_PERC_LIGHTS_INPUT].getVoltage();
        if (ledmsg != 0.f) ledQueue_[EHarp::KeyGroup::KG_PERC].write(ledmsg);
        // handleLedInput(ledmsg, EHarp::KeyGroup::KG_PERC);
    }

    {  // func voices
        bool ledActive = false;
        unsigned nChannels = maxFuncVoices_ < EHarp::MAX_VOICE ? maxFuncVoices_ : EHarp::MAX_VOICE;
        auto& keys = harpData_.funcVoices_;

        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];
            float key = encodeKeyId(vdata.key_);
            if (vdata.active_) {
                ledActive = true;
                outputs[OUT_FK_OUTPUT].setVoltage(key, voice);
                outputs[OUT_FG_OUTPUT].setVoltage(vdata.actV_.next(iRate), voice);
            } else {
                outputs[OUT_FK_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_FG_OUTPUT].setVoltage(0.f, voice);
            }
        }

        outputs[OUT_FK_OUTPUT].setChannels(nChannels);
        outputs[OUT_FG_OUTPUT].setChannels(nChannels);
        lights[LED_MODE_LIGHT].setBrightness(ledActive ? 1.0f : 0.0f);


        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC];
        outputs[OUT_KG_FUNC_OUTPUT].setVoltage(encodeKeyGroup(kg.r_, kg.c_));
        float ledmsg = inputs[IN_FUNC_LIGHTS_INPUT].getVoltage();
        if (ledmsg != 0.f) ledQueue_[EHarp::KeyGroup::KG_FUNC].write(ledmsg);
        // handleLedInput(ledmsg, EHarp::KeyGroup::KG_FUNC);
    }

    // throttle the number of leds changes.
    float ledmsg = 0.f;
    unsigned throttle = LED_THROTTLE;
    for (unsigned q = 0; q < 3; q++) {
        while (throttle > 0 && ledQueue_[q].read(ledmsg)) {
            handleLedInput(ledmsg, q);
            throttle--;
        }
    }

    outputs[OUT_BREATH_OUTPUT].setVoltage(harpData_.breathV_.next(iRate));

    outputs[OUT_S1A_OUTPUT].setVoltage(harpData_.stripAbs_[0].next(iRate));
    outputs[OUT_S1R_OUTPUT].setVoltage(harpData_.stripRel_[0].next(iRate));
    outputs[OUT_S1G_OUTPUT].setVoltage(harpData_.stripGate_[0] ? 10.0f : 0.0f);

    outputs[OUT_S2A_OUTPUT].setVoltage(harpData_.stripAbs_[1].next(iRate));
    outputs[OUT_S2R_OUTPUT].setVoltage(harpData_.stripRel_[1].next(iRate));
    outputs[OUT_S2G_OUTPUT].setVoltage(harpData_.stripGate_[1] ? 10.0f : 0.0f);

    outputs[OUT_P1_OUTPUT].setVoltage(harpData_.pedalV_[0].next(iRate));
    outputs[OUT_P2_OUTPUT].setVoltage(harpData_.pedalV_[1].next(iRate));
}
