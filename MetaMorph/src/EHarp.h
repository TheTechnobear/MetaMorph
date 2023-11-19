#pragma once

#include <string>

#include "../EigenLite/eigenapi/eigenapi.h"
#include "Encoding.h"

// todo move to namespace and classes
class keyValue {
   public:
    keyValue(float x) : tar_(x) {}
    inline void set(unsigned x) {
        tar_ = float(x) / 12.0f;
    }
    inline float next(float) {
        return tar_;
    }

   private:
    float tar_ = 0.f;
};

class bipolarValue {
   public:
    bipolarValue(float x) {
        set(x);
        cur_ = tar_;
    }

    inline void set(float x) {
        tar_ = x * 10.f;
    }
    inline float next(float stepPct_) {
        cur_ += (tar_ - cur_) * stepPct_;
        if(cur_ > -0.0001f && cur_ < 0.0001f) cur_ = 0.0f;
        return cur_;
    }

   private:
    float cur_ = 0.f;
    float tar_ = 0.f;
};

class unipolarValue {
   public:
    unipolarValue(float x) {
        set(x);
        cur_ = tar_;
    }
    inline void set(float x) {
        tar_ = x * 10.f;
    }
    inline float next(float stepPct_) {
        cur_ += (tar_ - cur_) * stepPct_;
        if(cur_ < 0.0001f) cur_ = 0.0f;
        return cur_;
    }

   private:
    float cur_ = 0.f;
    float tar_ = 0.f;
};

class Voice {
   public:
    Voice() = default;
    virtual ~Voice() = default;
    unsigned voiceId_ = 0;

    bool active_ = false;
    unsigned key_ = 0;

    virtual void freeVoice() = 0;
};

template <class T>
class Voices {
   public:
    static constexpr unsigned MAX_VOICE = 16;
    Voices() {
        unsigned vId = 0;
        for (auto& v : voices_) {
            v.voiceId_ = vId;
            vId++;
        }
    }

    T* findFreeVoice(unsigned maxVoiceId) {
        unsigned vId = 0;
        for (auto& v : voices_) {
            if (!v.active_) {
                return &v;
            }
            vId++;
            if (vId >= maxVoiceId) {
                return nullptr;
            }
        }
        return nullptr;
    }

    T* findVoice(unsigned key) {
        for (auto& v : voices_) {
            if (v.key_ == key && v.active_) {
                return &v;
            }
        }
        return nullptr;
    }

    void freeVoice(T* p) {
        p->freeVoice();
    }

    T voices_[MAX_VOICE];
};

struct FullVoice : public Voice {
    // target voltages
    keyValue keyV_ = 0.0f;
    unipolarValue pV_ = 0.0f;
    bipolarValue rV_ = 0.0f;
    bipolarValue yV_ = 0.0f;

    void updateVoice(unsigned key, bool a, float p, float r, float y) {
        active_ = a;
        key_ = key;
        keyV_.set(key_);
        pV_.set(active_ ? p : 0);
        rV_.set(active_ ? r : 0);
        yV_.set(active_ ? y : 0);
    }

    void freeVoice() override {
        updateVoice(key_, false, 0, 0, 0);
    }
};

struct FunctionVoice : public Voice {
    keyValue keyV_ = 0.0f;
    unipolarValue actV_ = 0.0f;

    void updateVoice(unsigned key, bool a) {
        key_ = key;
        keyV_.set(key_);
        active_ = a;
        actV_.set(a ? 1.0f : 0);
    }
    void freeVoice() override {
        updateVoice(0, false);
    }
};

struct EHarp {
    enum EType {
        PICO,
        TAU,
        ALPHA
    } type_;

    static constexpr unsigned MAX_VOICE = 16;
    static constexpr unsigned MAX_STRIP = 2;
    static constexpr unsigned MAX_PEDAL = 2;

    bipolarValue breathV_ = 0.0f;
    unipolarValue stripV_[2] = {0.0f, 0.0f};
    unipolarValue pedalV_[2] = {0.0f, 0.0f};
    Voices<FullVoice> mainVoices_;
    Voices<FullVoice> percVoices_;
    Voices<FunctionVoice> funcVoices_;

    struct KeyGroup {
        enum {
            KG_MAIN,
            KG_PERC,
            KG_FUNC
        };
        unsigned r_ = 0, c_ = 0;
    } keygroups_[3];

    bool translateRCtoK(unsigned kg, unsigned r, unsigned c, unsigned& course, unsigned& k) {
        unsigned kg_r = keygroups_[kg].r_;
        unsigned kg_c = keygroups_[kg].c_;
        if (type_ == TAU) {
            switch (kg) {
                case KeyGroup::KG_MAIN: {
                    // special handling for tau main group
                    // where we have 'faked' some extra keys
                    course = 0;
                    if (c < 2) {
                        if (r < 16) {
                            k = c * 16 + r;
                            return true;
                        }
                        // else out of range
                    } else {
                        if (c < kg_c && r < kg_r) {
                            k = 32 + ((c - 2)) * 20 + r;
                            return true;
                        }
                        // else out of range
                    }
                    break;
                }  // main
                case KeyGroup::KG_PERC: {
                    course = 0;
                    if (r < kg_r && c < kg_c) {
                        k = c * kg_r + r + 72;
                        return true;
                    }
                    break;
                }
                case KeyGroup::KG_FUNC: {
                    course = 1;
                    if (r < kg_r && c < kg_c) {
                        k = c * kg_r + r;
                        return true;
                    }
                    break;
                }
            }
        } else {
            if (r < kg_r && c < kg_c) {
                k = c * kg_r + r;
                course = (kg == KeyGroup::KG_FUNC);
                return true;
            }
            // else out of range
        }
        return false;
    }

