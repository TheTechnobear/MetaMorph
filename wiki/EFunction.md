# Function : trigger different actions 


## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
none


### Parameters
|Name | Values |Desc|
|---|---|---|
|Type| Gate/Trig/Toggle| Type of output for the funcions|

## Inputs
|Name | Values|Desc|
|---|---|---|
|Disable| 0/10v| disable the module , 10v disables|
|Key| **special** |represents key being pressed on harp|
|Gate| 0/10| gate function from a key, you can use Z too|
|KG| **special** |represents information about the keygroup|

### Outputs
|LED | **special** | digital stream used to populate leds on harp|



--------
## Func 1 - 4

### Parameters
|Name | Values |Desc|
|---|---|---|
|Row| 0-24| Row of function key, 0 is inactive |
|Col| 0-5| Column of function key, 0 is inactive|

### Outputs
|Name | Values|Desc|
|---|---|---|
|F 1-4| 0/10| output state depending on type|

------------------------------------------------------------------------------

## General Usage 
Gate : means the function will gate whilst you hold the key
Trig : when you press/release the key, you will get a momentary trigger
Toggle: press/release will toggle the state of the functions output.

Function12 is similar but with 12 functions, but less configurable

## Tips
the LED will only be lit on Eigenharp IF the relevant output is connected.

