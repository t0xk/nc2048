<p align="center">
  <img width="600" height="200" src="/res/nc2048_logo.png">
</p>

*ncurses 2048* or *nc2048* is a C implementation of the popular mobile game called '2048'. It is designed to be played
in a Terminal. The project was tested on Ubuntu 22.10 and Ubuntu for WSL2.

> **NOTE:** This project was made to help the original creator learn about ncurses and the standard C99
> library. It might not fully follow the C99 guidelines and conventions.

#### Prerequisites

* `cmake` version *3.10* or later,
* `ncruses` library, an installation guide is
  available [here](https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/).

#### Building the project

```shell
mkdir nc2048 && cd nc2048
git init
git pull https://github.com/t0xk/nc2048.git
cmake -S . -B build
cd build
make

# Run the project
./nc2048
```

#### Proposed improvements

* The `populateRandomBlock` gets inefficient when the field fills up, since it re-generates a random x and y coordinate
  for the block until it finds one that is empty. We plan to improve this in the future.
* The rendering and game logic should be executed on separate threads.

#### Screenshots

![A screenshot of the nc2048 game](/res/screenshot.png)
