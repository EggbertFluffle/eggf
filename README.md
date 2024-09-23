# eggf
This is simple file browser I'm working on as a c++ project for myself. The code is simple and easy to understand, but I thought it would just be fun to make. In order to run it, you must clone the repo and compile the source code.

## Installation
```bash
git clone https://github.com/EggbertFluffle/eggf
cd eggf
make
./main
```

## Config
Inside the source code there is a `config.def.hpp` file responsible for configuring your eggf experience. All the possible configurations are exsplained inside the file, but if you want to make your own configuration, copy the file and call it `config.hpp`. Then just uncomment the line that says `#define USER_CONFIG` and now compiling eggf will use your veyr own config file! Instructions on how to actually config the program are inside of the `config.def.hpp` file itself.

## Features

I intend to add a lot more features for fun, but as of now eggf is capable of:
* Navigating directories
* Previewing text files
* Opening files with spesified programs

In the future I hope to allow eggf to:
* Add, remove and rename files/directories
* Copy, cut and paste files/directories
* Preview images like ranger

## Contributing

I've never REALLY worked in open source or contributed to anything myself as I still feel like I have a lot to learn. But if anyone wants to contribute to eggf, feel free to email me at hdiambrosio@gmail.com. I really don't know how open source contribution works but I'd lobe to figure it out.
