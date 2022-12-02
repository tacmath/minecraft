#!/bin/bash

mkdir -p ./libraries
mkdir -p ./libraries/lib
mkdir -p ./libraries/include


var=$(which brew)

if [ ${#var} -eq 0 ]
then
curl -fsSL https://rawgit.com/kube/42homebrew/master/install.sh | zsh
fi



path=~/.brew/Cellar/glfw/*/lib/libglfw.dylib
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing lib glfw3'
    brew install glfw3
fi
mkdir -p ./libraries/lib/glfw3lib
cp -f $path ./libraries/lib/glfw3lib
cp -fr ~/.brew/Cellar/glfw/*/include/ ./libraries/include/

path=~/.brew/Cellar/freetype/*/lib/libfreetype.dylib
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing lib freetype'
    brew install freetype
fi
mkdir -p ./libraries/lib/freetypelib
cp -f $path ./libraries/lib/freetypelib/
cp -fr ~/.brew/Cellar/freetype/*/include/freetype2/ ./libraries/include/

path=./libraries/include/glad/khrplatform.h
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing khrplatform'
    mkdir -p ./libraries/include/glad
    curl -k https://raw.githubusercontent.com/matheme42/glad/master/khrplatform.h > ./libraries/include/glad/khrplatform.h

fi



path=./libraries/include/stb_image.h
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing stb_image.h'
    curl https://raw.githubusercontent.com/nothings/stb/master/stb_image.h > ./libraries/include/stb_image.h

fi

path=./libraries/include/glad/glad.h
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing glad.h'
    mkdir -p ./libraries/include/glad
    curl https://raw.githubusercontent.com/matheme42/glad/master/glad.h > ./libraries/include/glad/glad.h

fi

path=./glad.c
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing glad.c'
    curl https://raw.githubusercontent.com/matheme42/glad/master/glad.c > ./glad.c

fi


path=./libraries/include/glm
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing glm'
    git clone https://github.com/g-truc/glm.git ./libraries/include/.tmp
    mv -f ./libraries/include/.tmp/glm ./libraries/include/
    rm -rf ./libraries/include/.tmp

fi