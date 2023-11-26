# Installation 


--------------------------------------------------
## MacOS : Apple Silicon 

copy plugin file
MetaMorph-2.1.0-mac-arm64.vcvplugin

to Rack plugins directory
defaults to:  
~/Documents/Rack2/plugins-mac-arm64

--------------------------------------------------

## MacOS : Intel (64 bit only)

copy plugin file
MetaMorph-2.1.0-mac-x64.vcvplugin

to Rack plugins directory
defaults to:
~/Documents/Rack2/plugins-mac-x64


--------------------------------------------------
THEN restart VCV
--------------------------------------------------



## Scale and Pattern files

scales
~/Documents/Rack2/plugins-mac-*/MetaMorph/res/scales
each file is a scala file, many can be found on the internet.


illuminator patterns
 ~/Documents/Rack2/plugins-mac-*/MetaMorph/res/patterns
these are a json file format, see example

Note: about scala and pattern files 
THESE ARE LOADED INTO MEMORY to allow instant switching.
only add files into these directories you ACTUALLY USE!

these are also only loaded when MODULE is created.
