#include "scene.hpp"

Scene::Scene() {

    gl_Intersectable sphere = {
                               .a = glm::vec4(5, 0, 0, 0),
                               .b = glm::vec4(0.5, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .albedo = glm::vec4(0.5, 0.5, 0.5, 1),
                               .refIdx = 0.5,
                               .diffusivity = 1.0,
                               .type = 0,
    };
    objects_.push_back(sphere);

    params_ = {.nx = 1368,
               .ny = 768,
               .spp = 8,
               .camPos = glm::vec4(0.0, 0.0, 0.0, 0),
               .camDir = glm::vec4(1.0, 0.0, 0.0, 0),
               .camUp = 2 * (float)std::tan(90 * M_PI/360) / 1368 * glm::vec4(.0, .0, 1.0, 0),
               .camRight = 2 * (float)std::tan(90 * M_PI/360) / 1368 * glm::vec4(0, -1, 0, 0),
               .recursion_depth = 10,
               .tmin = 1e0f,
               .tmax = 1e8f,
               .nof_objects = 1,
               .seed = 1
              };
}
