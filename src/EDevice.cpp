#include "../EigenLite/eigenapi/eigenapi.h"
#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct EDevice : Module {
    enum ParamId {
        P_FILTERTYPE_PARAM,
        P_FILTERNUMBER_PARAM,
        P_FUNCPOLY_PARAM,
        P_BASEPOLY_PARAM,
        P_PERCPOLY_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        IN_FUNC_LIGHTS_INPUT,
        IN_MAIN_LIGHTS_INPUT,
        IN_PERC_LIGHTS_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        OUT_S1_OUTPUT,
        OUT_S2_OUTPUT,
        OUT_P1_OUTPUT,
        OUT_P2_OUTPUT,
        OUT_BREATH_OUTPUT,
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
    enum LightId {
        LED1_LIGHT,
        LED2_LIGHT,
        LED3_LIGHT,
        LED4_LIGHT,
        LED5_LIGHT,
        LED6_LIGHT,
        LED7_LIGHT,
        LED8_LIGHT,
        LED9_LIGHT,
        LED10_LIGHT,
        LED11_LIGHT,
        LED12_LIGHT,
        LED13_LIGHT,
        LED14_LIGHT,
        LED15_LIGHT,
        LED16_LIGHT,
        LIGHTS_LEN
    };

    EDevice() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(P_BASEPOLY_PARAM, 0.f, 16.f, 16.f, "");
        configParam(P_PERCPOLY_PARAM, 0.f, 16.f, 16.f, "");
        configParam(P_FUNCPOLY_PARAM, 0.f, 16.f, 4, "");
        configSwitch(P_FILTERTYPE_PARAM, 0.f, 2.f, 0.f, "Type", {"None", "Base", "Pico"});
        configParam(P_FILTERNUMBER_PARAM, 1.f, 5.f, 1.f, "Dev Num");
        configInput(IN_FUNC_LIGHTS_INPUT, "");
        configInput(IN_MAIN_LIGHTS_INPUT, "");
        configInput(IN_PERC_LIGHTS_INPUT, "");
        configOutput(OUT_S1_OUTPUT, "");
        configOutput(OUT_S2_OUTPUT, "");
        configOutput(OUT_P1_OUTPUT, "");
        configOutput(OUT_P2_OUTPUT, "");
        configOutput(OUT_BREATH_OUTPUT, "");
        configOutput(OUT_FK_OUTPUT, "");
        configOutput(OUT_FG_OUTPUT, "");
        configOutput(OUT_KG_FUNC_OUTPUT, "");
        configOutput(OUT_K_OUTPUT, "");
        configOutput(OUT_X_OUTPUT, "");
        configOutput(OUT_Y_OUTPUT, "");
        configOutput(OUT_Z_OUTPUT, "");
        configOutput(OUT_KG_MAIN_OUTPUT, "");
        configOutput(OUT_PK_OUTPUT, "");
        configOutput(OUT_PX_OUTPUT, "");
        configOutput(OUT_PY_OUTPUT, "");
        configOutput(OUT_PZ_OUTPUT, "");
        configOutput(OUT_KG_PERC_OUTPUT, "");

        paramQuantities[P_FILTERTYPE_PARAM]->snapEnabled = true;
        paramQuantities[P_FILTERNUMBER_PARAM]->snapEnabled = true;
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

    void processBypass(const ProcessArgs& args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs& args) override {
        doProcess(args);
    }

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
                    default:
                        break;
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

    std::shared_ptr<EigenApi::Eigenharp> harp_;
    EigenApi::FWR_Embedded firmware_;
    EHarp harpData_;
    std::shared_ptr<EHarpCallback> callback_;

    static constexpr unsigned MAX_LED_MSG = MsgQueue<float>::MAX_MSGS * 3;
    static constexpr unsigned LED_THROTTLE = 50;
    MsgQueue<float> ledQueue_[3] = {MAX_LED_MSG, MAX_LED_MSG, MAX_LED_MSG};

    unsigned maxMainVoices_ = EHarp::MAX_VOICE;
    unsigned maxPercVoices_ = EHarp::MAX_VOICE;
    unsigned maxFuncVoices_ = EHarp::MAX_VOICE;
    int filterType_ = 0;
    int filterDeviceNum_ = 0;
};

