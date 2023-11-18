#include "plugin.hpp"


struct EIlluminator : Module {
	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
		IN_DISABLE_INPUT,
		IN_K_INPUT,
		IN_KG_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_LIGHTS_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EIlluminator() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configInput(IN_DISABLE_INPUT, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configOutput(OUT_LIGHTS_OUTPUT, "");
	}

    void processBypass(const ProcessArgs& args) override {
        doProcessBypass(args);
    }

    void process(const ProcessArgs& args) override {
        doProcess(args);
    }

    void doProcessBypass(const ProcessArgs& args) {}
	void doProcess(const ProcessArgs& args);

};


struct EIlluminatorWidget : ModuleWidget {
	EIlluminatorWidget(EIlluminator* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EIlluminator.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(14.978, 74.749)), module, EIlluminator::IN_DISABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.416, 93.865)), module, EIlluminator::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.826, 93.865)), module, EIlluminator::IN_KG_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(15.549, 118.115)), module, EIlluminator::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelEIlluminator = createModel<EIlluminator, EIlluminatorWidget>("EIlluminator");

/////////////////////////////////////////////////////////////////////////////

void EIlluminator::doProcess(const ProcessArgs& args) {

}