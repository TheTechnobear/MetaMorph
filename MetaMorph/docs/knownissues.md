## General notes
when logging issues please note if the changes is whilst editing a patch or when loading.
often, make a change and encounter and issue, restarting/reloading that patch will fix it.


pay particular careful to LIGHT, KG and KEY inputs, these are kind of 'special'

**LIGHT** is a special kind of cable, as its not a constant voltage, its a 'stream of data'.so has alot of special use-cases.
**KG / KEY** are not a simple voltage, and KG in particular will have major effects no downstream modules.


# Known issues

## Light refresh (General)
generally there are occasions where the LEDS will not refresh as expected.
please log specific bugs, so that we have a comprehensive list.

workaround: changing a parameter on light source module will often fix this.

## Cable add/remove
removing or plugging cables result in unexpected behavioir
LIGHT is the best example, removing/adding light cables should refresh leds
KG may have some examples... but I think should be 'ok'
please log specific bugs, so that we have a comprehensive list.

workaround: reload patch, change upstream module


## Eigenharp module
connection may not switch when you change type
workround: reload patch, use new instance of Eigenharp module


## Disable input
this has not been tested throughly
there may be use-case it does not act as expected, particuarly not activating/deactivating lights.
