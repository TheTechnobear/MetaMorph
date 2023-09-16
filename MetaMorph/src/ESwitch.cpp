#include "plugin.hpp"

#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"


struct ESwitch : Module {
	enum ParamId {
		P_S1_POLY_PARAM,
		P_S2_POLY_PARAM,
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
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ESwitch() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_S1_POLY_PARAM, 1.f, 16.f, 8.f, "");
		configParam(P_S2_POLY_PARAM, 1.f, 16.f, 8.f, "");
		configInput(IN_SELECT_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(IN_Z_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configInput(IN1_LIGHTS_INPUT, "");
		configInput(IN2_LIGHTS_INPUT, "");
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

		paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;		
	}

	void processBypass (const ProcessArgs &args) override {
		doProcessBypass(args);
	}

	void process(const ProcessArgs& args) override {
		doProcess(args);
	}

	void doProcessBypass (const ProcessArgs &args)  {
	}


	void doProcess(const ProcessArgs& args) {
		unsigned maxVoices[2] = {0,0};
		maxVoices[0] = params[P_S1_POLY_PARAM].getValue();
		maxVoices[1] = params[P_S2_POLY_PARAM].getValue();


		unsigned in_kg_r, in_kg_c;
		float kgMsg = inputs[IN_KG_INPUT].getVoltage();
		decodeKeyGroup(kgMsg,in_kg_r,in_kg_c);

		// todo, we will need to revoice this, once we have poly param
		unsigned nChannels = inputs[IN_K_INPUT].getChannels();


		float selector = inputs[IN_SELECT_INPUT].getVoltage();
		unsigned activeSplit = selector >= 1.5f;
		
		for(unsigned ch=0; ch < nChannels; ch++) {
			unsigned in_r=0, in_c=0;
			bool valid= ! (inputs[IN_ENABLE_INPUT].getVoltage()  >= 1.5f);
			if(valid) {
				decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c);
			}

			float inX = inputs[IN_X_INPUT].getVoltage(ch);
			float inY = inputs[IN_Y_INPUT].getVoltage(ch);
			float inZ = inputs[IN_Z_INPUT].getVoltage(ch);
			
			unsigned splitId;
			{
				splitId=0;
				auto& voices = splits_[splitId];
				auto v =voices.findVoice(ch);
				bool inSplit = splitId == activeSplit;

				if(valid && inSplit) {
					unsigned r = in_r;
					unsigned c = in_c;

					if(!v) {
							v = voices.findFreeVoice(maxVoices[splitId]);
					}

					if (v)  {
						v->updateVoice(ch,r,c, valid, inX,inY,inZ);
						outputs[OUT1_K_OUTPUT].setVoltage(encodeKey(r,c),v->voiceId_);
						outputs[OUT1_X_OUTPUT].setVoltage(inX,v->voiceId_);
						outputs[OUT1_Y_OUTPUT].setVoltage(inY,v->voiceId_);
						outputs[OUT1_Z_OUTPUT].setVoltage(inZ,v->voiceId_);
					}
				} else {
					// was in this split, but now key is outside
					if(v) {
						voices.freeVoice(v);
						outputs[OUT1_K_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT1_X_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT1_Y_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT1_Z_OUTPUT].setVoltage(0.f,v->voiceId_);
					}
				}
			}

			{
				splitId=1;
				auto& voices = splits_[splitId];
				auto v =voices.findVoice(ch);
				bool inSplit = splitId == activeSplit;
				if(valid && inSplit) {
					unsigned r = in_r;
					unsigned c = in_c;

					if(!v) {
							v = voices.findFreeVoice(maxVoices[splitId]);
					}

					if (v)  {
						v->updateVoice(ch,r,c, valid, inX,inY,inZ);
						outputs[OUT2_K_OUTPUT].setVoltage(encodeKey(r,c),v->voiceId_);
						outputs[OUT2_X_OUTPUT].setVoltage(inX,v->voiceId_);
						outputs[OUT2_Y_OUTPUT].setVoltage(inY,v->voiceId_);
						outputs[OUT2_Z_OUTPUT].setVoltage(inZ,v->voiceId_);
					}
				} else {
					// was in this split, but now key is outside
					if(v) {
						voices.freeVoice(v);
						outputs[OUT2_K_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT2_X_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT2_Y_OUTPUT].setVoltage(0.f,v->voiceId_);
						outputs[OUT2_Z_OUTPUT].setVoltage(0.f,v->voiceId_);
					}
				}
			}
		} // for channel

		outputs[OUT1_K_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_X_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_Y_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_Z_OUTPUT].setChannels(maxVoices[0]);

		outputs[OUT2_K_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_X_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_Y_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_Z_OUTPUT].setChannels(maxVoices[1]);

		if(activeSplit==0) {
				outputs[OUT1_KG_OUTPUT].setVoltage(kgMsg);
				outputs[OUT2_KG_OUTPUT].setVoltage(0.f);
				outputs[OUT_LIGHTS_OUTPUT].setVoltage( inputs[IN1_LIGHTS_INPUT].getVoltage());
		} else {
				outputs[OUT1_KG_OUTPUT].setVoltage(0.f);
				outputs[OUT2_KG_OUTPUT].setVoltage(kgMsg);
				outputs[OUT_LIGHTS_OUTPUT].setVoltage( inputs[IN2_LIGHTS_INPUT].getVoltage());
		}
	}


	static constexpr int MAX_MSGS=10;
	MsgQueue<float> ledQueue_=MAX_MSGS;

	struct SplitVoice : public Voice  {
		unsigned r_=0;
		unsigned c_=0;
		float x_=0.f;
		float y_=0.f;
		float z_=0.f;

		void updateVoice(long key, unsigned r, unsigned c, bool a, float x, float y, float z) {
			active_ = a;
			key_ = key; // inbound channel
			r_=r;
			c_=c;
			x_=x;
			y_=y;
			z_=z;
		}

		void freeVoice() override {
			updateVoice(0xff,false,0,0,0.f,0.f,0.f);
		}
	};

    Voices<SplitVoice> splits_[2];
};


struct ESwitchWidget : ModuleWidget {
	ESwitchWidget(ESwitch* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ESwitch.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.458, 91.023)), module, ESwitch::P_S1_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(9.458, 112.719)), module, ESwitch::P_S2_POLY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(46.078, 29.499)), module, ESwitch::IN_SELECT_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.046, 61.951)), module, ESwitch::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(23.638, 61.951)), module, ESwitch::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(35.716, 61.951)), module, ESwitch::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(47.794, 61.951)), module, ESwitch::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(59.871, 61.951)), module, ESwitch::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(71.478, 61.951)), module, ESwitch::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.581, 91.023)), module, ESwitch::IN1_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(84.581, 112.719)), module, ESwitch::IN2_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(84.568, 61.951)), module, ESwitch::OUT_LIGHTS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.209, 91.023)), module, ESwitch::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.286, 91.023)), module, ESwitch::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.364, 91.023)), module, ESwitch::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.442, 91.023)), module, ESwitch::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(71.169, 91.023)), module, ESwitch::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(24.209, 112.719)), module, ESwitch::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(36.286, 112.719)), module, ESwitch::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.364, 112.719)), module, ESwitch::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.442, 112.719)), module, ESwitch::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(71.454, 112.719)), module, ESwitch::OUT2_KG_OUTPUT));
	}
};


Model* modelESwitch = createModel<ESwitch, ESwitchWidget>("ESwitch");