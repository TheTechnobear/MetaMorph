#include "plugin.hpp"

#include "EHarp.h"
#include "Encoding.h"
#include "LightWire.h"


struct ESplit4 : Module {
	enum ParamId {
		P_S1_NROW_PARAM,
		P_S1_POLY_PARAM,
		P_S2_NROW_PARAM,
		P_S2_POLY_PARAM,
		P_S3_NROW_PARAM,
		P_S3_POLY_PARAM,
		P_S4_NROW_PARAM,
		P_S4_POLY_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_ENABLE_INPUT,
		IN_K_INPUT,
		IN_X_INPUT,
		IN_Y_INPUT,
		IN_Z_INPUT,
		IN_KG_INPUT,
		IN1_LIGHTS_INPUT,
		IN2_LIGHTS_INPUT,
		IN3_LIGHTS_INPUT,
		IN4_LIGHTS_INPUT,
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
		OUT3_K_OUTPUT,
		OUT3_X_OUTPUT,
		OUT3_Y_OUTPUT,
		OUT3_Z_OUTPUT,
		OUT3_KG_OUTPUT,
		OUT4_K_OUTPUT,
		OUT4_X_OUTPUT,
		OUT4_Y_OUTPUT,
		OUT4_Z_OUTPUT,
		OUT4_KG_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	ESplit4() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(P_S1_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S1_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S2_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S3_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S3_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S4_NROW_PARAM, 0.f, 1.f, 0.f, "");
		configParam(P_S4_POLY_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN_ENABLE_INPUT, "");
		configInput(IN_K_INPUT, "");
		configInput(IN_X_INPUT, "");
		configInput(IN_Y_INPUT, "");
		configInput(IN_Z_INPUT, "");
		configInput(IN_KG_INPUT, "");
		configInput(IN1_LIGHTS_INPUT, "");
		configInput(IN2_LIGHTS_INPUT, "");
		configInput(IN3_LIGHTS_INPUT, "");
		configInput(IN4_LIGHTS_INPUT, "");
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
		configOutput(OUT3_K_OUTPUT, "");
		configOutput(OUT3_X_OUTPUT, "");
		configOutput(OUT3_Y_OUTPUT, "");
		configOutput(OUT3_Z_OUTPUT, "");
		configOutput(OUT3_KG_OUTPUT, "");
		configOutput(OUT4_K_OUTPUT, "");
		configOutput(OUT4_X_OUTPUT, "");
		configOutput(OUT4_Y_OUTPUT, "");
		configOutput(OUT4_Z_OUTPUT, "");
		configOutput(OUT4_KG_OUTPUT, "");


		paramQuantities[P_S1_POLY_PARAM]->snapEnabled = true;		
		paramQuantities[P_S2_POLY_PARAM]->snapEnabled = true;				
		paramQuantities[P_S3_POLY_PARAM]->snapEnabled = true;		
		paramQuantities[P_S4_POLY_PARAM]->snapEnabled = true;			}

	void processBypass (const ProcessArgs &args) override {
		doProcessBypass(args);
	}

	void process(const ProcessArgs& args) override {
		doProcess(args);
	}

	void doProcessBypass (const ProcessArgs &args)  {
	}

