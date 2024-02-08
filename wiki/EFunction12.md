# Function 12: trigger different actions 


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
none

## Outputs
|Name | Values|Desc|
|---|---|---|
|F 1-12| 0/10| output state depending on type|

------------------------------------------------------------------------------

## General Usage 

will assign the first 12 keys in a keygroup to each function.


Gate : means the function will gate whilst you hold the key
Trig : when you press/release the key, you will get a momentary trigger
Toggle: press/release will toggle the state of the functions output.

Function is similar but with 4 functions, is more configurable


## Tips
the LED will only be lit on Eigenharp IF the relevant output is connected.
so its fine to have 'unused' functions