#include "plugin.hpp"


struct KeyMap : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_X_INPUT,
		IN_Y_INPUT,
		IN_Z_INPUT,
		IN_KG_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_K_OUTPUT,
		OUT_X_OUTPUT,
		OUT_Y_OUTPUT,
		OUT_Z_OUTPUT,
		OUT_KG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	KeyMap() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(IN_Z_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configOutput(OUT_K_OUTPUT, "");
		configOutput(OUT_X_OUTPUT, "");
		configOutput(OUT_Y_OUTPUT, "");
		configOutput(OUT_Z_OUTPUT, "");
		configOutput(OUT_KG_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		doProcess(args);
	}

	void doProcess(const ProcessArgs& args);
};


struct KeyMapWidget : ModuleWidget {
	KeyMapWidget(KeyMap* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EKeyMap.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.404, 19.163)), module, KeyMap::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.382, 37.949)), module, KeyMap::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.382, 51.971)), module, KeyMap::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.382, 65.201)), module, KeyMap::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.404, 81.076)), module, KeyMap::IN_KG_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.264, 19.154)), module, KeyMap::OUT_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.996, 37.939)), module, KeyMap::OUT_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.029, 51.698)), module, KeyMap::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.05, 65.456)), module, KeyMap::OUT_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(25.349, 80.802)), module, KeyMap::OUT_KG_OUTPUT));
	}
};


Model* modelKeyMap = createModel<KeyMap, KeyMapWidget>("KeyMap");


void KeyMap::doProcess(const ProcessArgs& args) {
	// partner to illuminator
	// initially no led support, as it works with illuminator
	// in the future LED could possibly do reverse mapping
	// but thats tricky, as you can get duplicates

	// load a surface from a json file.
	// simply remap the KEY input
	// x, y and z remain unchanged, just pass thru 

	// struct co_ord{ int r, c;}
	// std::vector<std::vector<co_ord>> map_;
	
	// surface file is same structure
	// key group size is specified in surface file
	// so a list (rows) of list (cols in row) of pairs (new r,c)
	// repeat, like illuminator, allows pattern to repeat row list.

	// anything outside this is ignored!


}
