#include "pathtracer.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "shaders.hpp" // Shader source strings

using glm::vec3;

const float quad_vertices[] = {-1, -1, 0,
                               -1,  1, 0,
                                1, -1, 0,
                                1,  1, 0};

GLuint compileShader(const char * source, GLuint type) {

    // Create the shaders
    GLuint shader_id = glCreateShader(type);

    GLint result = GL_FALSE;
    int info_length;

    // Compile the shader
    glShaderSource(shader_id, 1, &source, NULL);
    glCompileShader(shader_id);

    // Check the shader
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_length);

    if (info_length > 0) {
        char err_msg[info_length + 1];
        glGetShaderInfoLog(shader_id, info_length, NULL, err_msg);
        std::cout << err_msg << std::endl;
    }
    return shader_id;
}

GLuint loadComputeShader(std::string source){

    GLuint shader_id = compileShader(source.data(), GL_COMPUTE_SHADER);

    GLint result = GL_FALSE;
    int info_length;

    // Link the program
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_length);

    if (info_length > 0) {
        char err_msg[info_length + 1];
        glGetProgramInfoLog(program_id, info_length, NULL, err_msg);
        std::cout << err_msg << std::endl;
    }

    glDetachShader(program_id, shader_id);

    glDeleteShader(shader_id);

    return program_id;
}

GLuint loadGraphicsShaders(std::string vsource, std::string fsource) {

    GLuint vshader_id = compileShader(vsource.data(), GL_VERTEX_SHADER);
    GLuint fshader_id = compileShader(fsource.data(), GL_FRAGMENT_SHADER);

    GLint result = GL_FALSE;
    int info_length;

    // Link the program
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vshader_id);
    glAttachShader(program_id, fshader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_length);

    if (info_length > 0) {
        char err_msg[info_length + 1];
        glGetProgramInfoLog(program_id, info_length, NULL, err_msg);
        std::cout << err_msg << std::endl;
    }

    glDetachShader(program_id, vshader_id);
    glDetachShader(program_id, fshader_id);

    glDeleteShader(vshader_id);
    glDeleteShader(fshader_id);

    return program_id;
}

PathTracer::PathTracer(): nof_samples_(0), max_samples_(0) {

    // Load programs
    pt_program_id_ = loadComputeShader(pathtracer_comp);
    draw_program_id_ = loadGraphicsShaders(quad_vert, quad_frag);

    // Generate and set up output texture
    glGenTextures(1, &output_texture_);
    glBindTexture(GL_TEXTURE_2D, output_texture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenBuffers(1, &objectbuffer_);
    glGenBuffers(1, &materialbuffer_);
    glGenBuffers(1, &paramsbuffer_);

    glGenBuffers(1, &quadbuffer_);
    glBindBuffer(GL_ARRAY_BUFFER, quadbuffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices),
                 quad_vertices, GL_STATIC_DRAW);

}

PathTracer::~PathTracer() {

    glDeleteBuffers(1, &objectbuffer_);
    glDeleteBuffers(1, &materialbuffer_);
    glDeleteBuffers(1, &paramsbuffer_);
    glDeleteBuffers(1, &quadbuffer_);

    glDeleteTextures(1, &output_texture_);

    glDeleteProgram(pt_program_id_);
    glDeleteProgram(draw_program_id_);

}

void PathTracer::setScene(const Scene& scene) {

    int nx = scene.getFrameWidth();
    int ny = scene.getFrameHeight();
    int spp = scene.getSamplesPerPixel();
    max_samples_ = spp;

    // Resize output texture
    std::vector<float> zeros(4*nx*ny, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, nx, ny, 0, GL_RGBA, GL_FLOAT, zeros.data());
    glBindImageTexture(0, output_texture_, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    nof_workgroups_ = vec3((nx  + SIZE_X - 1) / SIZE_X,
                           (ny  + SIZE_Y - 1) / SIZE_Y,
                           (spp + SIZE_Z - 1) / SIZE_Z);

    // Upload scene objects
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectbuffer_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, scene.getObjects().size() * sizeof(gl_Intersectable),
                 scene.getObjects().data(), GL_STATIC_DRAW);

    // Upload scene materials
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, materialbuffer_);
    glBufferData(GL_SHADER_STORAGE_BUFFER, scene.getMaterials().size() * sizeof(gl_Material),
                 scene.getMaterials().data(), GL_STATIC_DRAW);

    // Upload scene parameters
    PathTracerParams params = scene.getParams();
    glBindBuffer(GL_UNIFORM_BUFFER, paramsbuffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PathTracerParams),
                 &params, GL_STATIC_DRAW);

}

bool PathTracer::shouldSample() {return nof_samples_ < max_samples_;}

void PathTracer::sample() {

    glUseProgram(pt_program_id_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, objectbuffer_);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, materialbuffer_);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, paramsbuffer_);

    glUniform1i(glGetUniformLocation(pt_program_id_, "nof_samples"),
                nof_samples_);

    glDispatchCompute(nof_workgroups_.x, nof_workgroups_.y, 1);

    if (int err = glGetError() != GL_NO_ERROR) {
        std::cerr << "GL error: " << err << std::endl;
    }

    nof_samples_++;

}

void PathTracer::draw() {

    // Make sure rendering has finished
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(draw_program_id_);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, output_texture_);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quadbuffer_);
    glVertexAttribPointer(
        0,                          // attribute tag in vertex shader
        3,                          // number of elements per vertex
        GL_FLOAT,                   // type
        GL_FALSE,                   // Not normalized
        0,                          // stride
        nullptr                     // offset
    );
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisableVertexAttribArray(0);
}
