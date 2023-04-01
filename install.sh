#!/bin/bash

mkdir -p ./libraries
mkdir -p ./libraries/lib
mkdir -p ./libraries/include

git submodule init
git submodule update

#sudo apt-get install libimgui-dev libglfw3-dev libopenal-dev libsndfile1-dev

path=./libraries/include/glad/khrplatform.h
if [  ! -e $path ]
then
    printf '\ninstalling khrplatform\n'
    mkdir -p ./libraries/include/glad
    curl -k https://raw.githubusercontent.com/matheme42/glad/master/khrplatform.h > ./libraries/include/glad/khrplatform.h

fi

path=./libraries/include/stb_image.h
if [  ! -e $path ]
then
    printf '\ninstalling stb_image.h\n'
    curl https://raw.githubusercontent.com/nothings/stb/master/stb_image.h > ./libraries/include/stb_image.h

fi

path=./libraries/include/glad/glad.h
if [  ! -e $path ]
then
    printf '\ninstalling glad.h\n'
    mkdir -p ./libraries/include/glad
    curl https://raw.githubusercontent.com/matheme42/glad/master/glad.h > ./libraries/include/glad/glad.h

fi

path=./glad.c
if [  ! -e $path ]
then
    printf '\ninstalling glad.c\n'
    curl https://raw.githubusercontent.com/matheme42/glad/master/glad.c > ./glad.c

fi


path=./libraries/include/glm
if [ ! -e $path ]
then
    printf '\ninstalling glm\n'
    git clone https://github.com/g-truc/glm.git ./libraries/include/.tmp
    mv -f ./libraries/include/.tmp/glm ./libraries/include/
    rm -rf ./libraries/include/.tmp

fi

DEPENDENCY_FOLDER=~/.dep
LIB_FOLDER=$DEPENDENCY_FOLDER/usr/lib/x86_64-linux-gnu

echo DEPENDENCY_FOLDER = "$DEPENDENCY_FOLDER"
echo LIB_FOLDER = "$LIB_FOLDER"

install_lib()
{
    pkg_name=$1
    printf '\ninstalling %s\n' "$pkg_name"
    cd $DEPENDENCY_FOLDER
    for pkg in `apt-cache pkgnames "$pkg_name"`
    do 
        apt download $pkg
        dpkg -x "$pkg"*.deb $DEPENDENCY_FOLDER
        rm $pkg*.deb
    done
    cd -
    pkg-config --define-prefix "$DEPENDENCY_FOLDER/usr" $LIB_FOLDER/*
}

install_libs()
{
    uptade_lib=false
    for lib_name in "$@"
    do
        path=$LIB_FOLDER/$lib_name.so
        if [  ! -e $path ]; then
            install_lib $lib_name
            uptade_lib=true
        fi

    done
    if [ $uptade_lib = true ]; then
        cp -rf $DEPENDENCY_FOLDER/usr/include libraries/
    fi
}

if [  ! -e $DEPENDENCY_FOLDER ]
then
    printf "\ninit  $DEPENDENCY_FOLDER\n"
    mkdir $DEPENDENCY_FOLDER
    printf 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:%s/pkgconfig\n' "$LIB_FOLDER" >> ~/.bashrc
    printf 'export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:%s/pkgconfig\n' "$LIB_FOLDER" >> ~/.zshrc
fi


install_libs libglfw libsndfile libopenal