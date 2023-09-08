# fix me and things to do...
aka observations about how to get this working ! 



# rpath
Im not sure how to get rpath working within this plugin ... needs to be done via makefile
currently Ive copied picodecoder to /usr/local/lib

one option could be to make a static library version of pico decoder...useful for everyone!

# EigenLite location/submodule
we want to build EigenLite as a static library this seems to work

however, the vcvrack makefile systems wants everything UNDER the plugin directory.
as it copies source to build...

so I probably need to get the subdirectory under vcv module directory,
but this is going to be confusing as VCVModules/EigenLite/EigenLite
I probably need to rename this plugin to avoid this confusion.

# ihx files

we could include as resource ...
or
```
 xxd -i pico.ihx pico.cpp    
```
this will create a cpp file with the ihx as a data structure that we could store in binary

