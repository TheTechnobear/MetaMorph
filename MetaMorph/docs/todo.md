# fix me and things to do...
aka observations about how to get this working ! 


# performance

performance looks ok... seems to take about 9-10% in release build
a 16 voice poly is around 28% with EH
however, its around 25% without using EH... 

in debug rack, 16 voices is over 300%...
but thats also true without EH

but means we want a voice limitation feature! 



# todo 
## custom json
store voice limit
device filter


## check EigenLite
breath/strip/pedal are all unsigned... I though breath was bipolar?
... or is this inferred from 'value' ?

## voice limit
simple parameter for max voice, to reduce from 16.
.. implemented as menu item, but this does not appear to 'initialise' correctly or be save...
(may need to use custom json save/load - needed aanywa)
currently have put this as knob on panel, but feels intrusive


### keygroup size info.
currently Ive been sending wih the key message, as I thought we only needed to process a key
(which usually is the case... e.g. routing, scale message )

however, when scaler does static lighting.. so it needs to know before any button is touched.
and this is not a rare case.. we have static lighting in many places.. 
(e.g. octave buttons, so function, of keygroup switch)

so kg size cannot be sent with key messages... they need to be present from the moment a harp connects.



### fix module to one eigenharp?
this does not seem like a bad ideas.. 
most of the downstream modues are going to be configured with respect to a layout
e.g. function buttons placed on cols 4/5 of alpha... which doesnt work on pico ;) 

also lights... need to be target to a particular device... 

how should this look? 
I think device filter ANY, Base, Pico,  1..N is fine.

but ANY will mean first device only... 
its probably not a priority at the moment though.








