#include "plugin.hpp"


struct EScaler : Module {
	enum ParamId {
		P_NROW_PARAM,
		P_NCOLS_PARAM,
		P_ROW_MULT_PARAM,
		P_COL_MULT_PARAM,
		P_OFFSET_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_X_INPUT,
		IN_Y_INPUT,
		IN_Z_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_K_OUTPUT,
		OUT_X_OUTPUT,
		OUT_Y_OUTPUT,
		OUT_Z_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EScaler() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_NROW_PARAM, 0.f, 24.f, 9.f, "nRow");
		configParam(P_NCOLS_PARAM, 0.f, 5.f, 2.f, "nCol");
		configParam(P_ROW_MULT_PARAM, -24.f, 24.f, 1.f, "xRow");
		configParam(P_COL_MULT_PARAM, -24.0f, 24.f, 4.f, "xCol");
		configParam(P_OFFSET_PARAM, 0.f, 60.f, 0.f, "Offset");
		configInput(IN_K_INPUT, "K");
		configInput(OUT_X_OUTPUT, "X");
		configInput(IN_Y_INPUT, "Y");
		configInput(IN_Z_INPUT, "Z");
		configOutput(OUT_K_OUTPUT, "K");
		configOutput(OUT_X_OUTPUT, "X");
		configOutput(OUT_Y_OUTPUT, "Y");
		configOutput(OUT_Z_OUTPUT, "Z");

		paramQuantities[P_NROW_PARAM]->snapEnabled = true;
		paramQuantities[P_NCOLS_PARAM]->snapEnabled = true;
		paramQuantities[P_ROW_MULT_PARAM]->snapEnabled = true;
		paramQuantities[P_COL_MULT_PARAM]->snapEnabled = true;
		paramQuantities[P_OFFSET_PARAM]->snapEnabled = true;
	}

	void process(const ProcessArgs& args) override {
		unsigned nChannels = std::max(1, inputs[IN_K_INPUT].getChannels());

		int keyInRow = params[P_NCOLS_PARAM].getValue();
		int keyInCol = params[P_NROW_PARAM].getValue();
		int rowM = params[P_ROW_MULT_PARAM].getValue();
		int colM = params[P_COL_MULT_PARAM].getValue();
		int offset = params[P_OFFSET_PARAM].getValue();
		
		for(unsigned ch=0; ch < nChannels; ch++) {
			float inKey = inputs[IN_K_INPUT].getVoltage(ch);
			int k = int((inKey * 12.0f) + 0.25f);
		    // r = k % keyInCol , c = k / keyInCol, note = (r * rowM) + (c * colM) + offset
			int r = k % keyInCol;
			int c = k / keyInCol; 
			int note = (r * rowM) + (c * colM) + offset;
			float outKey = float(note) / 12.0f;

			outputs[OUT_K_OUTPUT].setVoltage(outKey,ch);
			outputs[OUT_X_OUTPUT].setVoltage(inputs[OUT_X_OUTPUT].getVoltage(),ch);
			outputs[OUT_Y_OUTPUT].setVoltage(inputs[OUT_Y_OUTPUT].getVoltage(),ch);
			outputs[OUT_Z_OUTPUT].setVoltage(inputs[OUT_Z_OUTPUT].getVoltage(),ch);
		}

		outputs[OUT_K_OUTPUT].setChannels(nChannels);
		outputs[OUT_X_OUTPUT].setChannels(nChannels);
		outputs[OUT_Y_OUTPUT].setChannels(nChannels);
		outputs[OUT_Z_OUTPUT].setChannels(nChannels);		
	}
};


struct EScalerWidget : ModuleWidget {
	EScalerWidget(EScaler* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EScaler.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.128, 30.813)), module, EScaler::P_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.725, 30.813)), module, EScaler::P_NCOLS_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.128, 58.202)), module, EScaler::P_ROW_MULT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.725, 58.202)), module, EScaler::P_COL_MULT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.321, 58.202)), module, EScaler::P_OFFSET_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 101.282)), module, EScaler::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 101.282)), module, EScaler::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.86, 101.282)), module, EScaler::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.938, 101.282)), module, EScaler::IN_Z_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 117.83)), module, EScaler::OUT_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 117.83)), module, EScaler::OUT_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 117.83)), module, EScaler::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 117.83)), module, EScaler::OUT_Z_OUTPUT));
	}
};


Model* modelEScaler = createModel<EScaler, EScalerWidget>("Scaler");