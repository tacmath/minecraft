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
    echo -n ''
else
    echo 'installing lib glfw3'
    brew install glfw3
fi
mkdir -p ./libraries/lib/glfw3lib
cp -f $path ./libraries/lib/glfw3lib
cp -fr ~/.brew/Cellar/glfw/*/include/ ./libraries/include/

path=./libraries/include/KHR/khrplatform.h
if [ -e $path ]
then
    echo -n ''
else
    echo 'installing khrplatform'
    mkdir -p ./libraries/include/KHR
    curl -k https://glad.dav1d.de/generated/tmpnwwssc10glad/include/KHR/khrplatform.h > ./libraries/include/KHR/khrplatform.h

fi



path=./libraries/include/stb_image.h
if [ -e $path ]
then
    echo -n ''
else
    echo 'installing stb_image.h'
    curl https://raw.githubusercontent.com/nothings/stb/master/stb_image.h > ./libraries/include/stb_image.h

fi

path=./libraries/include/glad/glad.h
if [ -e $path ]
then
    echo -n ''
else
    echo 'installing glad.h'
    mkdir -p ./libraries/include/glad
    curl https://glad.dav1d.de/generated/tmpnwwssc10glad/include/glad/glad.h > ./libraries/include/glad/glad.h

fi

path=./glad.c
if [ -e $path ]
then
    echo -n ''
else
    echo 'installing glad.c'
    curl https://glad.dav1d.de/generated/tmpnwwssc10glad/src/glad.c > ./glad.c

fi


path=./libraries/include/glm
if [ -e $path ]
then
    echo -n ''
else
    echo 'installing glm'
    git clone https://github.com/g-truc/glm.git ./libraries/include/.tmp
    mv -f ./libraries/include/.tmp/glm ./libraries/include/
    rm -rf ./libraries/include/.tmp

fi


#freetype
echo "installer :)"
