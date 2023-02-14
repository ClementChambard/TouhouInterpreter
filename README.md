# Touhou file interpreter

This is an interpreter for most touhou games' files.
Starting from touhou 13, it can open
- Ecl files (enemy movement and attack patterns)
- Std files (background scripts)
- Anm files (animation scripts)

for a good documentation on these file formats, you can look at <a href="https://exphp.github.io/thpages/"> ExpHP </a> and <a href="https://priw8.github.io/"> primw8 </a>'s website.

I will not supply any game file, please buy the games to support the original creator.

To obtain these files from the games, I recommend <a href="https://github.com/thpatch/thtk"> thtk </a>.

<img src="./screenshot.png">

I created a json file of hardcoded value for TH17. These values can change depending on the game. You can create your own version if you want to add bullet types for example.
You can find more of these in <a href='https://github.com/ClementChambard/TouhouHardcodedData'>this repo</a>.

## Compile

This project depends on <a href="https://github.com/ClementChambard/NSEngine">NSEngine</a>, which is my own game engine.
This project uses the 'new architecture' branch of the engine.
I use simlinks with relative paths to link to the engine's folder with this folder, but if you only want to compile the project once, you can replace them with their respective folders.

*Currently, this can't be easily compiled on Windows or MacOS. If you want you can modify the CMakeLists.txt to fit your needs*

* You will need :
  - make
  - cmake
  - a c++ compiler

* Make sure you have the required libraries :
  - OpenGL and glew
  - glm
  - SDL2

* Clone the repo
 `git clone https://github.com/ClementChambard/TouhouInterpreter.git`

* Create a build directory
 `mkdir build && cd build`

* Run cmake to configure the project
 `cmake ..`

* Run make to compile
 `make`

* Copy or link the assets directory in the build directory
 `ln -s ../assets`

*I need to learn more about cmake and find a way to remove these parts*

* The 'hardcoded.json' file should be in the execution path
 `cp ../hardcoded.json .`

* Run the executable
 `./ecl <name of ecl file> [name of entry point]`


## Contact

for further questions, contact me at clement.chambard@universite-paris-saclay.fr
