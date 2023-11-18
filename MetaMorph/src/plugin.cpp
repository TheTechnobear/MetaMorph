#include "plugin.hpp"


Plugin* pluginInstance;


void init(Plugin* p) {
	pluginInstance = p;

	p->addModel(modelEDevice);
	p->addModel(modelEScaler);
	p->addModel(modelESwitch);
	p->addModel(modelESplit);
	p->addModel(modelEFunction);
	p->addModel(modelERSplit);
	p->addModel(modelEFunction12);
	p->addModel(modelEIlluminator);
	
	// Any other plugin initialization may go here.
	// As an alternative, consider lazy-loading assets and lookup tables when your module is created to reduce startup times of Rack.
}
