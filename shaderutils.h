#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <GL/glew.h>

#define SU_RET_SHADER_OK 0
#define SU_RET_PATH_NOT_DIR 1
#define SU_RET_TOO_MANY_SHADERS 2
#define SU_RET_COMPILE_ERROR 3
#define SU_RET_LINK_ERROR 4
#define SU_RET_BAD_FILE 5

// Must be vertex or compute shader
#define SU_RET_NOR_VERT_COMP_PRESENT 6

#define SU_MAX_PROGRAMS 4

/*
    Compile shader

    @param result - put result by pointer
    @param type - OpenGL shader type
    @param source - source code text
    @return error code
*/
int su_compile_program(GLuint* result, GLenum type, const char* source);

/*
    Link array shader programs

    @param result - put result by pointer
    @param programs[SU_MAX_PROGRAMS] - array of program handlers
    @return error code
*/
int su_link_shader(GLuint* result, GLuint programs[]);

/*
    Load shaders from directory. Auto search .vs .fs .gs .cs extensions
    in directory.

    @param result - put result by pointer
    @param dirpath - path to directory
    @return error code
*/
int su_load_shader(GLuint* result, const char *dirpath);

/*
    Load vertex and fragment shader by exact path both

    @param result - put result by pointer
    @param vertpath - path to vertex shader
    @param fragpath - path to fragment shader
*/
int su_load_vert_frag(GLuint* result, const char* vertpath, const char* fragpath);

#ifdef __cplusplus
} // extern "C"
#endif

#endif