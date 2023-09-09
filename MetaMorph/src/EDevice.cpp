#include "plugin.hpp"

#include "../EigenLite/eigenapi/eigenapi.h"

#include <iostream>



struct EDevice : Module, EigenApi::Callback {
	static constexpr unsigned MAX_VOICE = 16;
	struct VoiceData {
		bool active_=false;
		unsigned key_=0;
		unsigned p_=0;
		int r_=0;
		int y_=0;

	} voices_[MAX_VOICE];

	enum ParamId {
		PARAMS_LEN
	};
	enum InputId {
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
		LED1_LIGHT,
		LED2_LIGHT,
		LED3_LIGHT,
		LED4_LIGHT,
		LED5_LIGHT,
		LED6_LIGHT,
		LED7_LIGHT,
		LED8_LIGHT,
		LED9_LIGHT,
		LED10_LIGHT,
		LED11_LIGHT,
		LED12_LIGHT,
		LED13_LIGHT,
		LED14_LIGHT,
		LED15_LIGHT,
		LED16_LIGHT,
		LIGHTS_LEN
	};


	EDevice() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configOutput(OUT_K_OUTPUT, "Key");
		configOutput(OUT_X_OUTPUT, "X");
		configOutput(OUT_Y_OUTPUT, "Y");
		configOutput(OUT_Z_OUTPUT, "Z");

		harp_ = std::make_shared<EigenApi::Eigenharp>(firmware_);
	    harp_->setPollTime(0);
		harp_->start();
		harp_->addCallback(this);
		
	}
	~EDevice() {
		if(harp_) {
			harp_->removeCallback(this);
			harp_->stop();
		}
	}

	void process(const ProcessArgs& args) override {

		int rate = args.sampleRate / 1000; // really should be 2k, lets do a bit more
		if((args.frame % rate) == 0) {
			harp_->process(); // will hit callbacks
		}

		for(unsigned voice=0;voice<MAX_VOICE;voice++) {
			auto& vdata = voices_[voice];
			float kV = vdata.active_ ? keyToV(vdata.key_) : 0.0f;
			float pV = vdata.active_ ? pToV(vdata.p_) : 0.0f;
			float rV = vdata.active_ ? rToV(vdata.r_) : 0.0f;
			float yV = vdata.active_ ? yToV(vdata.y_) : 0.0f;
	
			lights[voice].setBrightness(pV / 10.0f);

			outputs[OUT_K_OUTPUT].setVoltage(kV, voice);
			outputs[OUT_X_OUTPUT].setVoltage(rV, voice);
			outputs[OUT_Y_OUTPUT].setVoltage(yV, voice);
			outputs[OUT_Z_OUTPUT].setVoltage(pV, voice);
		}

		outputs[OUT_K_OUTPUT].setChannels(MAX_VOICE);
		outputs[OUT_X_OUTPUT].setChannels(MAX_VOICE);
		outputs[OUT_Y_OUTPUT].setChannels(MAX_VOICE);
		outputs[OUT_Z_OUTPUT].setChannels(MAX_VOICE);
	}


	void processBypass (const ProcessArgs &args) override {
		int rate = args.sampleRate / 1000; // really should be 2k, lets do a bit more
		if((args.frame % rate) == 0) {
			harp_->process(); // will hit callbacks
		}
	}

	std::shared_ptr<EigenApi::Eigenharp> harp_;
	EigenApi::FWR_Embedded firmware_;


	float keyToV(unsigned key) { // -5 to +5v, v/oct
		return  float(key) / 12.0f;
	}

	float pToV(unsigned p) { // 0..10v
		return float(p * 10)/ 4096.0f;
	}
	float rToV(int r) { // -5 to +5v
		return float(r * 5) / 4096.0f;
	}
	float yToV(int y) { // -5 to +5v
		return float(y * 5) / 4096.0f;
	}

    void device(const char* dev, DeviceType dt, int rows, int cols, int ribbons, int pedals) override {
    }

    void disconnect(const char* dev, DeviceType dt) override
    {
    }

    void key(const char* dev, unsigned long long t, unsigned course, unsigned key, bool a, unsigned p, int r, int y) override  {
		if(course > 0) {
			// function buttons
			return;
		}

		auto v = findVoice(key);
		if(v) {
			// active voice
			if(a) {
				// was active, still is, update below
			} else {
				// free the voice !
				freeVoice(v);
				harp_->setLED(dev,course,key,0);
				// no more action neccesary
				v = nullptr;
			}
		} else {
			// no active voice
			if(a) {
				// allocate new voice, we will update details below
				v = findFreeVoice();
				harp_->setLED(dev,course,key,3);

				// note: v will be null if we have no more voices available
			} else {
				// ok, not active... and no voice anyway - ignore
			}
		}
		if(v) {
			v->active_ = a;
			v->key_ = key;
			v->p_ = p;
			v->r_ = r;
			v->y_ = y;
		}
    }

    void breath(const char* dev, unsigned long long t, unsigned val) override {
    }
    
    void strip(const char* dev, unsigned long long t, unsigned strip, unsigned val, bool a) override  {
    }
    
    void pedal(const char* dev, unsigned long long t, unsigned pedal, unsigned val) override  {
    }
    
	VoiceData* findFreeVoice() {
		for(auto& v : voices_) {
			if(!v.active_) {
				return &v;
			}
		}
		return nullptr;
	}

	VoiceData* findVoice(unsigned key) {
		for(auto& v : voices_) {
			if(v.active_ && v.key_ == key) {
				return &v;
			}
		}
		return nullptr;
	}

	void freeVoice(VoiceData* vp) {
		vp->active_ = false;
		vp->key_ = 0;
		vp->p_ = 0;
		vp->r_ = 0;
		vp->y_ = 0;
	}
};


struct EDeviceWidget : ModuleWidget {
	EDeviceWidget(EDevice* module) {
		setModule(module);

		setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(7.275, 117.83)), module, EDevice::OUT_K_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.353, 117.83)), module, EDevice::OUT_X_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(31.431, 117.83)), module, EDevice::OUT_Y_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.509, 117.83)), module, EDevice::OUT_Z_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.846, 34.793)), module, EDevice::LED1_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.846, 34.793)), module, EDevice::LED2_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.846, 34.793)), module, EDevice::LED3_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.846, 34.793)), module, EDevice::LED4_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.846, 46.793)), module, EDevice::LED5_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.846, 46.793)), module, EDevice::LED6_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.846, 46.793)), module, EDevice::LED7_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.846, 46.793)), module, EDevice::LED8_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.846, 58.793)), module, EDevice::LED9_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.846, 58.793)), module, EDevice::LED10_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.846, 58.793)), module, EDevice::LED11_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.846, 58.793)), module, EDevice::LED12_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(7.846, 70.793)), module, EDevice::LED13_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(19.846, 70.793)), module, EDevice::LED14_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(31.846, 70.793)), module, EDevice::LED15_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(43.846, 70.793)), module, EDevice::LED16_LIGHT));
	}
};


Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EDevice");