# Splitter : Split keygroups into two


## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
none


## Primary inputs 

### Inputs
|Name | Values|Desc|
|---|---|---|
|Disable| 0/10v| disable the module , 10v disables|
|Key| **special** |represents key being pressed on harp|
|X| -5 to +5| X axis of key,  aka key bend, 0 is untilted|
|Y| -5 to +5| X  axis of key,aka timbre, 0 is untilted|
|Z| 0 to 10| Z axis of key, aka pressure, 0 is untilted|
|KG| **special** |represents information about the keygroup|

### Outputs
|LED | **special** | digital stream used to populate leds on harp|


--------
## Split 1 / 2

### Parameters
|Name | Values |Desc|
|---|---|---|
|Start Row| 1-24| Starting row of split |
|Start Col| 1-24| Starting column of split |
|n Row| 0 - 5| number of rows in split, 0 is inactive |
|n Col| 0 - 5| number of columns in split, 0 is inactive|
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


## Tips
Remember to set the Poly count, if you leave as off, you'll see no acivity! 


