# Overview 
a grab bag of topics that will help you get more from Meta Morph


## Common Labelling

Key : represents row,column - not a usable voltage must be connected to another KEY
KG : represents keygroup SIZE- not a usable voltage must be connected to another KG
LED : used to trasmit LED information, not a usable voltage must be connected to another LIGHT

X : represents vertical action of a key, often used for key pitch bend, -5v to 5v
Y : represents horizontal/yaw of a key, often used for timbre, -5v to 5v
Z : represents the pressure on a key, 0v to 10v

## KEY, KG, LED

you must connect KEY, KG and LED (optional) to other Meta Morph modules of the same name - these are NOT general purpose voltages.
e.g. Key is NOT v/oct. to get a v/oct you must use SCALER, which translates KEY to v/oct.

## Polyphony
Polyphony is your main control over CPU usage, use it wisely !
e.g. if you have 16 voices the patch will use 16x resources.
be careful particularly with Split and Switch use these to reduce polyphony!


### Avoiding clicks

a simple voice patch will do something like
Key -> (key) Scaler Key (v/oct) -> (v/oct) Oscillator -> VCA 
Z -> VCA -> Output
this is fine for testing, but you may notice that you get some clicks when you initially press a key - it really spoils the sound.

reason: on the Eigenharp Z is very sensitive and will jump from 0 virtually instantaneously, and some audio discontinuities, if you drive a VCA directly.
solution: use a slew limiter (vcv : process does the trick) to slightly smooth the values.
even a tiny slew, like 1-2ms will make a huge difference, and still feel responsive.

its possible to get similar artefacts from modulate with x/y but its not as common -but can be worth trying, if you feel like the sound has some aliasing etc.
(besides, adding slews can actually be a nice sound design tool anyway)