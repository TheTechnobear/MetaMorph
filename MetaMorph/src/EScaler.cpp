#include "plugin.hpp"


struct EScaler : Module {
	enum ParamId {
		P_ROW_MULT_PARAM,
		P_COL_MULT_PARAM,
		P_OFFSET_PARAM,
		P_KEYPBR_PARAM,
		P_GLOBALPBR_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_X_INPUT,
		IN_G_PB_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_VOCT_OUTPUT,
		OUT_LIGHTS_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EScaler() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_ROW_MULT_PARAM, -24.f, 24.f, 1.f, "xRow");
		configParam(P_COL_MULT_PARAM, -24.0f, 24.f, 4.f, "xCol");
		configParam(P_OFFSET_PARAM, 0.f, 60.f, 0.f, "Offset");
		configParam(P_KEYPBR_PARAM, 0.f, 48.f, 1.f, "");
		configParam(P_GLOBALPBR_PARAM, 0.f, 48.f, 12.f, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_G_PB_INPUT, "");
		configOutput(OUT_VOCT_OUTPUT, "");
		configOutput(OUT_LIGHTS_OUTPUT, "");
		
		paramQuantities[P_ROW_MULT_PARAM]->snapEnabled = true;
		paramQuantities[P_COL_MULT_PARAM]->snapEnabled = true;
		paramQuantities[P_OFFSET_PARAM]->snapEnabled = true;

		paramQuantities[P_KEYPBR_PARAM]->snapEnabled = true;
		paramQuantities[P_GLOBALPBR_PARAM]->snapEnabled = true;

	}

	void process(const ProcessArgs& args) override {
		unsigned nChannels = std::max(1, inputs[IN_K_INPUT].getChannels());

		// int keyInRow = params[P_NCOLS_PARAM].getValue();
		// int keyInCol = params[P_NROW_PARAM].getValue();
		// todp
		int keyInCol = 8;
		int rowM = params[P_ROW_MULT_PARAM].getValue();
		int colM = params[P_COL_MULT_PARAM].getValue();
		int offset = params[P_OFFSET_PARAM].getValue();
		float xPBR = params[P_KEYPBR_PARAM].getValue() / 12.0f; // v/oct
		float  gPBR = params[P_GLOBALPBR_PARAM].getValue() / 12.0f; 

		// assume a unipolar 10v scale?
		float global = (inputs[IN_G_PB_INPUT].getVoltage() - 5.0f) / 5.0f;

		float globalPb = global * gPBR;
		
		for(unsigned ch=0; ch < nChannels; ch++) {
			float inKey = inputs[IN_K_INPUT].getVoltage(ch);
			int k = int((inKey * 12.0f) + 0.25f);
		    // r = k % keyInCol , c = k / keyInCol, note = (r * rowM) + (c * colM) + offset
			int r = k % keyInCol;
			int c = k / keyInCol; 
			int note = (r * rowM) + (c * colM) + offset;
			float voct = float(note) / 12.0f;
			float inX = (inputs[IN_X_INPUT].getVoltage(ch) / 5.0f); // +-5v
			float xPb = (inX * inX) * ( xPBR / 12.0f);

			voct += xPb + globalPb;

			outputs[OUT_VOCT_OUTPUT].setVoltage(voct,ch);
		}

		outputs[OUT_VOCT_OUTPUT].setChannels(nChannels);
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

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.128, 26.981)), module, EScaler::P_ROW_MULT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(25.725, 26.981)), module, EScaler::P_COL_MULT_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(41.321, 26.981)), module, EScaler::P_OFFSET_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.52, 48.53)), module, EScaler::P_KEYPBR_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.116, 48.53)), module, EScaler::P_GLOBALPBR_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 101.282)), module, EScaler::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 101.282)), module, EScaler::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.86, 101.282)), module, EScaler::IN_G_PB_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 117.83)), module, EScaler::OUT_VOCT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 117.83)), module, EScaler::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelEScaler = createModel<EScaler, EScalerWidget>("Scaler");