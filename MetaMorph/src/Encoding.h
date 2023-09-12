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
inline void encodeKey(long keyId, float& key) {
    long kid = keyId;
    unsigned kg_r, kg_c, r, c;
    c = kid & 0xff;
    kid = kid >> 8;
    r = kid & 0xff;
    kid = kid >> 8;
    kg_c = kid & 0xff;
    kid = kid >> 8;
    kg_r = kid & 0xff;

    char* keyStream = (char*)(&key);
    keyStream[0] = (char) kg_r;	
    keyStream[1] = (char) kg_c;
    keyStream[2] = (char) r;
    keyStream[3] = (char) c;
}

// encode/decode floats 

inline void encoderKey(float& key, unsigned kg_r,unsigned kg_c, unsigned r, unsigned c) {
    char* keyStream = (char*)(&key);
    keyStream[0] = (char) kg_r;	
    keyStream[1] = (char) kg_c;
    keyStream[2] = (char) r;
    keyStream[3] = (char) c;
}

inline void decoderKey(float key, unsigned& kg_r,unsigned& kg_c, unsigned& r, unsigned& c) {
    char* keyStream = (char*)(&key);
    kg_r = (unsigned) keyStream[0];	
    kg_c = (unsigned) keyStream[1];	
    r = (unsigned) keyStream[2];	
    c = (unsigned) keyStream[3];				
}

