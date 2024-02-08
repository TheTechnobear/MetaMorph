# Illuminator : lights the leds


## Developer: TheTechnobear
I develop these plugins for free, please consider supporting my efforts with a donation.
https://ko-fi.com/thetechnobear


## Menu Items
|Name | Values |Desc|
|---|---|---|
|Pattern| None + user defined| select led pattern file|


### Parameters
None

## Inputs
|Name | Values|Desc|
|---|---|---|
|Disable| 0/10v| disable the module , 10v disables|
|Key| **special** |represents key being pressed on harp|
|KG| **special** |represents information about the keygroup|

### Outputs
|LED | **special** | digital stream used to populate leds on harp|



## General Usage 
This module is designed to allow arbitary lighting.
if you'd prefer not to use lighting from the scaler module, 
then dont connnect the scaler led output, an use this instead



## Tips

## Pattern file 

this is a json format file.
text based so you can edit with any text editor

located in documents/Rack2/MetaMorph/Patterns

diamond.json is an example file, which you can use as a template.

the file is made up of 3 groups (technically json arrays)
green, red, orange
in each you can specify the led coordinates you wish to light with that colour.
the co-ordinates are row,col


simple example
```
{
    "green"  :  [
        [1,2],
        [5,3],
        [9,4]
    ],
    "red"  :  [
        [3,1],
        [7,1]
    ],
    "orange"  :  [
        [3,3]
    ]
}
```

we have 3 green leds on 1,2 (row 1, col 2); 5,3 and 9,4
then 2 red leds on 3,1 and 7,1 
and finally 1 orange led at 3,3.

the format of the fie is very important ! 

basically you can see we are making 'lists' and the last in the list does NOT have a comma other items to !! 
(e.g. 9,4 and 7,1 and 3,1), 
if you remember that, you should not have any issues lol, but its easy to get wrong.


