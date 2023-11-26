# General Note

## Common Labelling
Key : represents row,column - **not** a usable voltage **must** be connected to another KEY 
KG : represents keygroup SIZE- **not** a usable voltage **must** be connected to another KG 
LIGHT : used to trasmit LED information- **not** a usable voltage **must** be connected to another LIGHT 

X : represent vertical action of a key, often used for key pitch bend. -10v to 10v, its **NOT** v/oct ! 
Y : represent horizontal/yaw of a  key, often used for timbre 
Z : represent horizontal/yaw of a  key, often used for timbre.


## General concepts
you must connect KEY, KG and LIGHT (optional) to other Meta Morph modules.these are NOT general purpose voltages.
e.g. Key is NOT v/oct. to get a v/oct you must use SCALER, which translates KEY to v/oct.

Polyphony is your main control over CPU usage, use it wisely ! 
e.g. if you have 16 voices the patch will use 16x resources.
becareful particularly witi Split and Switch use these to reduce polyphony!



-------------------------------------------------------


# Eigenharp 
The module to communicate with the eigenharp

## Menu Items

## Parameters
Device Filter
Type : None , Base, Pico
Number: Device Number 1..5

Mode Key : Poly : Number of Voices for Mode keys
Main Key : Poly : Number of Voices for Main keys
Percussion Key : Poly : Number of Voices for Percussion keys


## Outputs
Strip 1,Strip 2, Ped 1, Ped 2, Breath  - all 0..10v, except Breath which is -10v to 10v.
Mode Keys : Key, Gate, KG - see above , Gate is simple 0v or 10v. 
Main Keys : Key,X, Y, Z, KG 
Percussion Keys : Key,X, Y, Z, KG 

## Inputs
Mode Keys : Light
Main Keys : Light
Percussion Keys : Light


-------------------------------------------------------


# Scaler
a scaler that converts Meta Morph 'keys' into a v/oct to use with other vcv modules.


## Menu Items
Scale : select scale, name of scala files in res/scales.
None = chromatic.


## Parameters
x Row - row multiplier
x Col - column multiplier
+ Offset - offset scale by N keys

PBR Note : semi range of bend for note input
PBR Global : semi range of bend for global input

Led Scale Colours x 3
Degree : scale degree for led.
Colour : colour for the led (incl OFF)


## Inputs
Key, KG : normal usage.
Note: note bend input , 10v = PBR Note
Global: note bend input , 10v = PBR Note

## Outputs
V/oct : v/oct output
Lights : scale lighting 

-------------------------------------------------------


# Function
Used to drive functions (see type) based off a gate input

## Menu Items

## Parameters
Type : Momentary, Gate, Toggle
Func 1-4
Row, Col : key for function, 0=unassignd.


## Inputs
Key, GATE, KG : normal usage - GATE > 1.5v = on.
Disable : disable module > 1.5v

## Outputs
Func 1-4 : function output
Lights : function active  

-------------------------------------------------------

# Function 12 
Used to drive functions (see type) based off a gate input
this assumes keys number 1..12 , where key = (row * row size) + column

## Menu Items

## Parameters
Type : Momentary, Gate, Toggle


## Inputs
Key, GATE, KG : normal usage - GATE > 1.5v = on.
Disable : disable module > 1.5v

## Outputs
F 1-12 : function output
Lights : function active  

-------------------------------------------------------



# Illuminator 
allows arbitary lighting according to a pattern file located in res/patterns.

## Menu Items
pattern selection : none = off

## Parameters

## Inputs
Key, KG : normal usage
Disable : disable module > 1.5v

## Outputs
Lights : lights according to pattern.

-------------------------------------------------------

# Switch 
switch outputs according to a selector 

## Menu Items

## Parameters
Layer 1-4 : Poly : number of voices allocated to layer

## Inputs
Selector : select layer to be active 1..4v,outside = layer 1 
Disable : disable module > 1.5v
Key,X, Y, Z, KG  - inputs for switching

Layer 1-4 : Lights - light input from layer


## Outputs
Key,X, Y, Z, KG  - outputs for layer 
Lights : lights from switch


-------------------------------------------------------

# Splitter  
a general purpose splitter, splits the surface into 2 layers.


## Menu Items

## Parameters
Split 1-2 : Row/Col : start area of split
Split 1-2 : nRow/nCol : size area of split (zero disables)
Layer 1-2 : Poly : number of voices allocated to layer (zero disables)


## Inputs
Selector : select layer to be active 1..4v,outside = layer 1 
Disable : disable module > 1.5v
Key,X, Y, Z, KG  - inputs for splits
Layer 1-2 : Lights - light input from layer


## Outputs
Layer 1-4: Key,X, Y, Z, KG  - outputs for layer 
Lights : lights from split



-------------------------------------------------------

# Row Splitter  
Splits surface into up to 4 splits by ROW

## Menu Items

## Parameters
Layer 1-2 : nRow : number of rows (zero disables)
Layer 1-2 : Poly : number of voices allocated to layer (zero disables)


## Inputs
Selector : select layer to be active 1..4v,outside = layer 1 
Disable : disable module > 1.5v
Key,X, Y, Z, KG  - inputs for splits
Layer 1-4 : Lights - light input from layer


## Outputs
Layer 1-4: Key,X, Y, Z, KG  - outputs for layer 
Lights : lights from split




