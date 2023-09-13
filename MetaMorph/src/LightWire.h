#pragma once

enum LedMsgType {
    LED_CLEAR_ALL,
    LED_SET_OFF,
    LED_SET_GREEN,
    LED_SET_RED,
    LED_SET_ORANGE
};

inline void createLedClearMsg(float& msg,unsigned r, unsigned c) {
    char* stream = (char*)(&msg);
    stream[0] = (char) (0x10 | LED_CLEAR_ALL);
    stream[1] = (char) 0x00;
    stream[2] = (char) ( r & 0xff);
    stream[3] = (char) ( c & 0xff);
    
}

inline void createLedMsg(float& msg, unsigned r, unsigned c, LedMsgType t) {
    char* stream = (char*)(&msg);
    stream[0] = (char) (0x10 | t);
    stream[1] = (char) 0x00;
    stream[2] = (char) ( r & 0xff);
    stream[3] = (char) ( c & 0xff);
}

inline void decodeLedMsg(float msg, LedMsgType& t, unsigned& r, unsigned& c) {
    // unsigned dummy=0;
    char* stream = (char*)(&msg);
    t = (LedMsgType) ( stream[0] & 0x0f);	
    // dummy = (unsigned) stream[1];	
    r = (unsigned) stream[2];	
    c = (unsigned) stream[3];				
}



template<class T>
class MsgQueue {
public:
    MsgQueue(unsigned maxSz=10) : maxSize_(maxSz) {
        queue_ = new T[maxSize_];
    }
    
    bool write(T x) {
        queue_[writePtr_] = x;
        writePtr_ = (writePtr_ + 1) % maxSize_;

        //if readptr==writeptr we have filled queue to max
        return writePtr_ != readPtr_;
    }

    bool read(T& x) {
        if(readPtr_ == writePtr_) {
            return false;
        }
        x = queue_[readPtr_];
        readPtr_ = (readPtr_ + 1) % maxSize_;
        return true;
    }

    void clear() {
        readPtr_=writePtr_=0;
    }

    T* queue_;
    unsigned maxSize_;

    unsigned readPtr_=0,  writePtr_=0;

};