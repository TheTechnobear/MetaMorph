#include "plugin.hpp"

#include "../EigenLite/eigenapi/eigenapi.h"

#include <iostream>


class PrinterCallback: public  EigenApi::Callback
{
public:
    PrinterCallback(EigenApi::Eigenharp& eh) : eh_(eh), led_(false)
    {
    }
    
    virtual void device(const char* dev, DeviceType dt, int rows, int cols, int ribbons, int pedals)
    {
        std::cout << "device " << dev << " (" << dt << ") " << rows << " x " << cols << " strips " << ribbons << " pedals " << pedals << std::endl;
        switch(dt) {
            case PICO : maxLeds_ = 16; break;
            case TAU : maxLeds_ = 84; break;
            case ALPHA: maxLeds_ = 120; break;
            default: maxLeds_ = 0;
        }
    }

    virtual void disconnect(const char* dev, DeviceType dt)
    {
        std::cout << "disconnect " << dev << " (" << dt << ") " << std::endl;
    }

    virtual void key(const char* dev, unsigned long long t, unsigned course, unsigned key, bool a, unsigned p, int r, int y)
    {
        std::cout  << "key " << dev << " @ " << t << " - " << course << ":" << key << ' ' << a << ' ' << p << ' ' << r << ' ' << y << std::endl;
        if(course) {
            // mode key
            eh_.setLED(dev, course, key, a);
        } else {
            if(!a) {
                led_ = ! led_;
                for(int i=0;i<maxLeds_;i++) {
                    if(led_)
                        eh_.setLED(dev, 0, i,i % 3);
                    else
                        eh_.setLED(dev, 0, i, 0);
                }
            }
        }
    }
    virtual void breath(const char* dev, unsigned long long t, unsigned val)
    {
        std::cout  << "breath " << dev << " @ " << t << " - "  << val << std::endl;
    }
    
    virtual void strip(const char* dev, unsigned long long t, unsigned strip, unsigned val, bool a)
    {
        std::cout  << "strip " << dev << " @ " << t << " - " << strip << " = " << val << " " << a << std::endl;
    }
    
    virtual void pedal(const char* dev, unsigned long long t, unsigned pedal, unsigned val)
    {
        std::cout  << "pedal " << dev << " @ " << t << " - " << pedal << " = " << val << std::endl;
    }
    
    EigenApi::Eigenharp& eh_;
    bool led_=false;
    int  maxLeds_=0;  
};

class NullCallback: public  EigenApi::Callback
{
public:
    NullCallback(EigenApi::Eigenharp& eh) : eh_(eh), led_(false)
    {
    }
    
    void device(const char* dev, DeviceType dt, int rows, int cols, int ribbons, int pedals) override
    {
        std::cout << "device " << dev << " (" << dt << ") " << rows << " x " << cols << " strips " << ribbons << " pedals " << pedals << std::endl;
        switch(dt) {
            case PICO : maxLeds_ = 16; break;
            case TAU : maxLeds_ = 84; break;
            case ALPHA: maxLeds_ = 120; break;
            default: maxLeds_ = 0;
        }
    }

    void disconnect(const char* dev, DeviceType dt) override
    {
    }

    void key(const char* dev, unsigned long long t, unsigned course, unsigned key, bool a, unsigned p, int r, int y) override
    {
    }

    void breath(const char* dev, unsigned long long t, unsigned val) override
    {
    }
    
    void strip(const char* dev, unsigned long long t, unsigned strip, unsigned val, bool a) override
    {
    }
    
    void pedal(const char* dev, unsigned long long t, unsigned pedal, unsigned val) override
    {
    }
    
    EigenApi::Eigenharp& eh_;
    bool led_=false;
    int  maxLeds_=0;
};


struct EDevice : Module {
	enum ParamId {
		PARAM1_PARAM,
		PARAM2_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN1_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT2_OUTPUT,
		OUT1_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	EDevice() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PARAM1_PARAM, 0.f, 1.f, 0.f, "");
		configParam(PARAM2_PARAM, 0.f, 1.f, 0.f, "");
		configInput(IN1_INPUT, "");
		configOutput(OUT2_OUTPUT, "");
		configOutput(OUT1_OUTPUT, "");
		configOutput(OUT3_OUTPUT, "");
		configOutput(OUT4_OUTPUT, "");

		harp_ = std::make_shared<EigenApi::Eigenharp>(firmware_);
		harp_->start();
		harp_->addCallback(new NullCallback(*harp_));
		
	}
	~EDevice() {
		if(harp_) {
			harp_->stop();
		}
	}

	void process(const ProcessArgs& args) override {
	    harp_->setPollTime(0);
		harp_->process();
	}

	std::shared_ptr<EigenApi::Eigenharp> harp_;
	EigenApi::FWR_Embedded firmware_;


};


struct EDeviceWidget : ModuleWidget {
	EDeviceWidget(EDevice* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/EDevice.svg")));

		

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.556, 28.958)), module, EDevice::PARAM1_PARAM));
		addParam(createParamCentered<RoundHugeBlackKnob>(mm2px(Vec(27.96, 54.065)), module, EDevice::PARAM2_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.702, 105.562)), module, EDevice::IN1_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.684, 118.686)), module, EDevice::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.272, 118.971)), module, EDevice::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.097, 118.971)), module, EDevice::OUT3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(43.223, 119.256)), module, EDevice::OUT4_OUTPUT));
	}
};


Model* modelEDevice = createModel<EDevice, EDeviceWidget>("EDevice");