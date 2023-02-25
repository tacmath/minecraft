#define STB_IMAGE_IMPLEMENTATION
#include <ft2build.h>
#include FT_FREETYPE_H  
#include "application.h"


#define DEBUG_MODE 1

#if DEBUG_MODE == 1
#define ON_DEBUG(x) x
#else
#define ON_DEBUG(x)
#endif // DEBUG_MODE == 1



std::string glStringEnum(GLenum glenum)
{
    std::string enumString = "NONE";

    switch (glenum)
    {
    case GL_INVALID_ENUM:                   enumString = "INVALID_ENUM"; break;
    case GL_INVALID_VALUE:                  enumString = "INVALID_VALUE"; break;
    case GL_INVALID_OPERATION:              enumString = "INVALID_OPERATION"; break;
    case GL_OUT_OF_MEMORY:                  enumString = "OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:  enumString = "INVALID_FRAMEBUFFER_OPERATION"; break;
    
    case GL_DEBUG_SEVERITY_MEDIUM:          enumString = "GL_DEBUG_SEVERITY_MEDIUM"; break;
    case GL_DEBUG_SEVERITY_HIGH:            enumString = "GL_DEBUG_SEVERITY_HIGH"; break;
    case GL_DEBUG_SEVERITY_LOW:             enumString = "GL_DEBUG_SEVERITY_LOW"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:    enumString = "GL_DEBUG_SEVERITY_NOTIFICATION"; break;
    
    case GL_DEBUG_SOURCE_API:               enumString = "GL_DEBUG_SOURCE_API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     enumString = "GL_DEBUG_SOURCE_WINDOW_SYSTEM"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:   enumString = "GL_DEBUG_SOURCE_SHADER_COMPILER"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:       enumString = "GL_DEBUG_SOURCE_THIRD_PARTY"; break;
    case GL_DEBUG_SOURCE_APPLICATION:       enumString = "GL_DEBUG_SOURCE_APPLICATION"; break;
    case GL_DEBUG_SOURCE_OTHER:             enumString = "GL_DEBUG_SOURCE_OTHER"; break;

    case GL_DEBUG_TYPE_ERROR:               enumString = "GL_DEBUG_TYPE_ERROR"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: enumString = "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  enumString = "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         enumString = "GL_DEBUG_TYPE_PORTABILITY"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         enumString = "GL_DEBUG_TYPE_PERFORMANCE"; break;
    case GL_DEBUG_TYPE_MARKER:              enumString = "GL_DEBUG_TYPE_MARKER"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          enumString = "GL_DEBUG_TYPE_PUSH_GROUP"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           enumString = "GL_DEBUG_TYPE_POP_GROUP"; break;
    case GL_DEBUG_TYPE_OTHER:               enumString = "GL_DEBUG_TYPE_OTHER"; break;
    }
    return enumString;
}



void Debug(void) {
    std::cout << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
        if (severity == GL_DEBUG_SEVERITY_MEDIUM ||  severity == GL_DEBUG_SEVERITY_HIGH)
            std::cerr << "Opengl : " << " severity : " << glStringEnum(severity) << " type : " << glStringEnum(type) << " source : " << glStringEnum(source) << std::endl << "message : " << message << std::endl << std::endl;
        }, 0);
}

/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/
int main(void) {
    Application application;

    //    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    ON_DEBUG(Debug();)

    application.Start();
    application.Run();
    application.Stop();
	return (0);
}

