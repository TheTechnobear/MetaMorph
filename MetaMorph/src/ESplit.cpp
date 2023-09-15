#include "plugin.hpp"

#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"

#include <iostream>

struct ESplit : Module {
	enum ParamId {
		P_S1_START_X_PARAM,
		P_S1_END_X_PARAM,
		P_S1_START_Y_PARAM,
		P_S1_END_Y_PARAM,
		P_S1_POLY_PARAM,
		P_S2_POLY_PARAM,
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
		IN1_LIGHTS_INPUT,
		IN2_LIGHTS_INPUT,
		IN_ENABLE_INPUT,
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

	ESplit() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_S1_START_X_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_S1_END_X_PARAM, 1.f, 24.f, 4.f, "");
		configParam(P_S1_START_Y_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_S1_END_Y_PARAM, 1.f, 24.f, 4.f, "");
		configParam(P_S1_POLY_PARAM, 1.f, 16.f, 8.f, "");
		configParam(P_S2_POLY_PARAM, 1.f, 16.f, 8.f, "");
		configParam(P_S2_START_X_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_S2_END_X_PARAM, 1.f, 24.f, 4.f, "");
		configParam(P_S2_START_Y_PARAM, 0.f, 24.f, 0.f, "");
		configParam(P_S2_END_Y_PARAM, 1.f, 24.f, 4.f, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(IN_Z_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configInput(IN1_LIGHTS_INPUT, "");
		configInput(IN2_LIGHTS_INPUT, "");
		configInput(IN_ENABLE_INPUT, "");
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

		paramQuantities[P_S1_START_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_S1_END_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_S1_START_Y_PARAM]->snapEnabled = true;		
		paramQuantities[P_S1_END_Y_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_START_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_END_X_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_START_Y_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_END_Y_PARAM]->snapEnabled = true;		

		paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;		
	}

	void process(const ProcessArgs& args) override {
		doProcess(args);
	}

	void doProcess(const ProcessArgs& args) {
		unsigned startX[2],startY[2];
		unsigned endX[2], endY[2];
		startX[0] = params[P_S1_START_X_PARAM].getValue();
		startY[0] = params[P_S1_START_Y_PARAM].getValue();
		startX[1] = params[P_S2_START_X_PARAM].getValue();
		startY[1] = params[P_S2_START_Y_PARAM].getValue();
		endX[0] = params[P_S1_END_X_PARAM].getValue();
		endX[1] = params[P_S2_END_X_PARAM].getValue();
		endY[0] = params[P_S1_END_Y_PARAM].getValue();
		endY[1] = params[P_S2_END_Y_PARAM].getValue();

		unsigned maxVoices[2] = {0,0};
		maxVoices[0] = params[P_S1_POLY_PARAM].getValue();
		maxVoices[1] = params[P_S2_POLY_PARAM].getValue();


		// X = col (horizontal)
		// Y = row (vertical)

		unsigned in_kg_r, in_kg_c;
		decodeKeyGroup(inputs[IN_KG_INPUT].getVoltage(),in_kg_r,in_kg_c);

		// todo, we will need to revoice this, once we have poly param
		unsigned nChannels = inputs[IN_K_INPUT].getChannels();


		for(unsigned ch=0; ch < nChannels; ch++) {
			unsigned in_r=0, in_c=0;
			bool valid= ! (inputs[IN_ENABLE_INPUT].getVoltage()  > 2.0f);
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
				bool inSplit = 
					(in_r >= startY[splitId] && in_r < endY[splitId]) 
					&& 
					(in_c >= startX[splitId] && in_c < endX[splitId])
					;
				if(valid && inSplit) {
					unsigned r = in_r - startY[splitId];
					unsigned c = in_c - startX[splitId];

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
				bool inSplit = 
					(in_r >= startY[splitId] && in_r < endY[splitId]) 
					&& 
					(in_c >= startX[splitId] && in_c < endX[splitId])
					;
				if(valid && inSplit) {
					unsigned r = in_r - startY[splitId];
					unsigned c = in_c - startX[splitId];

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

		float ledmsg[2]= {0.f,0.f};
		ledmsg[0] = inputs[IN1_LIGHTS_INPUT].getVoltage();
		ledmsg[1] = inputs[IN2_LIGHTS_INPUT].getVoltage();
		for(unsigned s = 0;s<2;s++) {
			if(ledmsg[s] != 0.0f) {
				unsigned startr,startc, sizer,sizec;
				LedMsgType t;
				decodeLedMsg(ledmsg[s],t,startr,startc,sizer,sizec);
				float msg = encodeLedMsg(
						t,
						startr + startY[s],
						startc + startX[s],
						sizer,
						sizec
					);

				ledQueue_.write(msg);
			}
		}

		outputs[OUT1_K_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_X_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_Y_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_Z_OUTPUT].setChannels(maxVoices[0]);
		outputs[OUT1_KG_OUTPUT].setVoltage(encodeKeyGroup(endY[0] - startY[0], endX[0] - startX[0]));


		outputs[OUT2_K_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_X_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_Y_OUTPUT].setChannels(maxVoices[1]);
		outputs[OUT2_Z_OUTPUT].setChannels(maxVoices[1]);

		outputs[OUT2_KG_OUTPUT].setVoltage(encodeKeyGroup(endY[1] - startY[1], endX[1] - startX[1]));


		float msg=0.0f;
		// dont really need this check as empty queue leaves msg untouched.
		if(ledQueue_.read(msg)) {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
		} else {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.0f);
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
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(81.198, 29.53)), module, ESplit::P_S1_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(81.198, 46.463)), module, ESplit::P_S2_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(6.705, 47.622)), module, ESplit::P_S2_START_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(31.241, 47.48)), module, ESplit::P_S2_END_X_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.448, 47.622)), module, ESplit::P_S2_START_Y_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(43.414, 47.48)), module, ESplit::P_S2_END_Y_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.705, 63.712)), module, ESplit::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(18.782, 63.712)), module, ESplit::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.86, 63.712)), module, ESplit::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(42.938, 63.712)), module, ESplit::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(60.366, 64.424)), module, ESplit::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.877, 82.209)), module, ESplit::IN1_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(80.877, 100.2)), module, ESplit::IN2_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(7.275, 118.359)), module, ESplit::IN_ENABLE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(80.864, 64.603)), module, ESplit::OUT_LIGHTS_OUTPUT));
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
	}
};


Model* modelESplit = createModel<ESplit, ESplitWidget>("ESplit");