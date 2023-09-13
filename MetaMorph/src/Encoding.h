#pragma once


// create a keyid
inline long makeKeyId(unsigned kg_r,unsigned kg_c, unsigned r, unsigned c) {
    long k = 0;
    k = (kg_r & 0xff);
    k = (k << 8) + (kg_c & 0xff);
    k = (k << 8) + (r & 0xff);
    k = (k << 8) + (c & 0xff);
    return k;
}

// encoder a keyid into float
inline void encodeKey(long keyId, float& msg) {
    long kid = keyId;
    unsigned kg_r, kg_c, r, c;
    c = kid & 0xff;
    kid = kid >> 8;
    r = kid & 0xff;
    kid = kid >> 8;
    kg_c = kid & 0xff;
    kid = kid >> 8;
    kg_r = kid & 0xff;

    char* stream = (char*)(&msg);
    stream[0] = (char) kg_r;	
    stream[1] = (char) kg_c;
    stream[2] = (char) r;
    stream[3] = (char) c;
}

// encode/decode floats 

inline void encoderKey(float& msg, unsigned kg_r,unsigned kg_c, unsigned r, unsigned c) {
    char* stream = (char*)(&msg);
    stream[0] = (char) kg_r;	
    stream[1] = (char) kg_c;
    stream[2] = (char) r;
    stream[3] = (char) c;
}

inline void decoderKey(float msg, unsigned& kg_r,unsigned& kg_c, unsigned& r, unsigned& c) {
    char* stream = (char*)(&msg);
    kg_r = (unsigned) stream[0];	
    kg_c = (unsigned) stream[1];	
    r = (unsigned) stream[2];	
    c = (unsigned) stream[3];				
}

