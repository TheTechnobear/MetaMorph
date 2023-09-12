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
1. custom json
store voice limit
device filter


2. elitelite change 
allow connection of specific device
idea... 
change to use usb:enumerate(, , finder_t). this will give us multiple devices.
just report as pico_1, pico_2, tau_1, alpha_1  etc... so we assume usb enumerate is consistent.

eigenlite will then default to 'load first', but you can also specify filter above.
.. this 'filter' will be stored in custom json

note: discoverthread is separate from poll...
so use a quick n simple, volatile bool to protect... 
poll always just moves on ... catch you next time.
discover, will do another check on flag quickly after... 
so a kind of 'spin lock'

rememeber discover thread is not checking that often.

3. check EigenLite
breath/strip/pedal are all unsigned... I though breath was bipolar?
... or is this inferred from 'value' ?





# notes

1. voice limit
simple parameter for max voice, to reduce from 16.
.. implemented as menu item, but this does not appear to 'initialise' correctly or be save...
(may need to use custom json save/load - needed aanywa)