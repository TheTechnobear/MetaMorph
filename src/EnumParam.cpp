#include "EComponents.h"

using namespace rack;

EnumParam::EnumParam() {
	fb = new widget::FramebufferWidget;
	addChild(fb);
	sw = new widget::SvgWidget;
	fb->addChild(sw);
}

EnumParam::~EnumParam() {
}


void EnumParam::initParamQuantity() {
	ParamWidget::initParamQuantity();
	engine::ParamQuantity* pq = getParamQuantity();
	if (pq) {
		pq->snapEnabled = true;
		pq->smoothEnabled = false;
	}
}


void EnumParam::addFrame(std::shared_ptr<window::Svg> svg) {
	frames.push_back(svg);
	if (!sw->svg) {
		sw->setSvg(svg);
		box.size = sw->box.size;
		fb->box.size = sw->box.size;
	}
}


void EnumParam::onChange(const ChangeEvent& e) {
	engine::ParamQuantity* pq = getParamQuantity();
	if (!frames.empty() && pq) {
		int index = (int) std::round(pq->getValue() - pq->getMinValue());
		index = math::clamp(index, 0, (int) frames.size() - 1);
		sw->setSvg(frames[index]);
		fb->setDirty();
	}
	ParamWidget::onChange(e);
}

void EnumParam::onDragMove(const DragMoveEvent &e) {
	ParamWidget::onDragMove(e);
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	engine::ParamQuantity* pq = getParamQuantity();

	float speed = 5.f;
	int delta = std::round(-e.mouseDelta.y / speed);

    if (pq) {
        int oldValue = pq->getValue();
        int v = oldValue + delta;
		int max = pq->getMaxValue();
		int min = pq->getMinValue();

		v = v > max ? max : v;
		v = v < min ? min : v;

		pq->setValue(v);

        float newValue = pq->getValue();
        if (oldValue != newValue) {
            // Push ParamChange history action
            history::ParamChange* h = new history::ParamChange;
            h->name = "change enum";
            h->moduleId = module->id;
            h->paramId = paramId;
            h->oldValue = oldValue;
            h->newValue = newValue;
            APP->history->push(h);
        }
    }
}


void EnumParam::onDragStart(const DragStartEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	ParamWidget::onDragStart(e);
}

void EnumParam::onDragEnd(const DragEndEvent& e) {
	if (e.button != GLFW_MOUSE_BUTTON_LEFT)
		return;

	ParamWidget::onDragEnd(e);
}
