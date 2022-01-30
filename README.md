lsystem visualizer [![Build&Release](https://github.com/Tresonic/lsystem/actions/workflows/build.yml/badge.svg)](https://github.com/Tresonic/lsystem/actions/workflows/build.yml)
==========================
![screenshot](https://user-images.githubusercontent.com/37078118/151704332-a0019ede-c7d1-45c9-8507-5fe5889a1fe5.png)

Building
--------

```sh
mkdir build
cd build
cmake <path-to-cloned-repo>
cmake --build .
./src/lsystem # or .\src\Debug\lsystem.exe if you're on Windows
```

Dependencies
---

CMake >= 3.12

Also, if you're on Linux, you'll need to install dependencies for building SFML
specified
[here](https://www.sfml-dev.org/tutorials/2.5/compile-with-cmake.php#installing-dependencies).

Ubuntu:
```sh
sudo apt install libxrandr-dev libxcursor-dev libudev-dev libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libegl1-mesa-dev
```


sfml-imgui-template by [Elias Daler](https://github.com/eliasdaler) from [here](https://github.com/eliasdaler/imgui-sfml-fetchcontent/tree/dev)
