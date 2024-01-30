#pragma once

#include <rack.hpp>

using namespace rack;


#include "plugin.hpp"

struct EnumParam : ParamWidget {
	EnumParam();
	~EnumParam();

    void addFrame(std::shared_ptr<window::Svg> svg);

	void initParamQuantity() override;
	void onChange(const ChangeEvent& e) override;
    void onDragMove(const DragMoveEvent &e) override;
	void onDragStart(const DragStartEvent& e) override;
	void onDragEnd(const DragEndEvent& e) override;

	widget::FramebufferWidget* fb;
	widget::SvgWidget* sw;
	std::vector<std::shared_ptr<window::Svg>> frames;
};



struct PolyCountParam : EnumParam {
	PolyCountParam() {
        auto plugin = pluginInstance;

		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/00.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/01.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/02.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/03.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/04.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/05.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/06.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/07.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/08.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/09.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/10.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/11.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/12.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/13.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/14.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/15.svg")));
		addFrame(Svg::load(asset::plugin(plugin,"res/components/polycount/16.svg")));
	}
};
