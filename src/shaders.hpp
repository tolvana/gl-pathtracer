const char pt_cshader[] = R"(
#version 460 core
// Thread block dimensions
#define SIZE_X 8
#define SIZE_Y 8
#define SIZE_Z 8

// Intersectable types
#define SPHERE_TYPE 0
#define TRIANGLE_TYPE 1

#define PHI 1.61803398874989484820459

layout (local_size_x = SIZE_X, local_size_y = SIZE_Y, local_size_z = SIZE_Z) in;

layout (rgba32f, binding = 0) uniform image2D output_image;

struct Material {

    vec3 albedo;
    float refIdx, diffusivity;

};

struct Intersectable {

    int type;
    Material material;

    // Whatever data is required to specify the intersectable.
    // Interpretation of data depends on the type parameter,
    // see intersect method for details.
    vec3 a, b, c;

};

struct Ray {

    vec3 origin, direction;

};

struct IntersectResult {

    bool hit;
    float t;
    vec3 normal;
    vec3 point;
    Material material;

};


// input scene specifications
layout (std140, binding=1) buffer scene {

	Intersectable objects[];

};

// parameters
layout(std140, binding=2) uniform params {

    int nx, ny, spp; // output resolution, samples per pixel
    vec3 camPos, camDir, camUp; // camera position and orientation
    float h_fov; // horizontal field of view

    int recursion_depth;
    float tmin, tmax;

    int nof_objects;

    int seed;

};


IntersectResult intersect(Ray r, Intersectable obj, float tmin, float tmax) {

    IntersectResult res;
    res.hit = false;

    if (obj.type == SPHERE_TYPE) { // Sphere intersection
        vec3 center = obj.a;
        float radius = obj.b.x;

    } else if (obj.type == TRIANGLE_TYPE) { // Triangle intersection

    }

    return res;

}

Ray scatter(Ray incoming, IntersectResult intersect) {
    Ray ray;
    ray.origin = vec3(0.0);
    ray.direction = vec3(0.0);
    return ray;
}

vec3 shade(Ray incoming, IntersectResult intersect) {
    return vec3(0.8, 0.8, 1.0);
}

vec3 getBackgroundColor(Ray ray) {
    return vec3(0.8, 0.8, 1.0);
}

Ray generateRay(uint i, uint j) {
    Ray ray;
    ray.origin = vec3(0.0);
    ray.direction = vec3(0.0);
    return ray;
}
// from https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl

float rand(in vec2 xy, in float seed){
    return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

// shared array to store the result of each sampler thread of each pixel.
shared vec3 samples[SIZE_X * SIZE_Y * SIZE_Z];

void main() {

    uint x = gl_LocalInvocationID.x;
    uint y = gl_LocalInvocationID.y;
    uint z = gl_LocalInvocationID.z;


    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if (distance(coord, ivec2(nx, ny)/2) < recursion_depth * spp) imageStore(output_image, coord, vec4(1.0, 0.5, 0.5, 1.0));
    else imageStore(output_image, coord, vec4(camDir, 1.0));
    if (gl_GlobalInvocationID.x < nx && gl_GlobalInvocationID.y < ny && gl_GlobalInvocationID.z < spp) {

        uint idx = x * SIZE_Y * SIZE_Z + y * SIZE_Z + z; // thread index

        //thread coordinates for PRNG
        vec2 rand_co = vec2(1.0 * x / SIZE_X, 1.0 * y / SIZE_Y);
        // seed for PRNG
        float rand_state = 1.0 * seed * idx / (SIZE_X * SIZE_Y * SIZE_Z);

        Ray ray = generateRay(x, y); // Initial (camera) ray
        vec3 sample_color = vec3(1.0); // Initial color

        IntersectResult best, candidate;
        best.hit = false;

        for (int d = 0; d < recursion_depth; d++) {

            for (int i = 0; i < nof_objects; i++) {

                candidate = intersect(ray, objects[i], tmin, tmax);

                if (candidate.hit && candidate.t < best.t) {
                    best = candidate;
                }
            }

            if (best.hit) {
                sample_color *= shade(ray, best);
                ray = scatter(ray, best);
            } else {
                sample_color *= getBackgroundColor(ray);
                break;
            }
        }

        samples[idx] = sample_color;

        if (z==0) { // zeroth sampler of each pixel writes to output
            for (int i = 1; i < SIZE_Z; i++) {
                samples[idx] += samples[idx + i];
            }
        }
    }
}
)";

const char fshader[] = R"(
#version 460 core

in vec2 tex_coord;
out vec4 color;

uniform sampler2D render;

void main() {
    color = texture(render, tex_coord).rgba;
}
)";

const char vshader[] = R"(
#version 460 core

layout(location=0) in vec3 coordinate;

out vec2 tex_coord;

void main() {
    gl_Position = vec4(coordinate, 1);
    tex_coord = 0.5 * (coordinate.xy + 1.0);
}
)";
