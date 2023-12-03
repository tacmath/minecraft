# minecraft
Un projet pour tester des techniques de rendering graphique et m'entrainer au c++

# To Run

make install
make
./vox

Pour lancer sur Visual studio installer vcpkg et lancer la commande :
./vcpkg install opengl openal-soft glm libsndfile glfw3 imgui imgui[glfw-binding] imgui[opengl3-binding]
et cloner le ficher https://github.com/nothings/stb/blob/master/stb_image.h dans libraries\include

# Dependency
OpenGl 4.6
Glfw3.3
Glad
Glm
stb_image
ImGui
OpenAl
SndFile


# Chunk bit layout

Vertex pos :
|0000       |	0000000000 | 00000 | 00000 | 00000000|
|-----------|:----------:|:-----:|:-----:|:-------:|
|luminosity	|	unused     |	posZ |	posX | posY    |

Vertex data :
|0000000000000|0000|0000|0000|00000000|
|:------:|:--------:|:-----:|:-----:|:-----:|
|unused  |  normal  |	textY	|	textX	|	textID|

# Gpu texture slot
Texture:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|	 |cross	 |	inventory |	 |    | |

TextureArray:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|cube texture	|   |	sun |	shadow map |     | |

CubeMap:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|skybox	|	     |	 |	 |     | |
