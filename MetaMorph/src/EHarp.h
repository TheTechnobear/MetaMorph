#pragma once

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
    float tar_=1.0f;
};

class bipolarValue {
public:
    bipolarValue(float x) : cur_(x), tar_(x) {}
    inline void set(int x) {
        tar_ = float(x * 5)/ 4096.0f;			
    }
    inline float next(float stepPct_) {
        cur_ += (tar_ - cur_) * stepPct_; 
        return cur_;
    }
private: 
    float cur_=1.0f;
    float tar_=1.0f;
};

class unipolarValue {
public:
    unipolarValue(float x) :  cur_(x), tar_(x) {}
    inline void set(unsigned x) {
        tar_ = float(x * 10)/ 4096.0f;			
    }
    inline float next(float stepPct_) {
        cur_ += (tar_ - cur_) * stepPct_; 
        return cur_;
    }
private: 
    float cur_=1.0f;
    float tar_=1.0f;
};

class Voice {
public: 
    Voice() = default;
    virtual ~Voice() = default;
    bool active_=false;
    unsigned key_ =0;

    virtual void freeVoice() = 0;
};

template <class T>
class Voices {
    public:
    static constexpr unsigned MAX_VOICE = 16;

	T* findFreeVoice() {
		for(auto& v : voices_) {
			if(!v.active_) {
				return &v;
			}
		}
		return nullptr;
	}

	T* findVoice(unsigned key) {
		for(auto& v : voices_) {
			if(v.active_ && v.key_ == key) {
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
    keyValue keyV_=0.0f;
    unipolarValue pV_=0.0f;
    bipolarValue rV_=0.0f;
    bipolarValue yV_=0.0f;

    void updateVoice(unsigned key, bool a, unsigned p, int r, int y) {
        active_ = a;
        key_ = key;
        keyV_.set(key_);
        pV_.set(active_ ? p : 0);
        rV_.set(active_ ? r : 0);
        yV_.set(active_ ? y : 0);
    }

    void freeVoice() override {
        updateVoice(0,false,0,0,0); 
    }
};


struct FunctionVoice : public Voice {
    keyValue keyV_=0.0f;
    unipolarValue actV_=0.0f;

    void updateVoice(unsigned key, bool a) {
        key_ = key;
        keyV_.set(key_);
        active_ = a;
        actV_.set(a ? 4096 : 0);
    }
    void freeVoice()  override {
        updateVoice(0,false);
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

    bipolarValue breathV_=0.0f;
    unipolarValue stripV_[2]= {0.0f,0.0f};
    unipolarValue pedalV_[2]= {0.0f,0.0f};
    Voices<FullVoice> mainVoices_;
    Voices<FullVoice> percVoices_;
    Voices<FunctionVoice> funcVoices_;
};

class EHarpCallback : public EigenApi::Callback{
public:
    EHarpCallback(EHarp& harp) : harpData_(harp) {

    }

    void connected(const char* dev, DeviceType dt) override {
        
        switch (dt) {
            case PICO : {
                harpData_.type_ = EHarp::PICO;
                break;
            }
            case TAU : {
                harpData_.type_ = EHarp::TAU;
                break;
            }
            case ALPHA : {
                harpData_.type_ = EHarp::ALPHA;
                break;
            }
        }

    }


    void disconnected(const char* dev) override
    {
    }


    void key(const char* dev, unsigned long long t, unsigned course, unsigned k, bool a, unsigned p, int r, int y) override  {
        bool percKey = false;
        bool mainKey = false;
        bool funcKey = false;

        unsigned key = k;
        long keyId = 0;

        switch(harpData_.type_) {
            case EHarp::PICO : {
                if(course > 0 ) {
                    funcKey = true;
                    keyId = makeKeyId(4,1, key % 4, 0);
                }
                else {
                    mainKey = true;
                    keyId = makeKeyId(9,2, key % 9, key / 9);
                }
                break;
            }

            case EHarp::TAU : {
                if(course > 0 ) {
                    funcKey = true;
                    keyId = makeKeyId(8,1, key % 8, 0);
                }
                else {
                    if(key >=72 ) {
                        percKey = true;
                        key = key - 72;
                        keyId = makeKeyId(12,1, key % 12, 1);
                    } else {
                        mainKey = true;
                        // 16,16,20,20
                        int row = key < 32 ? key % 16 :(key - 32) % 20;
                        int col =  key < 32 ? key / 16 : ((key - 32) / 20) + 2;
                        keyId = makeKeyId(20,4, row, col);
                     }
                }
                break;
            }
            case EHarp::ALPHA : {
                    if(key >=120 ) {
                        percKey = true;
                        key = key - 120;
                        keyId = makeKeyId(12,1, key % 12, 1);
                    } else {
                        mainKey = true;
                        keyId = makeKeyId(24,5, key % 24, key / 24);
                    }
                break;
            }
        }

        if(mainKey || percKey) {
            auto& voices = mainKey ? harpData_.mainVoices_  : harpData_.percVoices_;
            auto v = voices.findVoice(keyId);
            if(v) {
                if(!a) {
                    voices.freeVoice(v);
                    v = nullptr;
                }
            } else {
                if(a) {
                    v = voices.findFreeVoice();
                }
            }
            if(v) {
                v->updateVoice(keyId,a,p,r,y);
            }
        } else if (funcKey) {
            auto& voices = harpData_.funcVoices_;
            auto v = voices.findVoice(keyId);
            if(v) {
                if(!a) {
                    voices.freeVoice(v);
                    v = nullptr;
                }
            } else {
                if(a) {
                    v = voices.findFreeVoice();
                }
            }
            if(v) {
                v->updateVoice(keyId,a);
            }
        }
    }


    void breath(const char* dev, unsigned long long t, unsigned val) override {
        // ! this should be EigenLite 
        if(breathZeroPoint_ < 0) breathZeroPoint_ = val;
        int iVal = int(val) - breathZeroPoint_;
        if(iVal > 0) {
            iVal =  (float(iVal) / (4096 - breathZeroPoint_)) * 4096;
        } else {
            iVal =  (float(iVal) / breathZeroPoint_) * 4096;
        }
 		harpData_.breathV_.set(iVal);
    }
    
    void strip(const char* dev, unsigned long long t, unsigned strip, unsigned val, bool a) override  {
		if(strip > EHarp::MAX_STRIP) return;
		if(a) {
			harpData_.stripV_[strip - 1].set(val);
		} else {
			// todo: abilty to hold value... rather than assume reset to 0.?
			harpData_.stripV_[strip - 1].set(0);
		}
    }
    
    void pedal(const char* dev, unsigned long long t, unsigned pedal, unsigned val) override  {
		if(pedal > EHarp::MAX_PEDAL) return;
		harpData_.pedalV_[pedal - 1].set(val);
    }
    
    int breathZeroPoint_ = -1;

    EHarp& harpData_;
   
};

