#pragma once
#include <GL/glew.h>

#include "scene.hpp"

#define SIZE_X 16
#define SIZE_Y 16
#define SIZE_Z 1

class PathTracer {
public:

    PathTracer();
    ~PathTracer();

    void setScene(const Scene&);
    bool shouldSample();
    void sample();
    void draw();
    int nof_samples_;
    int max_samples_;

private:



    glm::vec3 nof_workgroups_;

    GLuint pt_program_id_;
    GLuint draw_program_id_;

    GLuint output_texture_;

    GLuint objectbuffer_;
    GLuint materialbuffer_;

    GLuint paramsbuffer_;
    GLuint quadbuffer_;

};

