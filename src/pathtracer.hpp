#pragma once
#include <GL/glew.h>

#include "scene.hpp"

#define SIZE_X 2
#define SIZE_Y 2
#define SIZE_Z 64

class PathTracer {
public:

    PathTracer();
    ~PathTracer();

    void setScene(const Scene&);
    void render();
    void display();


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