	void doProcess(const ProcessArgs& args) {

		unsigned in_kg_r, in_kg_c;
		float kgMsg = inputs[IN_KG_INPUT].getVoltage();
		decodeKeyGroup(kgMsg,in_kg_r,in_kg_c);

		unsigned nChannels = inputs[IN_K_INPUT].getChannels();

		unsigned startR[MAX_SPLIT],startC[MAX_SPLIT];
		unsigned endR[MAX_SPLIT], endC[MAX_SPLIT];
		unsigned maxVoices[MAX_SPLIT] = {0,0,0,0};


		unsigned splitRow = 0;
		unsigned e_p_nRow =P_S1_NROW_PARAM;
		unsigned e_p_poly = P_S1_POLY_PARAM;
		unsigned e_out_K = OUT1_K_OUTPUT;
		unsigned e_out_X = OUT1_X_OUTPUT;
		unsigned e_out_Y = OUT1_Y_OUTPUT;
		unsigned e_out_Z = OUT1_Z_OUTPUT;
		unsigned e_in_light = IN1_LIGHTS_INPUT;
		for(int splitId=0; splitId< MAX_SPLIT; splitId++) {
			switch (i)
			{
			case 0: 
				e_p_nRow=P_S1_NROW_PARAM;
				e_p_poly = P_S1_POLY_PARAM;
				e_out_K = OUT1_K_OUTPUT;
				e_out_X = OUT1_X_OUTPUT;
				e_out_Y = OUT1_Y_OUTPUT;
				e_out_Z = OUT1_Z_OUTPUT;
				e_in_light = IN1_LIGHTS_INPUT;
				break;
			case 1: 
				e_p_nRow=P_S2_NROW_PARAM;
				e_p_poly = P_S2_POLY_PARAM;
				e_out_K = OUT2_K_OUTPUT;
				e_out_X = OUT2_X_OUTPUT;
				e_out_Y = OUT2_Y_OUTPUT;
				e_out_Z = OUT2_Z_OUTPUT;
				e_in_light = IN2_LIGHTS_INPUT;
				break;
			case 2: 
				e_p_nRow=P_S3_NROW_PARAM;
				e_p_poly = P_S3_POLY_PARAM;
				e_out_K = OUT3_K_OUTPUT;
				e_out_X = OUT3_X_OUTPUT;
				e_out_Y = OUT3_Y_OUTPUT;
				e_out_Z = OUT3_Z_OUTPUT;
				e_in_light = IN3_LIGHTS_INPUT;
				break;
			case 3: 
				e_p_nRow=P_S4_NROW_PARAM;
				e_p_poly = P_S4_POLY_PARAM;
				e_out_K = OUT4_K_OUTPUT;
				e_out_X = OUT4_X_OUTPUT;
				e_out_Y = OUT4_Y_OUTPUT;
				e_out_Z = OUT4_Z_OUTPUT;
				e_in_light = IN4_LIGHTS_INPUT;
				break;
			
			default:
				break;
			}

			unsigned nRow = params[e_p_nRow].getValue();
			maxVoices[splitId] = params[e_p_poly].getValue();

			if(nRow>0) {
				startR[splitId] = splitRow;
				startC[splitId] = 0;
				splitRow += nRow;
				endR[splitId] = splitRow - 1;
				endC[splitId] = in_kg_c;

				for(unsigned ch=0; ch < nChannels; ch++) {
					unsigned in_r=0, in_c=0;
					bool valid= ! (inputs[IN_ENABLE_INPUT].getVoltage()  >= 1.5f);
					if(valid) {
						decodeKey(inputs[IN_K_INPUT].getVoltage(ch), valid, in_r, in_c);
					}

					float inX = inputs[IN_X_INPUT].getVoltage(ch);
					float inY = inputs[IN_Y_INPUT].getVoltage(ch);
					float inZ = inputs[IN_Z_INPUT].getVoltage(ch);
					
					auto& voices = splits_[splitId];
					auto v =voices.findVoice(ch);
					bool inSplit = 
						(in_r >= startR[splitId] && in_r < endR[splitId]) 
						&& 
						(in_c >= startC[splitId] && in_c < endC[splitId])
						;
					if(valid && inSplit) {
						unsigned r = in_r - startR[splitId];
						unsigned c = in_c - startC[splitId];

						if(!v) {
								v = voices.findFreeVoice(maxVoices[splitId]);
						}

						if (v)  {
							v->updateVoice(ch,r,c, valid, inX,inY,inZ);
							outputs[e_out_K].setVoltage(encodeKey(r,c),v->voiceId_);
							outputs[e_out_X].setVoltage(inX,v->voiceId_);
							outputs[e_out_Y].setVoltage(inY,v->voiceId_);
							outputs[e_out_Z].setVoltage(inZ,v->voiceId_);
						}
					} else {
						// was in this split, but now key is outside
						if(v) {
							voices.freeVoice(v);
							outputs[e_out_K].setVoltage(0.f,v->voiceId_);
							outputs[e_out_X].setVoltage(0.f,v->voiceId_);
							outputs[e_out_Y].setVoltage(0.f,v->voiceId_);
							outputs[e_out_Z].setVoltage(0.f,v->voiceId_);
						}
					}
				} // for channel
				outputs[e_out_K].setChannels(maxVoices[split]);
				outputs[e_out_X].setChannels(maxVoices[split]);
				outputs[e_out_Y].setChannels(maxVoices[split]);
				outputs[e_out_Z].setChannels(maxVoices[split]);
			} else {
				// ignore, we are not using this split
			}


		}




		float ledmsg[MAX_SPLIT]= {0.f,0.f,0.f,0.f};
		unsigned split = 0;
		ledmsg[split] = inputs[IN1_LIGHTS_INPUT].getVoltage();
		if(kgMsg > 0.f) {
			outputs[OUT1_KG_OUTPUT].setVoltage(encodeKeyGroup(endR[split] - startR[split], endC[split] - startC[split]));
		} else {
			outputs[OUT1_KG_OUTPUT].setVoltage(0);
		}

		split++;
		ledmsg[split] = inputs[IN2_LIGHTS_INPUT].getVoltage();
		if(kgMsg > 0.f) {
			outputs[OUT2_KG_OUTPUT].setVoltage(encodeKeyGroup(endR[split] - startR[split], endC[split] - startC[split]));
		} else {
			outputs[OUT2_KG_OUTPUT].setVoltage(0.f);
		}

		split++;
		ledmsg[split] = inputs[IN3_LIGHTS_INPUT].getVoltage();
		if(kgMsg > 0.f) {
			outputs[OUT3_KG_OUTPUT].setVoltage(encodeKeyGroup(endR[split] - startR[split], endC[split] - startC[split]));
		} else {
			outputs[OUT3_KG_OUTPUT].setVoltage(0.f);
		}

		split++;
		ledmsg[split] = inputs[IN4_LIGHTS_INPUT].getVoltage();
		if(kgMsg > 0.f) {
			outputs[OUT4_KG_OUTPUT].setVoltage(encodeKeyGroup(endR[split] - startR[split], endC[split] - startC[split]));
		} else {
			outputs[OUT4_KG_OUTPUT].setVoltage(0.f);
		}


		for(unsigned s = 0;s<MAX_SPLIT;s++) {
			if(ledmsg[s] != 0.0f) {
				unsigned startr,startc, sizer,sizec;
				LedMsgType t;
				decodeLedMsg(ledmsg[s],t,startr,startc,sizer,sizec);
				float msg = encodeLedMsg(
						t,
						startr + startR[s],
						startc + startC[s],
						sizer,
						sizec
					);

				ledQueue_.write(msg);
			}
		}

		// dont really need this check as empty queue leaves msg untouched.
		float msg=0.0f;
		if(ledQueue_.read(msg)) {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(msg);
		} else {
			outputs[OUT_LIGHTS_OUTPUT].setVoltage(0.f);
		}

	}

