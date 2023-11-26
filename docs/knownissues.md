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


## demo presets 
these are considered examples, so I dont really plan to 'fix' them, nor want to log issues here with them
hopefully these will naturally evolve as the community builds on them and improves. 



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
# Known issues
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


known issues that I intend to resolve/fix, see below for 'limitations'

## General: Disable input
this has not been tested throughly, so may not work at all or partially.
report specific issues with details

## General: No note stealing
polyphony is currently a hard limit, if its set to 4, a 5th note will NOT sound.
we could instead 'steal' the (oldest) note, this generally 'feels' better to most players.
not really a bug/issue, rather a planned improvement.

## Module : Eigenharp  (low priority)
Eigenharp connection does not switch when you filter type - only necessary for multiple eigenharps
*workround*: reload patch, use new instance of Eigenharp module


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
## Limitations
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

limitations are unlikely to be fixed due to stated reasons, often out of my control.
often, I will have ideas on how to improve the situation if not resolve it.

## Module : Eigenharp
polyphony does not appear to be respected/ lost note
example: poly = 4
you play 4 notes, then release one note, and QUICKLY play another note.
the new note does not sound.


usually its not noticed when using higher polyphony. e.g. 16 notes.
this is exaggerated as Im currently **not** note stealing ... see below
however this would **not** fix the issue, just change/improve its behaviour.

reason: this appears to be a firmware issue on eigenharps of 'late reporting'
workaround: use more polyphony

## No linux/windows supprt 
MacOs is being used for intial release, other platforms will be tackled when this is considered stable.
Linux is likely to be added first as its pretty much just compilation/release.
Windows needs more thought/work so is a lower priority.


## 64 bit only
there are no plans to support 32 bit platform


## Demo patches
Ideally I'd like to replicated some of the existing EigenD patches and have similar functionality.
however, the this is not possible (with very complex patching) using the VCV factory modules.
e.g. creating a octave up/down button is difficult with factory modules but trivial with 3rd party modules.

this is a limitation of these demos...
later, the community can create more 'functional' patches using 3rd party modules that are closer to EigenD presets.
