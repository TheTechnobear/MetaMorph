#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"
#include "plugin.hpp"

struct ESwitch4 : Module {
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
		IN_SELECT_INPUT,
		IN_ENABLE_INPUT,
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
	enum LightId {
		LIGHTS_LEN
	};

	ESwitch4() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_S1_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S1_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S3_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S3_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S4_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S4_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_SELECT_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
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
	}

	void process(const ProcessArgs& args) override {
	}
};


struct ESwitch4Widget : ModuleWidget {
	ESwitch4Widget(ESwitch4* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ESwitch4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 58.215)), module, ESwitch4::P_S1_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 58.215)), module, ESwitch4::P_S1_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 77.265)), module, ESwitch4::P_S2_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 77.794)), module, ESwitch4::P_S2_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 96.844)), module, ESwitch4::P_S3_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 97.373)), module, ESwitch4::P_S3_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 115.894)), module, ESwitch4::P_S4_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 116.423)), module, ESwitch4::P_S4_POLY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.978, 32.135)), module, ESwitch4::IN_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.763, 32.772)), module, ESwitch4::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.355, 32.772)), module, ESwitch4::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.433, 32.772)), module, ESwitch4::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61.51, 32.772)), module, ESwitch4::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(73.588, 32.772)), module, ESwitch4::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.195, 32.772)), module, ESwitch4::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 58.215)), module, ESwitch4::IN1_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 77.794)), module, ESwitch4::IN2_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 97.373)), module, ESwitch4::IN3_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 116.423)), module, ESwitch4::IN4_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(98.285, 32.772)), module, ESwitch4::OUT_LIGHTS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 58.215)), module, ESwitch4::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 58.215)), module, ESwitch4::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 58.215)), module, ESwitch4::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 58.215)), module, ESwitch4::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 58.215)), module, ESwitch4::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 77.794)), module, ESwitch4::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 77.794)), module, ESwitch4::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 77.794)), module, ESwitch4::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 77.794)), module, ESwitch4::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 77.794)), module, ESwitch4::OUT2_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 97.373)), module, ESwitch4::OUT3_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 97.373)), module, ESwitch4::OUT3_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 97.373)), module, ESwitch4::OUT3_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 97.373)), module, ESwitch4::OUT3_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 97.373)), module, ESwitch4::OUT3_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 116.423)), module, ESwitch4::OUT4_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 116.423)), module, ESwitch4::OUT4_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 116.423)), module, ESwitch4::OUT4_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 116.423)), module, ESwitch4::OUT4_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 116.423)), module, ESwitch4::OUT4_KG_OUTPUT));
	}
};


Model* modelESwitch4 = createModel<ESwitch4, ESwitch4Widget>("ESwitch4");