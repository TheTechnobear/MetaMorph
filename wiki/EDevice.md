# Eigenharp : device module


## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
|Name | Values |Desc|
|---|---|---|
|Device Type | All, Alpha/Tau, Pico| Filter by device type|
|Device Number | All, 1-4| Filter by device number |
|Slew Rate| 1, 2, 5, 10, 25, 50, 100| how much slew to apply to values |


----------

## Strip 1/2
|Name | Values|Desc|
|Strip Abs| 0 to 10| Absolute touch position on strip, does **not** reset on release|
|Strip Rel| -5 to +5| Relative touch position to initial press, resets on release|
|Strip Gate| 0 / 10| high when strip is being touched|


## Pedal 1/2
|Name | Values|Desc|
|1 /2 | 0 to 10| Position of pedal|

## Breath
|Name | Values|Desc|
|Breath | -5 to +5| positive on blow, negative on draw|


--------
## Mode keys

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
|Gate| 0 /  10| high on pressing key|
|KG| **special** |represents information about the keygroup|

--------
## Main / Perc Keys

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

## Observations

### Pico
Perc Keys, Strip 2, Pedal are not used

### Tau
Strip 2,  Pedal usage depends on basestation.

### Alpha
Mode Keys, Pedal usage depends on basestation.



## Tips


