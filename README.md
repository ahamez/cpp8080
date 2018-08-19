# cpp8080
A 8080 emulator written in C++17 started after reading http://www.emulator101.com.
The idea was to see what C++17 could bring to the writing of an emulator.
The answer is: not so much. C++11/14 was helpful to [generate the jump table](https://github.com/ahamez/cpp8080/blob/bf9a7a05708fe299685fba1b94e8f5bb06e05962/cpp8080/meta/instruction.hh#L29),
but otherwise, I didn't find a way to use effectively use C++17 new features.

## Acknowledgements
I could not have written `cpp8080` without those ressources:
- http://www.emulator101.com
- https://github.com/superzazu/invaders

## Architecture
`cpp8080` is a C++17 library. The repository contains two examples built on this emulator:
- Space Invaders (without sounds)
- a test that exercices the emulated CPU

## Dependencies
- A C++17 compiler
- SDL2

## Build

### Xcode
Requires [homebrew](https://brew.sh).
```
brew install sdl2
xcodebuild
```

### Cmake
Requires [conan](https://conan.io).
```
mkdir build && cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install ..
cmake ..
make -j
```
