# Developing modules to work with Meta Morph

NOTE: SUBJECT TO CHANGE

This guide is intended to help you create modules that can interact with Meta Morph modules.

Important notes: 
Generally the development process for this is exactly the same as developing any other VCV Rack Module.
you will NOT need any special libraries or header files, we are interacing with a binary protocol.
This document covers this protocol only, it does not cover VCV development in general.


There are two aspects we need to discuss.

special inputs and voice handling


# Meta Morph inputs and outputs 

generally Meta Morph outputs standard VCV voltages for things like X,Y,Z, Strip etc.
this need no special handling.

However we have 3 special kinds of input/output ports which are 'encoding' information rather than simple voltages.
these are KEY, KG, LIGHT each discussed below



# KEY
this represents a key on the surface, as a row/column pair, which we encode as a float.
internally we use zero based index. (whereas for UI we use 1 based)

```
inline float encodeKey(unsigned r, unsigned c) {
    float msg;
    char* stream = (char*)(&msg);
    stream[0] = (char)0x01;
    stream[1] = (char)0;
    stream[2] = (char)r;
    stream[3] = (char)c;
    return msg;
}

inline void decodeKey(float msg, bool& valid, unsigned& r, unsigned& c) {
    char* stream = (char*)(&msg);
    valid = (bool)stream[0] & 0x1;
    r = (unsigned)stream[2];
    c = (unsigned)stream[3];
}
```

see voice handling to understand how key events represent key on/off ! 

# KG
this represents the SIZE of a keygroup, all keygroups are assumed to start from 0,0

```
inline float encodeKeyGroup(unsigned kg_r, unsigned kg_c) {
    float msg;
    char* stream = (char*)(&msg);
    stream[0] = (char)0;
    stream[1] = (char)0;
    stream[2] = (char)kg_r;
    stream[3] = (char)kg_c;
    return msg;
}

inline void decodeKeyGroup(float msg, unsigned& kg_r, unsigned& kg_c) {
    char* stream = (char*)(&msg);
    kg_r = (unsigned)stream[2];
    kg_c = (unsigned)stream[3];
}
```

# LIGHT
this is very special :) 
the LIGHT ports are used as a SERIAL stream of data. so ZERO volt = no message


### Light Message
the message is a start position (row, column) and then size (row/column).
this allows us to light a range in ONE message


```
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
```

### Light messages

as stated above this is a STREAM of messages.

VCV can only have ONE value on a port a time (simplified), so if we want to send more than one led message.
we have to queue them up and send one at a time.
you will almost ALWAYS need a message queue, where you pull off - I typically use a circular queue (100 elements).

example: 
consider my Scaler module. if the user changes the scale, I have many LEDS to send, first to clear, then to set others.
so,I put ALL of these messages onto a queue. and then each sample (process()) I pull ONE of the queue to send upstream.


### KG and Light
I try to minimise the number of LED messages I send, as they are in queues.
this means, I usually use KG msgs to determine which leds I need to set i.e. don't send whats out of range. 
but this also means, if the KG expands, I might need to refresh LEDs

### Light port connection
if a user connects a wire to the light port, you are likely to need to resend leds.
remember : its serial so upstream module does not know ... unless its keeping track itself. 
note: split/switch do this, since they move KGs around.

### Clearing LEDS
its assumed the downstream module will clear the leds when it needs to.
e.g. if you switch the light source from one to the other, it is the NEW source that will clear the leds.



# Voicing
this is VERY important , voicing / polyphony is THE major way to control cpu usage.
since polyphonic voices will contribute hugely to cpu usage.

but the most important part for this document is how we determine key on/off.

the Eigenharp module voices every KEY, but VCV is limited to 16 channels ! 

if you get a NON ZERO input on KEY for a channel, you know this is a KEY ON.
so can associate that channel with that KEY for the 'lifetime of that channel'

what about KEY OFF?
well its the opposite, (kind of), if you get KEY=0 you know that channel is now OFF.
this means you may need to track KEY -> channel if you want to have 'note off' behaviour.
since you cannot get row/col from KEY since its now ZERO ;) 

also the protocol allows for KEY to change (one a particular channel) 
e.g. ch 1 KEY=1,1 to ch1 KEY=10,11 without a ZERO. this implies a KEY OFF (for 1,1)  since ch1 now belongs to 10,11.
this will happen in note stealing scenarios.















