#include "plugin.hpp"


struct ESwitch : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN_SELECT_INPUT,
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

	ESwitch() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN_SELECT_INPUT, "");
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


struct ESwitchWidget : ModuleWidget {
	ESwitchWidget(ESwitch* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ESwitch.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(36.223, 46.463)), module, ESwitch::IN_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 63.712)), module, ESwitch::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 63.712)), module, ESwitch::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.86, 63.712)), module, ESwitch::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.938, 63.712)), module, ESwitch::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61.459, 63.712)), module, ESwitch::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.275, 118.359)), module, ESwitch::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(45.375, 118.359)), module, ESwitch::IN_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 82.376)), module, ESwitch::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 82.376)), module, ESwitch::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 82.376)), module, ESwitch::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 82.376)), module, ESwitch::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.971, 82.376)), module, ESwitch::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 100.367)), module, ESwitch::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 100.367)), module, ESwitch::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 100.367)), module, ESwitch::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 100.367)), module, ESwitch::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.971, 100.367)), module, ESwitch::OUT2_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.971, 118.359)), module, ESwitch::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelESwitch = createModel<ESwitch, ESwitchWidget>("ESwitch");