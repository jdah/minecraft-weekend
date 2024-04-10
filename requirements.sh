#!/bin/sh

#requirement for Debain and Ubuntu only

echo "installing"

# all this should be in the other distro repo i don't know
# this command will install everything it need to comepile minecraft in C

sudo apt-get install git fakeroot build-essential ncurses-dev xz-utils libssl-dev bc flex libelf-dev bison

sudo apt-get install cmake
sudo apt-get install libxi-dev

sudo apt-get install libxcursor-dev
sudo apt-get install libxinerama-dev

sudo apt-get install libxrandr-dev

make -j3

echo "Done"
