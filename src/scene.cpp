#include "scene.hpp"

Scene::Scene() {
    gl_Material mat = {
                       .albedo = glm::vec4(0.5),
                       .illuminance = glm::vec4(0.0),
                       .diffusivity = 1.0,
                       .ref_idx = 1.0
    };

    materials_.push_back(mat);

    gl_Intersectable small_lambertian_sphere = {
                               .a = glm::vec4(4, 0, 0, 0),
                               .b = glm::vec4(1.0, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0,
                               .material_idx = 0 // refers to gl_Material mat
    };

    gl_Intersectable huge_lambertian_sphere = {
                               .a = glm::vec4(0, 0, -5001, 0),
                               .b = glm::vec4(5000, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0,
                               .material_idx = 0 // refers to gl_Material mat
    };

    objects_.push_back(small_lambertian_sphere);
    objects_.push_back(huge_lambertian_sphere);

    params_ = {.nx = 1920,
               .ny = 1080,
               .spp = 256,
               .camPos = glm::vec4(0.0, 0.0, 0.0, 0),
               .camDir = glm::vec4(1.0, 0.0, 0.0, 0),
               .camUp = 2 * (float)std::tan(90 * M_PI/360) / 1368 * glm::vec4(.0, .0, 1.0, 0),
               .camRight = 2 * (float)std::tan(90 * M_PI/360) / 1368 * glm::vec4(0, -1, 0, 0),
               .recursion_depth = 10,
               .tmin = 1e0f,
               .tmax = 1e8f,
               .nof_objects = (int)objects_.size(),
               .seed = 1
              };
}
