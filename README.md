# Build instructions

So far I am building this on Ubuntu 22.04 and 24.04, so the used libraries might not be available on other operating sytems.

Apart from the standard library I use a library for the visualization which is `libsfml-dev`. It can be installed on Ubuntu via:
`$ sudo apt install libsfml-dev`

I currently compile it with GCC/G++ version 15, but older versions should work as well.
The compilation command is just:
`g++ main.cpp -lsfml-graphics -lsfml-window -lsfml-system`

When the project and compilation gets more complex I will also add a cmake file etc.
