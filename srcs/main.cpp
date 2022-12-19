#define STB_IMAGE_IMPLEMENTATION
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "application.h"


/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
int main(void) {
    Application application;

    //    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    application.Start();
    application.Run();
    application.Stop();
	return (0);
}

GLenum glCheckError()
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << std::endl;
    }
    return errorCode;
}


