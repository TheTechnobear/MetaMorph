## Reporting bugs/issue

Please be VERY specific about issues.
I will not look at complex patches to debug issues.

please replicate issues in a simplied patch, using the absolute minimun number of modules and connections.
often this will be no more than Eigenharp module + a couple of other modules.
note: I do not need voices/sounds ... look at the vcv cable voltages to highlight the issue

simple principle is,  the better you demostrate the issue, the quicker I can debug and fix.
if I get a 'fuzzy' bug report, then It'll take me more time, and so will get a lower priority.

## editing vs loading:  

when logging issues please note if the changes is whilst editing a patch or when loading.
usually restarting/reloading that patch will fix issues. 
when you report issue please state if its whilst editing, and if reloading module/patch will fix.

## LIGHT, KG, KEY

pay careful attention to LIGHT, KG and KEY inputs, these are kind of 'special' and so make be source of issues.

**LIGHT** its not a constant voltage, its a 'stream of data'.so has alot of special use-cases.
**KG / KEY** are not a simple voltages, and KG in particular will have major effects no downstream modules.

do NOT expect to see visible voltages on LIGHT, KG, KEY in VCV , for varios reasons, often they will appear as zero volts.
this is to be expected, and is not a bug/issue.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


# Known issues

## General: Light refresh 
generally there are occasions where the LEDS will not refresh as expected.
related : order you plug in cables will affect this... so pls test this.

report specific issues
*workround*:: changing a parameter on light source module will often fix this.

## General: Cable add/remove
removing or plugging cables result in unexpected behaviour.
related : order you plug in cables will affect this... so pls test this.

report specific issues
*workround*:: reload patch, change upstream module


## Module : Eigenharp  (low priority)
Eigenharp connection does not switch when you filter type
(only necessary for multiple eigenharps)
*workround*: reload patch, use new instance of Eigenharp module

## Module : Eigenharp polyphony
polyphony does not appear to be respected/ lost note
example: poly = 4
you play 4 notes, then release one note, and QUICKLY play another note.
the new note does not sound.

this appears to be an underlying firmware issue with Eigenharps reporting key off.
usually its not noticed when using higher polyphony. e.g. 16 notes.
this is exaggerated as Im currently **not** note stealing ... see below
however this would NOT fix the issue, just change its behaviour.

## General: No Note stealing
polyphony is currently a hard limit, if its set to 4, a 5th note will NOT sound.
we could instead 'steal' the (oldest) note.
this generally 'feels' better to most players.

## General: Disable input
this has not been tested throughly, so may not work at all or partially.
report specific issues with details