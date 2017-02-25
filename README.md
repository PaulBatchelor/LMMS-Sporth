# Sporth for LMMS 

This is a work in progress project to get a Sporth editor implemented
as an effects processor for LMMS. At the moment, it has only been tested
to work on Linux.

## LMMS-specific features

Sporth has some additional features to better integrate with LMMS:

- 4 parametric knobs that map to p-registers 0-3 in the range 0-1
- A compile switch that allows automation of recompilation of code
- variables "inL" and "inR" for left and right input signals.
- variable "bpm" to get project BPM
- "in" ugen overloaded to get the left input signal

## Compiliation 

LMMS-Sporth uses CMake for compilation. You will need to have installed 
LMMS beforehand.

    mkdir build
    cd build
    cmake -G "Unix Makefiles" ../
    make

To install, simply copy the generated plugin libsporthedit.so into the LMMS
plugin path, which on linux is typically /usr/local/lib/lmms:

    sudo cp libsporthedit.so /usr/local/lib/lmms

When you start up LMMS, you should hopefully see an effect called "Sporth". 
