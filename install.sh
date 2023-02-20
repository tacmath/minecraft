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

path=~/.dep
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'instaling lib'
    mkdir -p ~/.dep
    cd ~/.dep
    apt download libglfw3
    apt download libfreetype6
    dpkg -x libglfw* .
    dpkg -x libfreetype* .
    echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.dep/glfw3/usr/lib/x86_64-linux-gnu/' >> .bashrc
    cd -
fi

path=./libraries/lib/glfw3lib
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'copy libglfw'
    mkdir -p ./libraries/lib/glfw3lib
    cp ~/.dep/usr/lib/*/libglfw* ./libraries/lib/glfw3lib
fi

path=./libraries/lib/freetypelib
if [ -e $path ]
then
    echo '' > /dev/null
else
    echo 'copy libfreetype'
    mkdir -p ./libraries/lib/freetypelib
    cp ~/.dep/usr/lib/*/libfreetype* ./libraries/lib/freetypelib
fi
