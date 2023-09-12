#include "plugin.hpp"


struct EFunction : Module {
	enum ParamId {
		P_F1_X_PARAM,
		P_F2_X_PARAM,
		P_F1_Y_PARAM,
		P_F2_Y_PARAM,
		P_F3_X_PARAM,
		P_F4_X_PARAM,
		P_F3_Y_PARAM,
		P_F4_Y_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_X_INPUT,
		IN_ENABLE_INPUT,
		IN_LIGHTS_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_F1_OUTPUT,
		OUT_F2_OUTPUT,
		OUT_F3_OUTPUT,
		OUT_F4_OUTPUT,
		OUT_LIGHTS_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EFunction() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_F1_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F2_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F1_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F2_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F3_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F4_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F3_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_F4_Y_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
		configInput(IN_LIGHTS_INPUT, "");
		configOutput(OUT_F1_OUTPUT, "");
		configOutput(OUT_F2_OUTPUT, "");
		configOutput(OUT_F3_OUTPUT, "");
		configOutput(OUT_F4_OUTPUT, "");
		configOutput(OUT_LIGHTS_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct EFunctionWidget : ModuleWidget {
	EFunctionWidget(EFunction* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EFunction.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.175, 29.101)), module, EFunction::P_F1_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(31.77, 28.959)), module, EFunction::P_F2_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.919, 29.101)), module, EFunction::P_F1_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.943, 28.959)), module, EFunction::P_F2_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.175, 49.209)), module, EFunction::P_F3_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(31.77, 49.067)), module, EFunction::P_F4_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(18.919, 49.209)), module, EFunction::P_F3_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.943, 49.067)), module, EFunction::P_F4_Y_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 80.645)), module, EFunction::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 80.645)), module, EFunction::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.275, 118.359)), module, EFunction::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.029, 118.359)), module, EFunction::IN_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 100.367)), module, EFunction::OUT_F1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 100.367)), module, EFunction::OUT_F2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 100.367)), module, EFunction::OUT_F3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 100.367)), module, EFunction::OUT_F4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 118.359)), module, EFunction::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelEFunction = createModel<EFunction, EFunctionWidget>("EFunction");