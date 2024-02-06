# Meta Morph

### Goal
The purpose of Meta Morph is to allow all Eigenharp models to be used within VCVRack (Free and Pro),without any additional software or install.


## Motivation
The key motivation was providing a 'platform' for the Eigenharps that is modern, and well supported - yet extremely flexible.
also, due to low latency and high data bandwidth of the Eigenharp it was important we could do this all at audio rate.
VCV Rack was a perfect fit for this... and opens exciting new ways to use the Eigenharp.

furthermore, I wanted to provide a route that means other developers can add 'eigenharp' functionality to VCV without needing to understand a complex codebase.
to this end, I have documented the wire protocol, I use between meta morph modules to allow for this.





### Credits:
Id like to thank the following, without thier help this would not be possible.
- Ben Norland for work on the panel design, and also the meta morph logo.
- EigenLabs for embracing open source, making this possible
- VCVRack for having an open api, with a GPL policy 

and of course, users who provide valueable feedback, making this better than I can alone.


----------------------------------------------------------------------

this doc is for notes that may well be needed for release
e.g protcol 

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


----------------------------


# Modules 

## Eigenharp 
has to be connected to specific eigenharp i.e. ONE not more using filter
voices are allocated per output group using polyphony to maximise

max of 16 voices per group.

## Scaler
scaler does NOT re-voice

## Splitter 
Splitter DOES re-voice, based on polyphony of each split
you can overlap splits


## Function


## Switch




