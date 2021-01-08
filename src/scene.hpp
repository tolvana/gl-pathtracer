#pragma once

#include <vector>
#include <glm/glm.hpp>

struct gl_Intersectable {

    glm::vec4 a, b, c;
    int type, material_idx;

    int pad0, pad1; // padding for std430 layout compatibility

};

struct gl_Material {

    glm::vec4 color;
    float fuzz, ref_idx;
    int type;

    int pad0; // padding for std430 layout compatibility

};

struct PathTracerParams {
    int nx, ny, spp; // output resolution, samples per pixel
    int pad0; // padding for std140 layout compatibility
    glm::vec4 camPos, camDir, camUp, camRight; // camera position and orientation
    float aperture, focal_distance;

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
        std::vector<gl_Material> getMaterials() const {return materials_;}
        PathTracerParams getParams() const {return params_;}



    private:
        std::vector<gl_Intersectable> objects_;
        std::vector<gl_Material> materials_;
        PathTracerParams params_;

};
