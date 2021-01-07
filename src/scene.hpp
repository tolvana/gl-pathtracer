#pragma once

#include <vector>
#include <glm/glm.hpp>

struct gl_Intersectable {

    glm::vec4 a, b, c;
    glm::vec4 albedo;
    float refIdx, diffusivity;
    int type;

};

struct PathTracerParams {
    int nx, ny, spp; // output resolution, samples per pixel
    int pad0; // padding to conform with std140 layout
    glm::vec4 camPos, camDir, camUp, camRight; // camera position and orientation

    int recursion_depth;
    float tmin, tmax;

    int nof_objects;

    int seed;
};

class Scene {
    public:
        Scene();
        ~Scene() = default;

        int getFrameWidth() const {return params_.nx;}
        int getFrameHeight() const {return params_.ny;}
        int getSamplesPerPixel() const {return params_.spp;}
        std::vector<gl_Intersectable> getObjects() const {return objects_;}
        PathTracerParams getParams() const {return params_;}



    private:
        std::vector<gl_Intersectable> objects_;
        PathTracerParams params_;

};