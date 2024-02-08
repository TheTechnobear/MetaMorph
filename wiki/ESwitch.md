# Switch : Switch between different keygroups with cv

## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
none

## Primary inputs 

### Inputs
|Name | Values|Desc|
|---|---|---|
|Selector| 0/1/2/3| select which keygroup is active|
|Disable| 0/10| disable the module , 10v disables|
|Key| **special** |represents key being pressed on harp|
|X| -5 to +5| X axis of key,  aka key bend, 0 is untilted|
|Y| -5 to +5| X  axis of key,aka timbre, 0 is untilted|
|Z| 0 to 10| Z axis of key, aka pressure, 0 is untilted|
|KG| **special** |represents information about the keygroup|

### Outputs
|LED | **special** | digital stream used to populate leds on harp|


--------
## Split 1 -4 

### Parameters
|Name | Values |Desc|
|---|---|---|
|Poly Count| OFF, 1-16 |Number of polyphone voices |

### Inputs
|Name | Values|Desc|
|---|---|---|
|LED | **special** | digital stream used to populate leds on harp|


### Outputs
|Name | Values|Desc|
|---|---|---|
|Key| **special** |represents key being pressed on harp|
|X| -5 to +5| X axis of key,  aka key bend, 0 is untilted|
|Y| -5 to +5| X  axis of key,aka timbre, 0 is untilted|
|Z| 0 to 10| Z axis of key, aka pressure, 0 is untilted|
|KG| **special** |represents information about the keygroup|

------------------------------------------------------------------------------

## General Usage 
if you want to select between differnt layouts, or different synth voices use this module.
connect different 'voice' to each keygroup and then use selector to dynamically route to appropriate voice.

## Tips
Selector the volage is rounded down to select a group i.e. 1.2 v will select keygroup 2.
above 3v, it will always select keygroup 4.

Remember to set the Poly count, if you leave as off, you'll see no acivity! 




