# CRLOG: A fast printf-like logging engine

## Motivation

The idea of **crlog** (which stands for [**CRIU**](http://criu.org) logging
engine) came from the fact that we need a huge pile of debug informtaion
to be logged when doing checkpoint and (or) restore of containers.

Calling **libc** helpers increase the overall timing significantly
because _printf_ and friends are very general functions, which support
a huge variety of formats and as expected parsing format strings does
not come for free. Thus we need a way to save complete context of
_printf_ call and be able to parse it on demand later.

The **crlog** serves exactly that -- a complete _printf_ context is saved
in binary architecture dependant form and allow to decode it later.
