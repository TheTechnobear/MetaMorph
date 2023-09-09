# fix me and things to do...
aka observations about how to get this working ! 


# performance

performance looks ok... seems to take about 9-10% in release build
a 16 voice poly is around 28% with EH
however, its around 25% without using EH... 

in debug rack, 16 voices is over 300%...
but thats also true without EH

but means we want a voice limitation feature! 




# features to do..

1. voice limit
simple parameter for max voice, to reduce from 16.


# issues



I can see a lot of stepping ! 
and this feels like its causing distortion...

checking Rack SDK, process is caused every frame...
i.e. it is not per block... 

this is a bit unncessary for the eigenharp which tops out at about 2khz

```
Key Properties:
Sampled at 2000 samples/second, per key.
10 bit (1024 values) resolution.
Sensitive to direct pressure and to lateral pressure in both directions.
```

so, for 48khz SR, we only need to process every 24 samples... 
i.e. SR / 2khz  = 48000/2000 

processBypass... we need to implement so we can still process messages even if we dont output data


why the distortion stepping? do we need to slew it for use at audio rate...
feels like we might need to... as we are only updating at 2khz, and in 10bit steps.
perhaps use a slew to over 2khz ->48hz... as it will not be noticed !
