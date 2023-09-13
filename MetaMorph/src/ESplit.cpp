#include "plugin.hpp"


struct ESplit : Module {
	enum ParamId {
		P_S1_START_X_PARAM,
		P_S1_END_X_PARAM,
		P_S1_START_Y_PARAM,
		P_S1_END_Y_PARAM,
		P_S2_START_X_PARAM,
		P_S2_END_X_PARAM,
		P_S2_START_Y_PARAM,
		P_S2_END_Y_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_X_INPUT,
		IN_Y_INPUT,
		IN_Z_INPUT,
		IN_KG_INPUT,
		IN_ENABLE_INPUT,
		IN_LIGHTS_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
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
		OUT_LIGHTS_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ESplit() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_S1_START_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S1_END_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S1_START_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S1_END_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_START_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_END_X_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_START_Y_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_END_Y_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(IN_Z_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
		configInput(IN_LIGHTS_INPUT, "");
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
		configOutput(OUT_LIGHTS_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct ESplitWidget : ModuleWidget {
	ESplitWidget(ESplit* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ESplit.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.705, 29.101)), module, ESplit::P_S1_START_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(31.241, 28.959)), module, ESplit::P_S1_END_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.448, 29.101)), module, ESplit::P_S1_START_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.414, 28.959)), module, ESplit::P_S1_END_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.705, 47.622)), module, ESplit::P_S2_START_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(31.241, 47.48)), module, ESplit::P_S2_END_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.448, 47.622)), module, ESplit::P_S2_START_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.414, 47.48)), module, ESplit::P_S2_END_Y_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 63.712)), module, ESplit::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 63.712)), module, ESplit::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.86, 63.712)), module, ESplit::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.938, 63.712)), module, ESplit::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.366, 64.424)), module, ESplit::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.275, 118.359)), module, ESplit::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.788, 118.359)), module, ESplit::IN_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 82.376)), module, ESplit::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 82.376)), module, ESplit::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 82.376)), module, ESplit::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 82.376)), module, ESplit::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.056, 82.376)), module, ESplit::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 100.367)), module, ESplit::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 100.367)), module, ESplit::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 100.367)), module, ESplit::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 100.367)), module, ESplit::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.341, 100.635)), module, ESplit::OUT2_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(59.384, 118.359)), module, ESplit::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelESplit = createModel<ESplit, ESplitWidget>("ESplit");