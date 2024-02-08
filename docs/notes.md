
# General notes

## Red patch points
these are data cables, only connect to same name inputs

## Key 
encoded row/column - bit is set when valid key
0v = not active !

downstream modules can see a voice is turning off,   ( * ) 
by looking for transition from valid key (high bit) to invalid ( 0v) 


KEY IS NOT V/OCT.. you need SCALER to convert to V/OCT

## Key Group
encoded row/column size

## Lights
serial stream of led info
NEVER mix ! will need an led mixer module for this

## Channeling / voicing
voices are allocated on device module and re-allocated for splits etc.
you can share data between modules as long as they are not re-voiced/channeled

## nomenclature 

axis, use mpe style
x - pitchbend, aka roll
y - timbre, aka yaw
z - pressure

# key release handling
this is NOT as simple as it may first appear ! 

keys are voiced, so key = 0v means the key that was previously active is no longer active.
BUT the KEY message (off) does NOT contain the R/C .. its just off

so if you want to track a key being released the way to do this is to basically store the key r/c alongside the channel number.
then when you get 0v on that channel you know that key (you stored r/c) has been released.

this is done because key release is the END OF THE STREAM.
I dont want to artificially keep the stream open a bit longer to send 'key off', 
since its possible (though unlikely) the next sample could turn on a (perhaps different) key




