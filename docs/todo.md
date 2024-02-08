# fix me and things to do...
aka observations about how to get this working ! 

# release plan

- testing, see below
- move documentation to wiki (from forum)




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

