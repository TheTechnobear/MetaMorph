# readme

this doc is for notes that may well be needed for release
e.g protcol 

# General notes

## Red patch points
these are data cables, only connect to same name inputs

## Key 
encoded row/column - bit is set when valid key
0v = not active

downstream modules can see a voice is turning off, 
by looking for transition from valid key (high bit) to invalid (0v)

KEY IS NOT V/OCT.. you need SCALER to convert to V/OCT

## Key Group
encoded row/column size

## Lights
serial stream of led info
NEVER mix ! will need an led mixer module for this

## Channeling / voicing
voices are allocated on device module and re-allocated for splits etc.
you can share data between modules as long as they are not re-voiced/channeled
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


