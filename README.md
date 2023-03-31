# minecraft
Un projet pour tester des techniques de rendering graphique et m'entrainer au c++ 
# Chunk bit layout

Vertex pos :
|0000       |	0000000000 | 00000 | 00000 | 00000000|
|-----------|:----------:|:-----:|:-----:|:-------:|
|luminosity	|	unused     |	posZ |	posX | posY    |

vertex data :
|0000000000000|0000|0000|0000|00000000|
|:------:|:--------:|:-----:|:-----:|:-----:|
|unused  |  normal  |	textY	|	textX	|	textID|

# Gpu texture slot
Texture:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|	 |	 |	 |	 |    | |

TextureArray:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|cube texture	|   |	sun |	shadow map |     | |

CubeMap:
|0 |1 | 2 | 3 | 4 | 5| 6 | 7 | 8 |
|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|:--:|
|skybox	|	     |	 |	 |     | |
