#!/bin/bash

mkdir -p ./libraries
mkdir -p ./libraries/lib
mkdir -p ./libraries/include


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

path=./libraries/include/GLFW
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing glfw'
    git clone https://github.com/glfw/glfw.git ./.tmp
    mv -f ./.tmp/include/* ./libraries/include/
    rm -rf ./.tmp

fi


path=./libraries/include/freetype
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'installing freetype'
    git clone https://github.com/freetype/freetype.git ./.tmp
    mv -f ./.tmp/include/* ./libraries/include/
    rm -rf ./.tmp

fi

