#include "plugin.hpp"

#include "../EigenLite/eigenapi/eigenapi.h"

#include <iostream>

#include "EHarp.h"

#include "Encoding.h"
#include "LightWire.h"

struct EDevice : Module {
	enum ParamId {
		P_BASEPOLY_PARAM,
		P_PERCPOLY_PARAM,
		P_FUNCPOLY_PARAM,
		P_FILTERTYPE_PARAM,
		P_FILTERNUMBER_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_LIGHTS_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
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
		OUT_FK_OUTPUT,
		OUT_FG_OUTPUT,
		OUT_KG_FUNC_OUTPUT,
		OUT_BREATH_OUTPUT,
		OUT_S1_OUTPUT,
		OUT_S2_OUTPUT,
		OUT_P1_OUTPUT,
		OUT_P2_OUTPUT,
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
		configInput(IN_LIGHTS_INPUT, "");
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
		configOutput(OUT_FK_OUTPUT, "");
		configOutput(OUT_FG_OUTPUT, "");
		configOutput(OUT_KG_FUNC_OUTPUT, "");
		configOutput(OUT_BREATH_OUTPUT, "");
		configOutput(OUT_S1_OUTPUT, "");
		configOutput(OUT_S2_OUTPUT, "");
		configOutput(OUT_P1_OUTPUT, "");
		configOutput(OUT_P2_OUTPUT, "");


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
		if(harp_) {
			harp_->removeCallback(callback_.get());
			harp_->stop();
		}
	}


	void process(const ProcessArgs& args) override {

		maxMainVoices_ = params[P_BASEPOLY_PARAM].getValue();
		maxPercVoices_ = params[P_PERCPOLY_PARAM].getValue();

		if(	params[P_FILTERTYPE_PARAM].getValue() != filterType_
			|| params[P_FILTERNUMBER_PARAM].getValue() != filterDeviceNum_) {
				filterType_ = params[P_FILTERTYPE_PARAM].getValue();
				filterDeviceNum_ = params[P_FILTERNUMBER_PARAM].getValue();

				switch(filterType_)  {
					case 0:  {
						harp_->setDeviceFilter(false, 0);
						break;
					}
					case 1:  {
						harp_->setDeviceFilter(false, filterDeviceNum_);
						break;
					}
					case 2:  {
						harp_->setDeviceFilter(true, filterDeviceNum_);
						break;
					}
					default: 
						break;
				}
		}


		int rate = args.sampleRate / 1000; // really should be 2k, lets do a bit more
		float iRate = 1.0f / float(rate);
		if((args.frame % rate) == 0) {
			harp_->process(); // will hit callbacks
		}
		// to do - leds
		// pico we need to set led when key is made active
		// but this needs to be review complete when we have light inputs !

		// harp_->setLED(dev,course,key,3);


		{ // main voices
			unsigned nChannels = maxMainVoices_ < EHarp::MAX_VOICE ? maxMainVoices_ : EHarp::MAX_VOICE;
			auto& keys = harpData_.mainVoices_;

			for(unsigned voice=0;voice<nChannels;voice++) {
				auto& vdata = keys.voices_[voice];
				float pV = vdata.pV_.next(iRate);
				lights[voice].setBrightness(pV / 10.0f);

				float key = 0.0f;
				encodeKey(vdata.key_, key);
				outputs[OUT_K_OUTPUT].setVoltage(key, voice);
				outputs[OUT_X_OUTPUT].setVoltage(vdata.rV_.next(iRate) , voice);
				outputs[OUT_Y_OUTPUT].setVoltage(vdata.yV_.next(iRate) , voice);
				outputs[OUT_Z_OUTPUT].setVoltage(pV, voice);
			}

			outputs[OUT_K_OUTPUT].setChannels(nChannels);
			outputs[OUT_X_OUTPUT].setChannels(nChannels);
			outputs[OUT_Y_OUTPUT].setChannels(nChannels);
			outputs[OUT_Z_OUTPUT].setChannels(nChannels);
		}

		{ // perc voices
			unsigned nChannels = maxPercVoices_ < EHarp::MAX_VOICE ? maxPercVoices_ : EHarp::MAX_VOICE;
			auto& keys = harpData_.percVoices_;

			for(unsigned voice=0;voice<nChannels;voice++) {
				auto& vdata = keys.voices_[voice];
				float key = 0.0f;
				encodeKey(vdata.key_, key);
				outputs[OUT_PK_OUTPUT].setVoltage(key, voice);
				outputs[OUT_PX_OUTPUT].setVoltage(vdata.rV_.next(iRate) , voice);
				outputs[OUT_PY_OUTPUT].setVoltage(vdata.yV_.next(iRate) , voice);
				outputs[OUT_PZ_OUTPUT].setVoltage(vdata.pV_.next(iRate) , voice);
			}

			outputs[OUT_PK_OUTPUT].setChannels(nChannels);
			outputs[OUT_PX_OUTPUT].setChannels(nChannels);
			outputs[OUT_PY_OUTPUT].setChannels(nChannels);
			outputs[OUT_PZ_OUTPUT].setChannels(nChannels);
		}

		{ // func voices
			unsigned nChannels = maxFuncVoices_ < EHarp::MAX_VOICE ? maxFuncVoices_ : EHarp::MAX_VOICE;
			auto& keys = harpData_.funcVoices_;

			for(unsigned voice=0;voice<nChannels;voice++) {
				auto& vdata = keys.voices_[voice];
				float key = 0.0f;
				encodeKey(vdata.key_, key);
				outputs[OUT_FK_OUTPUT].setVoltage(key, voice);
				outputs[OUT_FG_OUTPUT].setVoltage(vdata.actV_.next(iRate), voice);
			}

			outputs[OUT_FK_OUTPUT].setChannels(nChannels);
			outputs[OUT_FG_OUTPUT].setChannels(nChannels);
		}

		outputs[OUT_BREATH_OUTPUT].setVoltage(harpData_.breathV_.next(iRate));
		outputs[OUT_S1_OUTPUT].setVoltage(harpData_.stripV_[0].next(iRate));
		outputs[OUT_S2_OUTPUT].setVoltage(harpData_.stripV_[1].next(iRate));
		outputs[OUT_P1_OUTPUT].setVoltage(harpData_.pedalV_[0].next(iRate));
		outputs[OUT_P2_OUTPUT].setVoltage(harpData_.pedalV_[1].next(iRate));


		// led handling 
		float ledmsg = inputs[IN_LIGHTS_INPUT].getVoltage();
		if(ledmsg!=0.0f) {
			LedMsgType t;
			unsigned r=0,c=0;
			decodeLedMsg(ledmsg,t,r,c);
			int k = c * 9 + r;

			switch(t) {
				case LED_CLEAR_ALL : {
					for (unsigned ir = 0; ir < r; ir++) {
						for (unsigned ic = 0; ic < c ; ic++) {
							int k = ic * 9 + ir;
							harp_->setLED(harpData_.lastDevice_,0, k,0);
						}
					}
					break;
				}
				case LED_SET_OFF : {
					harp_->setLED(harpData_.lastDevice_,0, k, 0);
					break;
				}
				case LED_SET_GREEN : {
					harp_->setLED(harpData_.lastDevice_,0, k, 1);
					break;
				}
				case LED_SET_RED : {
					harp_->setLED(harpData_.lastDevice_,0, k, 2);
					break;
				}
				case LED_SET_ORANGE : {
					harp_->setLED(harpData_.lastDevice_,0, k, 3);
					break;
				}
			}
		}


	}




