#!/usr/bin/env -S bash ../.port_include.sh
port=half-life
version=0.1
workdir=xash3d
useconfigure=true
files="https://github.com/Quaker762/hl_test/raw/master/xash3d.tar.gz xash3d.tar.gz"
makeopts=""
configopts="-DCMAKE_TOOLCHAIN_FILE=${SERENITY_SOURCE_DIR}/Toolchain/CMake/CMakeToolchain.txt -DMAINUI_USE_CUSTOM_FONT_RENDER=OFF -DHL_SDK_DIR=./hlsdk -DXASH_SDL=yes -DXASH_VGUI=no"
depends=SDL2
launcher_name=Half-Life
launcher_category=Games
launcher_command=halflife

configure() {
    run cmake $configopts
}

install() {
    run make
}

# FIXME: Uhh, the things in this directory are not supposed to run on the host, why add this to $PATH?!
export PATH="${SERENITY_BUILD_DIR}/Root/usr/bin:${PATH}"