struct EDeviceWidget : ModuleWidget {
    EDeviceWidget(EDevice* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.3, 26.287)), module, EDevice::P_FILTERTYPE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(28.635, 26.287)), module, EDevice::P_FILTERNUMBER_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.088, 77.319)), module, EDevice::P_FUNCPOLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.047, 97.531)), module, EDevice::P_BASEPOLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(11.047, 118.169)), module, EDevice::P_PERCPOLY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(63.211, 77.953)), module, EDevice::IN_FUNC_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(93.453, 97.878)), module, EDevice::IN_MAIN_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(93.453, 118.516)), module, EDevice::IN_PERC_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(27.656, 55.306)), module, EDevice::OUT_S1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.734, 55.306)), module, EDevice::OUT_S2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.812, 55.306)), module, EDevice::OUT_P1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(63.889, 55.306)), module, EDevice::OUT_P2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(78.379, 55.164)), module, EDevice::OUT_BREATH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(27.371, 77.831)), module, EDevice::OUT_FK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.449, 77.831)), module, EDevice::OUT_FG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(49.805, 77.831)), module, EDevice::OUT_KG_FUNC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(27.085, 97.578)), module, EDevice::OUT_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.163, 97.578)), module, EDevice::OUT_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.241, 97.578)), module, EDevice::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(63.319, 97.578)), module, EDevice::OUT_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(79.171, 98.149)), module, EDevice::OUT_KG_MAIN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(27.085, 118.216)), module, EDevice::OUT_PK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.163, 118.216)), module, EDevice::OUT_PX_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(51.241, 118.216)), module, EDevice::OUT_PY_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(63.319, 118.216)), module, EDevice::OUT_PZ_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(79.023, 118.318)), module, EDevice::OUT_KG_PERC_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.7, 20.786)), module, EDevice::LED1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(78.88, 20.786)), module, EDevice::LED2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.059, 20.786)), module, EDevice::LED3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(91.238, 20.786)), module, EDevice::LED4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.7, 26.965)), module, EDevice::LED5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(78.88, 26.965)), module, EDevice::LED6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.059, 26.965)), module, EDevice::LED7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(91.238, 26.965)), module, EDevice::LED8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.7, 33.144)), module, EDevice::LED9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(78.88, 33.144)), module, EDevice::LED10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.059, 33.144)), module, EDevice::LED11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(91.238, 33.144)), module, EDevice::LED12_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(72.7, 39.323)), module, EDevice::LED13_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(78.88, 39.323)), module, EDevice::LED14_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(85.059, 39.323)), module, EDevice::LED15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(91.238, 39.323)), module, EDevice::LED16_LIGHT));
    }
};

Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EigenHarp");

///////////////////////////////////////////////////////////////////////////////////////////////

void EDevice::doProcess(const ProcessArgs& args) {
    maxMainVoices_ = params[P_BASEPOLY_PARAM].getValue();
    maxPercVoices_ = params[P_PERCPOLY_PARAM].getValue();
    maxFuncVoices_ = params[P_FUNCPOLY_PARAM].getValue();

    if (params[P_FILTERTYPE_PARAM].getValue() != filterType_ || params[P_FILTERNUMBER_PARAM].getValue() != filterDeviceNum_) {
        filterType_ = params[P_FILTERTYPE_PARAM].getValue();
        filterDeviceNum_ = params[P_FILTERNUMBER_PARAM].getValue();

        switch (filterType_) {
            case 0: {
                harp_->setDeviceFilter(false, 0);
                break;
            }
            case 1: {
                harp_->setDeviceFilter(false, filterDeviceNum_);
                break;
            }
            case 2: {
                harp_->setDeviceFilter(true, filterDeviceNum_);
                break;
            }
            default:
                break;
        }
    }

    static constexpr int ESAMPERATE = 2000;
    int rate = args.sampleRate / ESAMPERATE;  // 48000k = 24
    float iRate = 1.0f / float(rate);
    if ((args.frame % rate) == 0) {
        harp_->process();  // will hit callbacks
    }

    {  // main voices
        unsigned nChannels = maxMainVoices_ < EHarp::MAX_VOICE ? maxMainVoices_ : EHarp::MAX_VOICE;
        auto& keys = harpData_.mainVoices_;

        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];

            if (vdata.active_) {
                float pV = vdata.pV_.next(iRate);
                lights[voice].setBrightness(pV / 10.0f);

                float key = encodeKeyId(vdata.key_);
                outputs[OUT_K_OUTPUT].setVoltage(key, voice);
                outputs[OUT_X_OUTPUT].setVoltage(vdata.rV_.next(iRate), voice);
                outputs[OUT_Y_OUTPUT].setVoltage(vdata.yV_.next(iRate), voice);
                outputs[OUT_Z_OUTPUT].setVoltage(pV, voice);
            } else {
                lights[voice].setBrightness(0.f);
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

        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];

            if (vdata.active_) {
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

        auto& kg = harpData_.keygroups_[EHarp::KeyGroup::KG_PERC];
        outputs[OUT_KG_PERC_OUTPUT].setVoltage(encodeKeyGroup(kg.r_, kg.c_));

        float ledmsg = inputs[IN_PERC_LIGHTS_INPUT].getVoltage();
        if (ledmsg != 0.f) ledQueue_[EHarp::KeyGroup::KG_PERC].write(ledmsg);
        // handleLedInput(ledmsg, EHarp::KeyGroup::KG_PERC);
    }

    {  // func voices
        unsigned nChannels = maxFuncVoices_ < EHarp::MAX_VOICE ? maxFuncVoices_ : EHarp::MAX_VOICE;
        auto& keys = harpData_.funcVoices_;

        for (unsigned voice = 0; voice < nChannels; voice++) {
            auto& vdata = keys.voices_[voice];
            float key = encodeKeyId(vdata.key_);
            if (vdata.active_) {
                outputs[OUT_FK_OUTPUT].setVoltage(key, voice);
                outputs[OUT_FG_OUTPUT].setVoltage(vdata.actV_.next(iRate), voice);
            } else {
                outputs[OUT_FK_OUTPUT].setVoltage(0.f, voice);
                outputs[OUT_FG_OUTPUT].setVoltage(0.f, voice);
            }
        }

        outputs[OUT_FK_OUTPUT].setChannels(nChannels);
        outputs[OUT_FG_OUTPUT].setChannels(nChannels);

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

    outputs[OUT_S1_OUTPUT].setVoltage(harpData_.stripV_[0].next(iRate));
    outputs[OUT_S2_OUTPUT].setVoltage(harpData_.stripV_[1].next(iRate));

    outputs[OUT_P1_OUTPUT].setVoltage(harpData_.pedalV_[0].next(iRate));
    outputs[OUT_P2_OUTPUT].setVoltage(harpData_.pedalV_[1].next(iRate));
}