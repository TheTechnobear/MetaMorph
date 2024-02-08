# Scaler : keys to notes


## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
|Name | Values |Desc|
|---|---|---|
|Scale| chromatic + user defined| musical scale to use|



## Parameters
### Layout
|Name | Values |Desc|
|---|---|---|
|x Row| +/- 24| Row multipler |
|x Col| +/- 24| Column multpiler|

### Pitch Bend Range
|Note| +/- 12| Pitch bend range for note input|
|Global| +/- 12|Pitch bend range for global input |

|Ref Note| +/- 12| Reference note for start of scale|

### LEDS 1-3
|Scale Degree| 0-11| degree of scale to light|
|Colour| off, red, green , orange|colour for led, or off |

## Inputs
|Name | Values|Desc|
|---|---|---|
|Disable| 0/10v| disable the module , 10v disables|
|Key| **special** |represents key being pressed on harp|
|Note| -5 to +5| key bend|
|Global| -5 to +5| global bend | 
|KG| **special** |represents information about the keygroup|

### Outputs
|v/oct | -10 to + 10v | v/oct output|
|LED | **special** | digital stream used to populate leds on harp|


## General Usage 
Scaler is very simple to use, but may be daunting at first, as its extremely flexible.

Its primary purpose is convert our keygroup , which is a grid format into a musical note.
and in particualr v/oct which is used in modular to define pitch.

I will start with a basic default scale (chromatic).

X Row determines for each row in the keygroup, how many scale degress will it increase/decrease.
X Column determines for each column in the keygroup, how many scale degress will it increase/decrease.

so for a chromatic scale, with xRow = 1, xCol=4
then you will have semitones going down the harp, and then next column will be a 4th above. 
so a common isomophic layout.

if you did something like xRow = 5, and xCol = -1, then the scale would be playes across the harp from left to right.

but importantly, I said above that xRow/xCol are measures of scale degrees.
thats because we can use a scale (defined as a scala file , see below), 
therefore we can have steps in diatonic scales, or even microtonal scales.

when we decide on using a scale, its important we use a 'reference note'.
i.e we have a major scale, then the reference note needs to be defined as C, Bb, D etc.
this is what we use Ref Note for...

Ref Note 0 = C, and proceeds up/down in semitones.


once we have determined this base v/oct from the scale.
we then can apply a pitch bend  either at the key (note input) or global level (e.g for ribbon strips)


Finally, for visualisation, then LED section allows us to light up to 3 different scale degrees with 3 different colours.



## Tips


### Scale files

we use scala file format to defined scales, as these are freely available to download
place the ones you want to use in 
documents/Rack2/MetaMorph/scales


**important note** 
ONLY place scales you are using, all scales present are loaded into RAM.
so you are placing additional load on your computer for redundant files.
also its only a simple list in the menu, since i dont expect users to be using many.
unlikely to change in the near future ;)  
