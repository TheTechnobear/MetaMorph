# fix me and things to do...
aka observations about how to get this working ! 

# issues


# todo 


/ ESplit
/ EDevice - Eigenharp
/ EIlluminator
/ ESwitch
/ EFunction
/ ERSplit - Row splitter 
/ EFunction12
/ EScaler

notes:
ESwitch : leds incorrectly labelled
ERSplit : leds incorrectly labelled
ESplit : led incorrect label for led_s2


## general
### use menu to reduce parameters
things like polyphony can be implemented as context menu
however, this requires using custom json


-----------------------------------------------------

## modules ideas
LightMerge - merge multipe light sources 
becareful with KG... as we need translation?
main purpose is multiple function modules from one split


-----------------------------------------------------


### EigenLite
- reduce calls to msg flush from poll? 

it seems reasonable that we update leds less frequently.
note: set_led just marks leds, its the msg_flush that sends over usb
however, I didnt really notice a big differene, e.g. flashing still existed etc.

bool EF_BaseStation::poll(long long t)
{
    static unsigned pollCount = 0;

    if (!EF_Harp::poll(t)) return false;
    pLoop_->poll(t);

    if(pollCount==0) {
        pLoop_->msg_flush();
    }

    pollCount = (pollCount + 1) % 100;
    return true;
}



-----------------------------------------------------
## module - eigenHarp 


### fix module to one eigenharp?
this implies EigenLite filter will allow ANY,1
currently this would link to both first base and pico.

not sure, without eigenlite api change we do this AND allow no filter

-----------------------------------------------------
## module - scaler 

allow non-chromatic scales
perhaps have a scale selector, which loads from json file?
0 = no scale = chromatic, as this is most effecient.

custom lighting - currently fixed to root, 5th , 7th


-----------------------------------------------------


# module - split 


-----------------------------------------------------

# module - function 


-----------------------------------------------------

# module - switch 

-----------------------------------------------------
