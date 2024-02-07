# fix me and things to do...
aka observations about how to get this working ! 

# release plan

- testing, see below
- move documentation to wiki (from forum)
- consistency of module name (e.g. EFunction -> Function)
- check tags in plugin.json


# Testing notes

# general 

crash at startup, something to so with paramquantity
... need to isolate which module!

random crash on deletion... 



-----------------------------------------------------
## module - eigenharp 

strip 1 : rel - voltage range incorect.
strip 1 : gate - not working

untested so far: 
strip 2
pedals 
strip 1 abs

-----------------------------------------------------
# module - function12 

tested : ok

-----------------------------------------------------
# module - function 

tested : ok


-----------------------------------------------------
## module - scaler 

leds checked, 
untested, musical e.g refnote and pb etc.

(led wrong order : fixed in code , not in svg)

-----------------------------------------------------
# module - splitter 

-----------------------------------------------------
# module - rowsplitter 


-----------------------------------------------------
# module - switch 

-----------------------------------------------------
# module - illuminator 




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


-----------------------------------------------------
## module - scaler 

custom lighting - currently fixed to root, 5th , 7th


-----------------------------------------------------


# module - split 


-----------------------------------------------------

# module - function 


-----------------------------------------------------

# module - switch 

-----------------------------------------------------
