#!/usr/bin/env -S bash ../.port_include.sh
port="half-life"
version="1.0.0"
useconfigure="true"
depends=("SDL2")
workdir="xash3d-fwgs-master"
files="https://github.com/SerenityPorts/xash3d-fwgs/archive//master.tar.gz xash3d_engine.tar.gz
https://github.com/SerenityPorts/hlsdk-xash3d/archive/master.tar.gz xash3d_hldll.tar.gz"
launcher_name="Half-Life"
launcher_category="Games"
launcher_command="/home/anon/Games/halflife/xash3d"

# This one is a bit tricky to build, so I'm going a little bit off the script....
configure() {
    # Configure the shared object projects (client and game)
    cd ./hlsdk-xash3d-master
    ./waf configure -T release
    cd ../

    # Configure the engine itself...
    cd ./xash3d-fwgs-master
    ./waf configure --sdl2=../../SDL2/SDL-main-serenity/ -T release
}

build() {
    # Build the game and client
    cd ../hlsdk-xash3d-master
    ./waf build
    cd ../

    # Build the engine
    cd ./xash3d-fwgs-master
   ./waf build
    cd ../
}

install() {
    cd ./hlsdk-xash3d-master
    ./waf install --destdir=${SERENITY_INSTALL_ROOT}/home/anon/Games/halflife
    cd ../

    # Build the engine
    cd ./xash3d-fwgs-master
    ./waf install --destdir=${SERENITY_INSTALL_ROOT}/home/anon/Games/halflife/
    cd ../
}

post_install() {
    echo 'LD_LIBRARY_PATH=/home/anon/Games/halflife/' >> ${SERENITY_INSTALL_ROOT}/Base/home/anon/.shellrc
}
