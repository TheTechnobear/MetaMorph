#pragma once




inline float encodeKey(unsigned r, unsigned c) {
    float msg;
    char* stream = (char*)(&msg);
    stream[0] = (char) 0;	
    stream[1] = (char) 0;
    stream[2] = (char) r;
    stream[3] = (char) c;
    return msg;
}

inline void decodeKey(float msg,unsigned& r, unsigned& c) {
    char* stream = (char*)(&msg);
    r = (unsigned) stream[2];	
    c = (unsigned) stream[3];				
}


inline float encodeKeyId(long keyId) {
    long kid = keyId;
    unsigned r, c;
    c = kid & 0xff;
    kid = kid >> 8;
    r = kid & 0xff;
    return encodeKey(r,c);
}

inline long makeKeyId(unsigned r, unsigned c) {
    long k = 0;
    k = (k << 8) + (r & 0xff);
    k = (k << 8) + (c & 0xff);
    return k;
}



inline float encodeKeyGroup(unsigned kg_r, unsigned kg_c) {
    float msg;
    char* stream = (char*)(&msg);
    stream[0] = (char) 0;	
    stream[1] = (char) 0;
    stream[2] = (char) kg_r;
    stream[3] = (char) kg_c;
    return msg;
}

inline void decodeKeyGroup(float msg, unsigned& kg_r, unsigned& kg_c) {
    char* stream = (char*)(&msg);
    kg_r = (unsigned) stream[2];	
    kg_c = (unsigned) stream[3];	
}