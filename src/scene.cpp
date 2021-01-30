#include "scene.hpp"

Scene::Scene() {
    gl_Material grey_lambertian = {
                       .color = glm::vec4(0.6),
                       .type = 0
    };
    gl_Material red_lambertian = {
                       .color = glm::vec4(1.0, 0.0, 0.0, 0.0),
                       .type = 0
    };

    gl_Material green_lambertian = {
                       .color = glm::vec4(0.0, 1.0, 0.0, 0.0),
                       .type = 0
    };

    gl_Material blue_lambertian = {
                       .color = glm::vec4(0.0, 0.0, 1.0, 0.0),
                       .type = 0
    };

    gl_Material copper = {
                       .color = glm::vec4(0.8, 0.5, 0.3, 0.0),
                       .fuzz = 0.5,
                       .type = 1
    };

    gl_Material glass = {
                       .color = glm::vec4(1.0),
                       .fuzz = 0.0,
                       .ref_idx = 1.5,
                       .type = 2
    };

    gl_Material plasma = {
                       .color = glm::vec4(5.0),
                       .type = 3
    };

    gl_Material mirror = {
                       .color = glm::vec4(0.95, 0.95, 0.95, 0.0),
                       .fuzz = 0.0,
                       .type = 1
    };

    materials_.push_back(grey_lambertian);
    materials_.push_back(red_lambertian);
    materials_.push_back(green_lambertian);
    materials_.push_back(blue_lambertian);
    materials_.push_back(copper);
    materials_.push_back(glass);
    materials_.push_back(plasma);
    materials_.push_back(mirror);

    /*gl_Intersectable huge_lambertian_sphere = {
                               .a = glm::vec4(0, 0, -5001, 0),
                               .b = glm::vec4(4997, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0, // sphere
                               .material_idx = 0 // grey lambertian
    };*/

    gl_Intersectable small_lambertian_sphere = {
                               .a = glm::vec4(4, 0, 0, 0),
                               .b = glm::vec4(1.0, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0, // sphere
                               .material_idx = 1 // red lambertian
    };

    gl_Intersectable copper_sphere = {
                               .a = glm::vec4(2, 2, 1, 0),
                               .b = glm::vec4(0.9, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0, // sphere
                               .material_idx = 4 // copper
    };


    /*gl_Intersectable glass_sphere = {
                               .a = glm::vec4(0, -0., -0, 0),
                               .b = glm::vec4(1.0, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0, // sphere
                               .material_idx = 5 // glass
    };*/

    gl_Intersectable mirror_sphere = {
                               .a = glm::vec4(2, -1, 0, 0),
                               .b = glm::vec4(0.5, 0, 0, 0),
                               .c = glm::vec4(0.0, 0, 0, 1),
                               .type = 0, // sphere
                               .material_idx = 7 // mirror
    };

    gl_Intersectable bottom_0 = {
                               .a = glm::vec4(0,  3, -3, 0),
                               .b = glm::vec4(0, -3, -3, 0),
                               .c = glm::vec4(5,  3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };

    gl_Intersectable bottom_1 = {
                               .a = glm::vec4(5,  3, -3, 0),
                               .b = glm::vec4(5, -3, -3, 0),
                               .c = glm::vec4(0, -3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };

    gl_Intersectable top_0 = {
                               .a = glm::vec4(0,  3, 3, 0),
                               .b = glm::vec4(0, -3, 3, 0),
                               .c = glm::vec4(5,  3, 3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };

    gl_Intersectable top_1 = {
                               .a = glm::vec4(5,  3, 3, 0),
                               .b = glm::vec4(5, -3, 3, 0),
                               .c = glm::vec4(0, -3, 3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };

    gl_Intersectable left_0 = {
                               .a = glm::vec4(0,  3, -3, 0),
                               .b = glm::vec4(0,  3,  3, 0),
                               .c = glm::vec4(5,  3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 2 // green lambertian

    };

    gl_Intersectable left_1 = {
                               .a = glm::vec4(5,  3,  3, 0),
                               .b = glm::vec4(0,  3,  3, 0),
                               .c = glm::vec4(5,  3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 2 // green lambertian

    };

    gl_Intersectable right_0 = {
                               .a = glm::vec4(0, -3, -3, 0),
                               .b = glm::vec4(0, -3,  3, 0),
                               .c = glm::vec4(5, -3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 3 // blue lambertian

    };

    gl_Intersectable right_1 = {
                               .a = glm::vec4(5, -3,  3, 0),
                               .b = glm::vec4(0, -3,  3, 0),
                               .c = glm::vec4(5, -3, -3, 1),
                               .type = 1, // triangle
                               .material_idx = 3 // blue lambertian

    };

    gl_Intersectable back_0 = {
                               .a = glm::vec4(5, -3, -3, 0),
                               .b = glm::vec4(5, -3,  3, 0),
                               .c = glm::vec4(5,  3,  3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };

    gl_Intersectable back_1 = {
                               .a = glm::vec4(5, -3, -3, 0),
                               .b = glm::vec4(5,  3, -3, 0),
                               .c = glm::vec4(5,  3,  3, 1),
                               .type = 1, // triangle
                               .material_idx = 0 // grey lambertian

    };
    gl_Intersectable light_0 = {
                               .a = glm::vec4(1.5,  1.5, 2.99, 0),
                               .b = glm::vec4(1.5, -1.5, 2.99, 0),
                               .c = glm::vec4(3.5,  1.5, 2.99, 1),
                               .type = 1, // triangle
                               .material_idx = 6 // plasma

    };

    gl_Intersectable light_1 = {
                               .a = glm::vec4(3.5,  1.5, 2.99, 0),
                               .b = glm::vec4(3.5, -1.5, 2.99, 0),
                               .c = glm::vec4(1.5, -1.5, 2.99, 1),
                               .type = 1, // triangle
                               .material_idx = 6 // plasma

    };

    objects_.push_back(small_lambertian_sphere);
    //objects_.push_back(huge_lambertian_sphere);
    objects_.push_back(copper_sphere);
    //objects_.push_back(glass_sphere);
    objects_.push_back(mirror_sphere);
    objects_.push_back(bottom_0);
    objects_.push_back(bottom_1);
    objects_.push_back(top_0);
    objects_.push_back(top_1);
    objects_.push_back(left_0);
    objects_.push_back(left_1);
    objects_.push_back(right_0);
    objects_.push_back(right_1);
    objects_.push_back(back_0);
    objects_.push_back(back_1);
    objects_.push_back(light_0);
    objects_.push_back(light_1);

    params_ = {.nx = 1920,
               .ny = 1080,
               .spp = 2048,
               .camPos = glm::vec4(-5.0, 0.0, 0.0, 0),
               .camDir = glm::vec4(1.0, 0.0, 0.0, 0),
               .camUp = 2 * (float)std::tan(75 * M_PI/360) / 1368 * glm::vec4(.0, .0, 1.0, 0),
               .camRight = 2 * (float)std::tan(75 * M_PI/360) / 1368 * glm::vec4(0, -1, 0, 0),
               .aperture = 0.0,
               .focal_distance = 6.0,
               .recursion_depth = 10,
               .tmin = 1e-3f,
               .tmax = 1e8f,
               .nof_objects = (int)objects_.size(),
               .seed = rand()
              };
}
