#!/bin/bash

mkdir -p ./libraries
mkdir -p ./libraries/lib
mkdir -p ./libraries/include

git submodule init
git submodule update

sudo apt-get install libimgui-dev libglfw3-dev libopenal-dev libsndfile1-dev

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

path=./libraries/include/GLFW
if [ ! -e  $path ]
then
    printf '\ninstalling glfw\n'
    git clone https://github.com/glfw/glfw.git ./.tmp
    mv -f ./.tmp/include/* ./libraries/include/
    rm -rf ./.tmp

fi

path=./libraries/include/AL
if [ ! -e  $path ]
then
    printf '\ninstalling openAl\n'
    git clone https://github.com/kcat/openal-soft.git ./.tmp
    mv -f ./.tmp/include/AL ./libraries/include/
    rm -rf ./.tmp

fi

path=./libraries/include/sndfile.h
if [ ! -e  $path ]
then
    printf '\ninstalling libsndfile\n'
    git clone https://github.com/libsndfile/libsndfile.git ./.tmp
    mv -f ./.tmp/include/sndfile.h ./libraries/include/
    rm -rf ./.tmp

fi

path=~/.dep
if [  ! -e $path ]
then
    echo 'instaling lib'
    mkdir -p ~/.dep
    cd ~/.dep
    apt download libglfw3
    dpkg -x libglfw* .
    echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.dep/usr/lib/x86_64-linux-gnu/' >> ~/.bashrc
    echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/.dep/usr/lib/x86_64-linux-gnu/' >> ~/.zshrc
    cd -
fi

path=~/.dep/usr/lib/*/libglfw.so.3
if [  ! -e $path ]
then
    echo 'instaling libglfw'
    cd ~/.dep
    apt download libglfw3
    dpkg -x libglfw* .
    cd -
fi

path=./libraries/lib/glfw3lib
if [  ! -e $path ]
then
    echo 'copy libglfw'
    mkdir -p ./libraries/lib/glfw3lib
    cp ~/.dep/usr/lib/*/libglfw* ./libraries/lib/glfw3lib
fi

path=./libraries/lib/freetypelib
if [  ! -e $path ]
then
    echo 'copy libfreetype'
    mkdir -p ./libraries/lib/freetypelib
    cp ~/.dep/usr/lib/*/libfreetype* ./libraries/lib/freetypelib
fi
