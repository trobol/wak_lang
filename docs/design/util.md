# Utils

generic functions and data strutures not related directly to the parser

for "dynamic" classes I want it to be clear that passing them around is a reference not a copy
for this reason they are dynamically allocated, 
because of the nature of them most of the probelems with dynamic allocation do not apply
- fragmentation: there won't be a large number of these and they wont be freed very often
- forgeting to free: they underlying resource must be freed anyway
- poor cache utilization: this might be a problem, but because most operations are on a single vector in most situations this wont apply