	void processBypass (const ProcessArgs &args) override {
		int rate = args.sampleRate / 1000; // really should be 2k, lets do a bit more
		if((args.frame % rate) == 0) {
			harp_->process(); // will hit callbacks
		}
	}

	std::shared_ptr<EigenApi::Eigenharp> harp_;
	EigenApi::FWR_Embedded firmware_;
	EHarp harpData_;
	std::shared_ptr<EHarpCallback> callback_;

	unsigned maxMainVoices_ = EHarp::MAX_VOICE;
	unsigned maxPercVoices_ = EHarp::MAX_VOICE;
	unsigned maxFuncVoices_ = EHarp::MAX_VOICE;
	int filterType_=0;
	int filterDeviceNum_=0;
};


struct EDeviceWidget : ModuleWidget {
	EDeviceWidget(EDevice* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.859, 31.622)), module, EDevice::P_BASEPOLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.859, 47.497)), module, EDevice::P_PERCPOLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.859, 64.43)), module, EDevice::P_FUNCPOLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(7.591, 70.251)), module, EDevice::P_FILTERTYPE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(24.926, 70.251)), module, EDevice::P_FILTERNUMBER_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.0, 118.701)), module, EDevice::IN_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.898, 31.669)), module, EDevice::OUT_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.976, 31.669)), module, EDevice::OUT_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.054, 31.669)), module, EDevice::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(94.132, 31.669)), module, EDevice::OUT_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(109.984, 32.239)), module, EDevice::OUT_KG_MAIN_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.898, 47.544)), module, EDevice::OUT_PK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.976, 47.544)), module, EDevice::OUT_PX_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(82.054, 47.544)), module, EDevice::OUT_PY_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(94.132, 47.544)), module, EDevice::OUT_PZ_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(109.835, 47.646)), module, EDevice::OUT_KG_PERC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.613, 64.941)), module, EDevice::OUT_FK_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.691, 64.941)), module, EDevice::OUT_FG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.047, 64.941)), module, EDevice::OUT_KG_FUNC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(109.192, 64.726)), module, EDevice::OUT_BREATH_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.613, 81.13)), module, EDevice::OUT_S1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(69.691, 81.13)), module, EDevice::OUT_S2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(81.768, 81.13)), module, EDevice::OUT_P1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(93.846, 81.13)), module, EDevice::OUT_P2_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.278, 29.657)), module, EDevice::LED1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.457, 29.657)), module, EDevice::LED2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.636, 29.657)), module, EDevice::LED3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.816, 29.657)), module, EDevice::LED4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.278, 35.836)), module, EDevice::LED5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.457, 35.836)), module, EDevice::LED6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.636, 35.836)), module, EDevice::LED7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.816, 35.836)), module, EDevice::LED8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.278, 42.015)), module, EDevice::LED9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.457, 42.015)), module, EDevice::LED10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.636, 42.015)), module, EDevice::LED11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.816, 42.015)), module, EDevice::LED12_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(5.278, 48.195)), module, EDevice::LED13_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(11.457, 48.195)), module, EDevice::LED14_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.636, 48.195)), module, EDevice::LED15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(23.816, 48.195)), module, EDevice::LED16_LIGHT));
	}
	
	void appendContextMenu(Menu* menu) override {
		// EDevice* module = getModule<EDevice>();

		// menu->addChild(new MenuSeparator);

		// // Controls int Module::mode
		// menu->addChild(
		// 	createIndexPtrSubmenuItem("Max Voices",
		// 	{
		// 		"1","2","3","4","5", "6", "7", "8",
		// 		"9","10","11","12","13","14","15","16"
		// 	},
		// 	&module->maxVoices_
		// ));
	}
};


Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EigenHarp");