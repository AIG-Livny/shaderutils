#include "shaderutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

#include "fileutils.h"
#include "stringlib.h"

int check_result(GLuint handler, bool isLink){
    GLint status;
    if(isLink){
        glGetProgramiv(handler, GL_LINK_STATUS, &status);
    }else{
        glGetShaderiv(handler, GL_COMPILE_STATUS, &status);
    }

    if (status == GL_FALSE) {
        GLint length;

        if(isLink){
            glGetProgramiv(handler, GL_INFO_LOG_LENGTH, &length);
        }else{
            glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &length);
        }

        GLchar *info = calloc(length, sizeof(GLchar));

        int error;
        if(isLink){
            glGetProgramInfoLog(handler, length, NULL, info);
            fprintf(stderr, "link shader failed:\n%s\n", info);
            error = SU_RET_LINK_ERROR;
        }else{
            glGetShaderInfoLog(handler, length, NULL, info);
            fprintf(stderr, "compile shader failed:\n%s\n", info);
            error = SU_RET_COMPILE_ERROR;
        }
        free(info);

        return error;
    }

    return SU_RET_SHADER_OK;
}

int su_compile_program(GLuint* result, GLenum type, const char *source) {
    *result = glCreateShader(type);
    if(result == 0){
        return SU_RET_COMPILE_ERROR;
    }
    glShaderSource(*result, 1, &source, NULL);
    glCompileShader(*result);

    GLint status;
    glGetShaderiv(*result, GL_COMPILE_STATUS, &status);

    int error = check_result(*result, false);

    if (error) {
        glDeleteShader(*result);
        *result = 0;
    }
    return error;
}

int su_link_shader(GLuint* result, GLuint programs[]){
    *result = glCreateProgram();
    for(int i = 0; i < SU_MAX_PROGRAMS; i++){
        if(programs[i]){
            glAttachShader(*result, programs[i]);
        }
    }

    glLinkProgram(*result);

    int error = check_result(*result, true);

    for(int i=0; i < SU_MAX_PROGRAMS; i++){
        if ( programs[i] ) {
            glDetachShader(*result, programs[i]);
            glDeleteShader(programs[i]);
            programs[i] = 0;
        }
    }

    return error;
}

int su_load_shader(GLuint* result, const char* dirpath){
    if(fs_identify_path(dirpath) != FS_DIRECTORY) { return SU_RET_PATH_NOT_DIR; }

    int error = SU_RET_SHADER_OK;
    char vert_shader_present = 0;
    char comp_shader_present = 0;

    dir_t dir = d_init(dirpath);
    if(dir == NULL){
        error = SU_RET_PATH_NOT_DIR;
        goto exit;
    }

    GLuint shader_programs[SU_MAX_PROGRAMS] = {};
    int programs_num = 0;

    for(int fn = 0; fn < d_num_files(dir); fn++){
        char* path = d_files_full_path(dir)[fn];
        file_t file = f_init(path);
        if(file == NULL){
            continue;
        }

        #define TYPES_NUM 4
        const struct {
            const char* extension;
            GLuint type;
        }
        types_map[TYPES_NUM] =
        {
            {"vs", GL_VERTEX_SHADER},
            {"fs", GL_FRAGMENT_SHADER},
            {"gs", GL_GEOMETRY_SHADER},
            {"cs", GL_COMPUTE_SHADER},
        };

        const char* ext = f_extension(file);
        int t = 0;
        for(; t<TYPES_NUM; t++){
            if(s_cmp_alt(ext,types_map[t].extension,CASE_INSENSITIVE) == 0){
                if(t == 0){
                    vert_shader_present = 1;
                }
                if(t == 3){
                    comp_shader_present = 1;
                }
                break;
            }
        }

        if(t < TYPES_NUM){
            error = su_compile_program(&shader_programs[programs_num], types_map[t].type, f_read_file(file));
            programs_num += 1;

            if (programs_num > SU_MAX_PROGRAMS){
                error = SU_RET_TOO_MANY_SHADERS;
            }
        }

        f_free(file);
        if(error) goto exit;
    }

    if((vert_shader_present == 0) && (comp_shader_present == 0)){
        error = SU_RET_NOR_VERT_COMP_PRESENT;
        goto exit;
    }

    error = su_link_shader(result, shader_programs);

    exit:
    for(int i = 0; i < SU_MAX_PROGRAMS; i++){
        if(shader_programs[i]){
            glDeleteShader(shader_programs[i]);
        }
    }

    d_free(dir);

    return error;
}

int su_load_vert_frag(GLuint* result, const char* vertpath, const char* fragpath){
    if(fs_identify_path(vertpath) != FS_FILE) { return SU_RET_BAD_FILE; }
    if(fs_identify_path(fragpath) != FS_FILE) { return SU_RET_BAD_FILE; }

    int error = SU_RET_SHADER_OK;

    file_t vert_file = f_init(vertpath);
    file_t frag_file = f_init(fragpath);
    if((vert_file == NULL) || (frag_file == NULL)){
        error = SU_RET_BAD_FILE;
        goto exit;
    }

    GLuint shader_programs[SU_MAX_PROGRAMS] = {};

    error = su_compile_program(&shader_programs[0], GL_VERTEX_SHADER, f_read_file(vert_file));
    if(error) goto exit;
    error = su_compile_program(&shader_programs[1], GL_FRAGMENT_SHADER, f_read_file(frag_file));
    if(error) goto exit;
    error = su_link_shader(result, shader_programs);

    exit:
    f_free(vert_file);
    f_free(frag_file);

    return error;
}