    std::string lastDevice_ = "";
};

class EHarpCallback : public EigenApi::Callback {
   public:
    EHarpCallback(EHarp& harp) : harpData_(harp) {
    }

    void connected(const char* dev, DeviceType dt) override {
        if (harpData_.lastDevice_ != "" && harpData_.lastDevice_ != dev) {
            return;
        }

        switch (dt) {
            case PICO: {
                harpData_.type_ = EHarp::PICO;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].r_ = 9;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].c_ = 2;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].r_ = 0;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].c_ = 0;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].r_ = 4;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].c_ = 1;

                break;
            }
            case TAU: {
                harpData_.type_ = EHarp::TAU;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].r_ = 20;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].c_ = 4;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].r_ = 12;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].c_ = 1;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].r_ = 8;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].c_ = 1;
                break;
            }
            case ALPHA: {
                harpData_.type_ = EHarp::ALPHA;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].r_ = 24;
                harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].c_ = 5;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].r_ = 12;
                harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].c_ = 1;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].r_ = 0;
                harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].c_ = 0;
                break;
            }
        }

        harpData_.lastDevice_ = dev;
    }

    void disconnected(const char* dev) override {
        if (harpData_.lastDevice_ != dev) return;

        harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].r_ = 0;
        harpData_.keygroups_[EHarp::KeyGroup::KG_MAIN].c_ = 0;
        harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].r_ = 0;
        harpData_.keygroups_[EHarp::KeyGroup::KG_PERC].c_ = 0;
        harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].r_ = 0;
        harpData_.keygroups_[EHarp::KeyGroup::KG_FUNC].c_ = 0;
        harpData_.lastDevice_ = "";
    }

    void key(const char* dev, unsigned long long t, unsigned course, unsigned k, bool a, float p, float r, float y) override {
        if (harpData_.lastDevice_ != dev) return;

        bool percKey = false;
        bool mainKey = false;
        bool funcKey = false;

        unsigned key = k;
        long keyId = 0;

        switch (harpData_.type_) {
            case EHarp::PICO: {
                if (course > 0) {
                    funcKey = true;
                    keyId = makeKeyId(key % 4, 0);
                } else {
                    mainKey = true;
                    keyId = makeKeyId(key % 9, key / 9);
                }
                break;
            }

            case EHarp::TAU: {
                if (course > 0) {
                    funcKey = true;
                    keyId = makeKeyId(key % 8, 0);
                } else {
                    if (key >= 72) {
                        percKey = true;
                        key = key - 72;
                        keyId = makeKeyId(key % 12, 0);
                    } else {
                        mainKey = true;
                        // 16,16,20,20
                        int row = key < 32 ? key % 16 : (key - 32) % 20;
                        int col = key < 32 ? key / 16 : ((key - 32) / 20) + 2;
                        keyId = makeKeyId(row, col);
                    }
                }
                break;
            }
            case EHarp::ALPHA: {
                if (key >= 120) {
                    percKey = true;
                    key = key - 120;
                    keyId = makeKeyId(key % 12, 0);
                } else {
                    mainKey = true;
                    keyId = makeKeyId(key % 24, key / 24);
                }
                break;
            }
        }

        if (mainKey || percKey) {
            auto& voices = mainKey ? harpData_.mainVoices_ : harpData_.percVoices_;
            auto v = voices.findVoice(keyId);
            if (v) {
                if (v->active_ && !a) {
                    voices.freeVoice(v);
                    v = nullptr;
                }
            } else {
                if (a) {
                    v = voices.findFreeVoice(voices.MAX_VOICE);
                }
            }

            if (v) {
                v->updateVoice(keyId, a, p, r, y);
            }
        } else if (funcKey) {
            auto& voices = harpData_.funcVoices_;
            auto v = voices.findVoice(keyId);
            if (v) {
                if (v->active_ && !a) {
                    voices.freeVoice(v);
                    v = nullptr;
                }
            } else {
                if (a) {
                    v = voices.findFreeVoice(voices.MAX_VOICE);
                }
            }
            if (v) {
                v->updateVoice(keyId, a);
            }
        }
    }

    void breath(const char* dev, unsigned long long t, float val) override {
        if (harpData_.lastDevice_ != dev) return;

        harpData_.breathV_.set(val);
    }

    void strip(const char* dev, unsigned long long t, unsigned strip, float val, bool a) override {
        if (harpData_.lastDevice_ != dev) return;

        if (strip > EHarp::MAX_STRIP) return;
        if (a) {
            harpData_.stripV_[strip - 1].set(val);
        }
        // } else {
        // 	// todo: abilty to hold value... rather than assume reset to 0.?
        // 	harpData_.stripV_[strip - 1].set(0);
        // }
    }

    void pedal(const char* dev, unsigned long long t, unsigned pedal, float val) override {
        if (harpData_.lastDevice_ != dev) return;

        if (pedal > EHarp::MAX_PEDAL) return;
        harpData_.pedalV_[pedal - 1].set(val);
    }

    EHarp& harpData_;
};
