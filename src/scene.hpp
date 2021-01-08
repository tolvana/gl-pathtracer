#pragma once

#include <vector>
#include <glm/glm.hpp>

struct gl_Intersectable {

    glm::vec4 a, b, c;
    int type, material_idx;

};

struct gl_Material {

    glm::vec4 albedo, illuminance;
    float diffusivity, ref_idx;

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
        std::vector<gl_Material> getMaterials() const {return materials_;}
        PathTracerParams getParams() const {return params_;}



    private:
        std::vector<gl_Intersectable> objects_;
        std::vector<gl_Material> materials_;
        PathTracerParams params_;

};
