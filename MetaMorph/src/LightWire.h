#pragma once

enum LedMsgType {
    LED_SET_OFF,
    LED_SET_GREEN,
    LED_SET_RED,
    LED_SET_ORANGE
};

inline float encodeLedMsg(
    LedMsgType t,
    unsigned startr, unsigned startc,
    unsigned sizer, unsigned sizec) {
    float msg;
    long lmsg = 0;
    lmsg = (lmsg << 6) + (startr & 0x3f);
    lmsg = (lmsg << 6) + (startc & 0x3f);
    lmsg = (lmsg << 6) + (sizer & 0x3f);
    lmsg = (lmsg << 6) + (sizec & 0x3f);
    lmsg = (lmsg << 6) + (t & 0x03);

    char* pL = (char*)&lmsg;
    char* stream = (char*)(&msg);
    stream[0] = (char)pL[0];
    stream[1] = (char)pL[1];
    stream[2] = (char)pL[2];
    stream[3] = (char)pL[3];
    return msg;
}

inline void decodeLedMsg(float msg, LedMsgType& t,
                         unsigned& startr, unsigned& startc,
                         unsigned& sizer, unsigned& sizec) {
    long lmsg = 0;
    char* pL = (char*)&lmsg;
    char* stream = (char*)(&msg);
    pL[0] = unsigned(stream[0]) & 0xff;
    pL[1] = unsigned(stream[1]) & 0xff;
    pL[2] = unsigned(stream[2]) & 0xff;
    pL[3] = unsigned(stream[3]) & 0xff;

    t = (LedMsgType)(lmsg & 0x3);
    lmsg = lmsg >> 6;
    sizec = (lmsg & 0x3f);
    lmsg = lmsg >> 6;
    sizer = (lmsg & 0x3f);
    lmsg = lmsg >> 6;
    startc = (lmsg & 0x3f);
    lmsg = lmsg >> 6;
    startr = (lmsg & 0x3f);
}

static constexpr int MAX_MSGS = 100;

template <class T>
class MsgQueue {
   public:
    MsgQueue(unsigned maxSz = MAX_MSGS) : maxSize_(maxSz) {
        queue_ = new T[maxSize_];
    }

    bool write(T x) {
        queue_[writePtr_] = x;
        writePtr_ = (writePtr_ + 1) % maxSize_;

        // if readptr==writeptr we have filled queue to max
        return writePtr_ != readPtr_;
    }

    bool read(T& x) {
        if (readPtr_ == writePtr_) {
            return false;
        }
        x = queue_[readPtr_];
        readPtr_ = (readPtr_ + 1) % maxSize_;
        return true;
    }

    void clear() {
        readPtr_ = writePtr_ = 0;
    }

    T* queue_;
    unsigned maxSize_;

    unsigned readPtr_ = 0, writePtr_ = 0;
};