	static constexpr unsigned MAX_SPLIT = 4;

	MsgQueue<float> ledQueue_;

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

    Voices<SplitVoice> splits_[MAX_SPLIT];

};


struct ESplit4Widget : ModuleWidget {
	ESplit4Widget(ESplit4* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ESplit4.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 58.215)), module, ESplit4::P_S1_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 58.215)), module, ESplit4::P_S1_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 77.265)), module, ESplit4::P_S2_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 77.794)), module, ESplit4::P_S2_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 96.844)), module, ESplit4::P_S3_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 97.373)), module, ESplit4::P_S3_POLY_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(8.929, 115.894)), module, ESplit4::P_S4_NROW_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(23.745, 116.423)), module, ESplit4::P_S4_POLY_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.763, 32.772)), module, ESplit4::IN_ENABLE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(37.355, 32.772)), module, ESplit4::IN_K_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(49.433, 32.772)), module, ESplit4::IN_X_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(61.51, 32.772)), module, ESplit4::IN_Y_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(73.588, 32.772)), module, ESplit4::IN_Z_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(85.195, 32.772)), module, ESplit4::IN_KG_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 58.215)), module, ESplit4::IN1_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 77.794)), module, ESplit4::IN2_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 97.373)), module, ESplit4::IN3_LIGHTS_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(98.869, 116.423)), module, ESplit4::IN4_LIGHTS_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(98.285, 32.772)), module, ESplit4::OUT_LIGHTS_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 58.215)), module, ESplit4::OUT1_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 58.215)), module, ESplit4::OUT1_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 58.215)), module, ESplit4::OUT1_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 58.215)), module, ESplit4::OUT1_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 58.215)), module, ESplit4::OUT1_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 77.794)), module, ESplit4::OUT2_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 77.794)), module, ESplit4::OUT2_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 77.794)), module, ESplit4::OUT2_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 77.794)), module, ESplit4::OUT2_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 77.794)), module, ESplit4::OUT2_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 97.373)), module, ESplit4::OUT3_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 97.373)), module, ESplit4::OUT3_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 97.373)), module, ESplit4::OUT3_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 97.373)), module, ESplit4::OUT3_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.456, 97.373)), module, ESplit4::OUT3_KG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.496, 116.423)), module, ESplit4::OUT4_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(50.574, 116.423)), module, ESplit4::OUT4_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(62.652, 116.423)), module, ESplit4::OUT4_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.73, 116.423)), module, ESplit4::OUT4_Z_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(85.742, 116.423)), module, ESplit4::OUT4_KG_OUTPUT));
	}
};


Model* modelESplit4 = createModel<ESplit4, ESplit4Widget>("ESplit4");