#include "plugin.hpp"

#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"

// #include <iostream>

struct EFunction : Module {
	enum ParamId {
		P_F1_X_PARAM,
		P_F1_Y_PARAM,
		P_F2_X_PARAM,
		P_F2_Y_PARAM,
		P_F3_X_PARAM,
		P_F3_Y_PARAM,
		P_F4_X_PARAM,
		P_F4_Y_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_K_INPUT,
		IN_GATE_INPUT,
		IN_KG_INPUT,
		IN_ENABLE_INPUT,
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
		configParam(P_F1_X_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_F1_Y_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_F2_X_PARAM, 0.f, 24.f, 1.f, "");
		configParam(P_F2_Y_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_F3_X_PARAM, 0.f, 24.f, 2.f, "");
		configParam(P_F3_Y_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_F4_X_PARAM, 0.f, 24.f, 3.f, "");
		configParam(P_F4_Y_PARAM, 0.f, 24.f, 0.f, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_GATE_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
		configOutput(OUT_F1_OUTPUT, "");
		configOutput(OUT_F2_OUTPUT, "");
		configOutput(OUT_F3_OUTPUT, "");
		configOutput(OUT_F4_OUTPUT, "");
		configOutput(OUT_LIGHTS_OUTPUT, "");


		paramQuantities[P_F1_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_F1_Y_PARAM]->snapEnabled = true;				
		paramQuantities[P_F2_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_F2_Y_PARAM]->snapEnabled = true;				
		paramQuantities[P_F3_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_F3_Y_PARAM]->snapEnabled = true;				
		paramQuantities[P_F4_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_F4_Y_PARAM]->snapEnabled = true;				
	}

	void processBypass (const ProcessArgs &args) override {
		doProcessBypass(args);
	}

	void process(const ProcessArgs& args) override {
		doProcess(args);
	}

	void doProcessBypass (const ProcessArgs &args)  {
	}

	void doProcess(const ProcessArgs& args)  {

		unsigned in_kg_r, in_kg_c;
		float kgMsg = inputs[IN_KG_INPUT].getVoltage();
		decodeKeyGroup(kgMsg,in_kg_r,in_kg_c);

		layoutChanged_ = false;
		layoutChanged_ |= funcs_[0].updateKey(params[P_F1_Y_PARAM].getValue(), params[P_F1_X_PARAM].getValue());
		layoutChanged_ |= funcs_[1].updateKey(params[P_F2_Y_PARAM].getValue(), params[P_F2_X_PARAM].getValue());
		layoutChanged_ |= funcs_[2].updateKey(params[P_F3_Y_PARAM].getValue(), params[P_F3_X_PARAM].getValue());
		layoutChanged_ |= funcs_[3].updateKey(params[P_F4_Y_PARAM].getValue(), params[P_F4_X_PARAM].getValue());

		if(in_kg_r != kg_r_ || in_kg_c != kg_c_) {
			layoutChanged_ |= true;
			kg_r_ = in_kg_r;
			kg_c_ = in_kg_c;
		}

		if(layoutChanged_) {
			float msg = encodeLedMsg(LED_SET_OFF, 0,0, kg_r_ , kg_c_);
			// std::cout << "layout clear leds " << kg_r_ << "," << kg_c_ << std::endl;
			ledQueue_.write(msg);

			for(unsigned fk = 0; fk < MAX_FUNCS; fk++) {
				auto& func= funcs_[fk];
				unsigned r = func.r_;
				unsigned c = func.c_;
				LedMsgType t = func.state_ ? LED_SET_GREEN : LED_SET_ORANGE;
				if(r < kg_r_ && c < kg_c_) {
					// std::cout << "layout led " << r << "," << c << " state" << t << std::endl;
					float msg = encodeLedMsg(t, r, c, 1 , 1);
				 	ledQueue_.write(msg);
				}
			}
			layoutChanged_ = false;
		}

		unsigned nChannels = inputs[IN_K_INPUT].getChannels();
		for(unsigned ch=0; ch < nChannels; ch++) {
			unsigned in_r=0, in_c=0;
			bool valid= ! (inputs[IN_ENABLE_INPUT].getVoltage()  > 2.0f);
			if(valid) {
				decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c);
			}
			
			for(unsigned fk = 0; fk < MAX_FUNCS; fk++) {
				auto& func= funcs_[fk];
				if(valid) {
					if(in_r == func.r_ && in_c==func.c_) {
						// trig @ 1..2v
						bool state  = (inputs[IN_GATE_INPUT].getVoltage() >= 1.5f);
						if(func.state_!=state) {
							LedMsgType t = state ? LED_SET_GREEN : LED_SET_ORANGE;
							// std::cout << "change led " << in_r << "," << in_c << " state" << t << std::endl;
							float msg = encodeLedMsg(t, in_r, in_c, 1 , 1);
							ledQueue_.write(msg);
						}
						func.state_ = state;
						func.ch_ = ch;
					}
				} else {
					if(func.ch_ == ch) {
						// key now not active
						func.ch_ = 0xff;
						func.state_ = false;

						unsigned r = func.r_;
						unsigned c = func.c_;
						if(r < kg_r_ && c < kg_c_) {
							// std::cout << "revert led " << r << "," << c << " orange" << std::endl;
							float msg = encodeLedMsg(LED_SET_ORANGE, r, c, 1 , 1);
							ledQueue_.write(msg);
						}
					} 
				}
			}
		} // channel

		for(unsigned fk = 0; fk < MAX_FUNCS; fk++) {
			auto& func= funcs_[fk];
			outputs[OUT_F1_OUTPUT+fk].setVoltage(func.state_ * 10.f); //  gate @ 10v
		}

		float msg=0.0f;
		if(ledQueue_.read(msg)) {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
		} else {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
		}

	}


	static constexpr int MAX_MSGS=10;
	MsgQueue<float> ledQueue_=MAX_MSGS;

	bool layoutChanged_=false;
	unsigned kg_r_=0,kg_c_=0;

	static constexpr unsigned MAX_FUNCS=4;
	struct FuncKey {
		bool state_ = false;
		unsigned r_ = 0, c_ = 0;
		unsigned ch_ = 0xff;

		bool updateKey(unsigned r, unsigned c) {
			if(r!=r_ || c_!=c) {
				r_=r;
				c_=c;
				return true;
			}
			return false;
		}
	} funcs_[MAX_FUNCS];

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
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 80.645)), module, EFunction::IN_GATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(43.217, 80.645)), module, EFunction::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.275, 118.359)), module, EFunction::IN_ENABLE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 100.367)), module, EFunction::OUT_F1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 100.367)), module, EFunction::OUT_F2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 100.367)), module, EFunction::OUT_F3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 100.367)), module, EFunction::OUT_F4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 118.359)), module, EFunction::OUT_LIGHTS_OUTPUT));
	}
};


Model* modelEFunction = createModel<EFunction, EFunctionWidget>("EFunction");