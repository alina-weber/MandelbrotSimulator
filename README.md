# Build instructions

So far I am building this on Ubuntu 22.04 and 24.04, so the used libraries might not be available on other operating sytems.

Apart from the standard library I use a library for the visualization which is `libsfml-dev`. It can be installed on Ubuntu via:
`$ sudo apt install libsfml-dev`

If you do not have this library you can also use the visualization in the terminal. Each two spaces will be one pixel of the graphic.

# TODO

- Implement calculation and visualization fully on GPU.
- Implement second GPU implementation for AMD GPUs using ROCm
- Improve terminal visualization. Not sure why the curser adjustments are buggy
- Tidy up project
- Implement more fractals
- Search for more complex projects for GPU calculation


