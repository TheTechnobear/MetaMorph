#include "plugin.hpp"

#include "../EigenLite/eigenapi/eigenapi.h"

struct EDevice : Module {
	enum ParamId {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT2_OUTPUT,
		OUT1_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EDevice() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM2_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN1_INPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");

		EigenApi::FWR_Embedded reader;
		auto harp = new EigenApi::Eigenharp(reader);
		harp->start();
	}

	void process(const ProcessArgs& args) override {
	}
};


struct EDeviceWidget : ModuleWidget {
	EDeviceWidget(EDevice* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));

		

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.556, 28.958)), module, EDevice::PARAM1_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(27.96, 54.065)), module, EDevice::PARAM2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.702, 105.562)), module, EDevice::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.684, 118.686)), module, EDevice::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.272, 118.971)), module, EDevice::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.097, 118.971)), module, EDevice::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.223, 119.256)), module, EDevice::OUT4_OUTPUT));
	}
};


Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EDevice");