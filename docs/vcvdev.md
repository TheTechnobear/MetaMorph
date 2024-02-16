# vcv dev setup
https://vcvrack.com/manual/Building

you can either just use the SDK dylib or build and use your own RACK standalone build.



if you are using RACK then ensure you set RACK_DIR


build is based on make

```
make 
make install
```

make install will place in your installed Rack 


if using a dev version dont use install... it can install in rack dir

### cross compile
```
make CROSS_COMPILE=x86_64-apple-darwin 
make CROSS_COMPILE=x86_64-apple-darwin install
```

```
make CROSS_COMPILE=arm64-apple-darwin 
make CROSS_COMPILE=arm64-apple-darwin install
```

always do a `make clean` before cross compiling




# vcv grouping of modules
VCV has 3 concepts for modules

brand = developer, so for me TheTechnobear
plugin = group of related modues
module = actual module.

brand is used for selecting/grouping in browser and on store.
plugin is a compilation unit, so collection of modules, user does not realy see this much.
module is what we place on canvas etc.



# releasing
for some reason building on latest macOS will cause issues on older macOS versions.
this is being handled by releasing via github actions, which builds on older (and compatible) macOS.
see below for more details on this

# development process 


https://vcvrack.com/manual/PluginDevelopmentTutorial


## create a plugin
so the grouping for your plugins.
for many developers they will just have one plugin, with all modules in it
e.g so brand = plugin

```
$RACK_DIR/helper.py createplugin EigenLite 
```

basically this creates plugin.json/cpp/hpp and makefile
but with no modules ! 

## create module

now we need to create the actual modules that are within the plugin

the easiest way to do this is to start with a module svg image
as we will need this for the backround and text anyway. 

so, in inkscape create an SVG panel, 


https://vcvrack.com/manual/Panel


some notes : 

height 128.5 mm 
width  HP *  5.08 mm 
so 10hp = 50.8mm

mm is used on doc preferences

now we can create the initial classes from the panel 

```
$RACK_DIR/helper.py createmodule EDevice res/EDevice.svg src/EDevice.cpp
```

note: do not use helper.py on existing modules, you'll overwrite all your custom code!
really, its only useful to create initial template class.


this will look in the 'components' layer for circles/ellipsefor and generate jacks/knobs based on colour

component will use centre of circle
you can also use rectange, top-left will be used, useful for custom widgets

colours determine usage : 

Param: red #ff0000
Input: green #00ff00
Output: blue #0000ff
Light: magenta #ff00ff
Custom widgets: yellow #ffff00

use the 'Label' field of an object and this will become cpp name.

hide the components layer so its not displayed in VCV


text - you must convert to a PATH , this avoids vcv having to have fonts.
Path > Object to Path


notes: 
5mm = normal round knob - default from generation 
10mm = huge round knob


the svg is used 'as is' you only need to regenerate wth createmodule if you add new components.
... but this is a destructive process as re-writes module.cpp
workaround : you could write to a temp cpp file and then copy paste generated code to the 'real' module.cpp


## using vscode

setup RACK_DIR 
code . in main directory

in $RACK_DIR
```
cd plugins 
ln -s "MetaMorph project directory " MetaMorph

```

launch task is using
cd $RACK_DIR
./Rack -d . 

so is using MetaMorph plugin  from plugins/MetaMorph
which you have linked with ln above, to the built version!


## git action
this is setup in .github/workflows/build-plugin.yml

notes: 
it uses the RackSDK which is donloaded from vcvrack.com
it cross compiles for various targets 
using an older version of macOS for compatiblity.

its CI, a release is created when a tag is used of the for  v1.2.3



-------

## plugin guide
https://vcvrack.com/manual/PluginGuide

## debugging with vscode
note: this implies building debug version of rack executable?!
https://community.vcvrack.com/t/debug-plugins-with-vscode/19068/14