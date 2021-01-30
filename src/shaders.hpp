#pragma once
const char pathtracer_comp[] = R"(#version 460 core

// Thread block dimensions
#define SIZE_X 16
#define SIZE_Y 16
#define SIZE_Z 1

// Intersectable types
#define SPHERE 0
#define TRIANGLE 1

// Material types
#define LAMBERTIAN 0
#define METAL 1
#define DIELECTRIC 2
#define EMISSIVE 3

// Mathematical constants
#define PHI 1.61803398874989484820459
#define M_PI 3.1415926535897f

layout (local_size_x = SIZE_X, local_size_y = SIZE_Y, local_size_z = SIZE_Z) in;

layout (rgba32f, binding = 0) uniform image2D output_image;

struct Intersectable {

    // Whatever data is required to specify the intersectable.
    // Interpretation of data depends on the type parameter,
    // see intersect method for details.
    vec4 a, b, c;

    int type, material_idx;

};

struct Material {

    vec4 color;
    float fuzz, ref_idx;
    int type;

};

struct Ray {
    vec3 origin, direction;
    bool inside;
};

struct IntersectResult {

    bool hit;
    float t;
    vec3 normal;
    vec3 point;
    int material_idx;

};


// input scene specifications
layout (std430, binding = 1) buffer scene_objects {

    Intersectable objects[];

};

layout (std430, binding = 2) buffer scene_materials {

    Material materials[];

};

// parameters
layout(std140, binding = 3) uniform params {

    int nx, ny, spp; // output resolution, samples per pixel
    vec4 camPos, camDir, camUp, camRight; // camera position and orientation
    float aperture, focal_distance;

    int recursion_depth;
    float tmin, tmax;

    int nof_objects;

    int seed;

};

uniform int nof_samples;

// shared array to store the result of each sampler thread of each pixel.
shared vec3 samples[SIZE_X][SIZE_Y][SIZE_Z];

// PRNG state for each thread
shared uint randomState[SIZE_X][SIZE_Y][SIZE_Z];

const uint k = 1103515245U;  // GLIB C

// hash function for generating pseudorandom uints
uvec3 hash(uvec3 x) {
    x = ((x>>8U)^x.yzx)*k;
    x = ((x>>8U)^x.yzx)*k;
    x = ((x>>8U)^x.yzx)*k;

    return x;
}

// sample a random vec3 from the uniform distribution [0, 1]^3.
vec3 randVec() {
    uint s = randomState[gl_LocalInvocationID.x][gl_LocalInvocationID.y][gl_LocalInvocationID.z];
    uvec3 new = hash(gl_GlobalInvocationID.xyz * s);
    randomState[gl_LocalInvocationID.x][gl_LocalInvocationID.y][gl_LocalInvocationID.z] = new.x;
    return new * (1.0/float(0xffffffffU));
}

// sample a random float from the uniform distribution [0, 1].
float rand() { return randVec().x; }

// sample a uniformly distributed vec3 from the unit sphere.
vec3 sampleSphere() {
    vec3 rand = randVec();
    float theta = 2 * M_PI * rand.x; // azimuth (uniform)
    float psi =  acos(1.0 -  2*rand.y); // inclination (uniform cosine)
    float r = sin(psi);
    return vec3(r * cos(theta), r * sin(theta), cos(psi));
}

// sample a uniformly distributed vec3 from the unit ball.
vec3 sampleBall() {
    vec3 rand = randVec();
    float theta = 2 * M_PI * rand.x; // azimuth (uniform)
    float psi =  acos(1.0 -  2*rand.y); // inclination (uniform cosine)
    float r = sqrt(rand.z); // radius (uniform square)
    return vec3(r * cos(theta), r * sin(theta), cos(psi));
}

// Find intersection of ray and object between tmin and tmax.
// Assumes ray direction is normalized.
IntersectResult intersect(Ray ray, Intersectable obj, float tmin, float tmax) {

    IntersectResult res;
    res.hit = false;

    if (obj.type == SPHERE) { // Sphere intersection
        vec3 center = obj.a.xyz;
        float radius = obj.b.x;

        vec3 RC = ray.origin - center;
        float b = 2 * dot(ray.direction, RC);
        float c = dot(RC, RC) - radius * radius;
        float D = b * b - 4 * c;
        float t = 0;

        if (D >= 0) {
            t = (-b - sqrt(D)) / 2; // smaller of the two roots
            t = (t > tmin)? t: c/t; // choose larger root if the smaller is behind origin
        }

        if (t > tmin) {
            res.hit = true;
            res.t = t;
            res.point = ray.origin + t * ray.direction;
            res.material_idx = obj.material_idx;
            res.normal = normalize(res.point - center);
            res.normal = (dot(res.normal, ray.direction) > 0.0)? -res.normal: res.normal;
        }

    } else if (obj.type == TRIANGLE) {
        // Triangle intersection (somewhat less efficient than Möller-Trumbore)
        //
        vec3 e_ba = obj.b.xyz - obj.a.xyz;
        vec3 e_ca = obj.c.xyz - obj.a.xyz;

        vec3 p = obj.a.xyz;

        vec3 n = cross(e_ba, e_ca);
        float a = dot(ray.direction, n);

        if (a == 0) return res;
        vec3 nc = cross(n, e_ca);

        float t = dot(p - ray.origin, n) / a;

        vec3 point = ray.origin + t * ray.direction;
        vec3 puv = point - p;
        float u = dot(puv, nc) / dot(e_ba, nc);
        if (u < 0.0 || u > 1.0) return res;
        float v = dot(puv - u*e_ba, e_ca) / dot(e_ca, e_ca);
        if (v < 0.0 || (u + v > 1.0)) return res;

        if (t > tmin) {
            res.hit = true;
            res.t = t;
            res.point = point;
            res.material_idx = obj.material_idx;
            res.normal = (dot(ray.direction, n) < 0.0)? normalize(n): -normalize(n);
        }

    }

    return res;

}

vec3 refract_custom(vec3 uv, vec3 n, float eta) {
    float cos_theta = dot(-uv, n);
    vec3 r_out_perp =  eta * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(abs(1.0 - dot(r_out_perp, r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

Ray scatter(Ray incoming, IntersectResult intersect) {

    Ray ray;
    ray.inside = incoming.inside;
    ray.origin = intersect.point;

    Material material = materials[intersect.material_idx];

    if (material.type == LAMBERTIAN) {

        ray.direction = normalize(intersect.normal + sampleSphere());

    } else if (material.type == METAL) {

        ray.direction = normalize(reflect(incoming.direction, intersect.normal)
                                  + material.fuzz * sampleBall());

    } else if (material.type == DIELECTRIC) {

        float ref_idx = incoming.inside? material.ref_idx: 1.0 / material.ref_idx;
        float cosine = dot(-incoming.direction, intersect.normal);
        float sine = sqrt(1 - cosine * cosine);

        float r0 = pow((1 - ref_idx) / (1 + ref_idx), 2);
        float schlick = r0 + (1 - r0) * pow(1 - cosine, 5);

        // ray is reflected, either due to total internal reflection
        // or randomly due to nonzero reflection coefficient
        if (ref_idx * sine >= 1.0) {
            ray.direction = reflect(incoming.direction,
                                    intersect.normal
                                    );
        } else {
            ray.direction = refract(incoming.direction,
                                    intersect.normal,
                                    ref_idx
                                    );
            ray.inside = !incoming.inside;
        }
    }

    return ray;

}

vec3 getBackgroundColor(Ray ray) {

    const vec3 up = vec3(0, 0, 1);
    const vec3 upColor = vec3(0.0, 0.0, 0.0);
    const vec3 downColor = vec3(0.0, 0.0, 0.0);

    float t = 0.5 * (dot(ray.direction, up) + 1);
    return t*upColor + (1-t)*downColor;
}

Ray generateRay(ivec2 coords, ivec2 size) {

    vec3 aperture_offset = aperture * (rand() * normalize(camRight.xyz)
                              + rand() * normalize(camUp.xyz));
    vec2 frame_coords = vec2(coords) - vec2(size)/2 + vec2(rand(), rand());
    Ray ray;
    ray.origin = camPos.xyz + aperture_offset;
    ray.direction = normalize(camDir.xyz
                              + frame_coords.x * camRight.xyz
                              + frame_coords.y * camUp.xyz
                              - aperture_offset / focal_distance);
    ray.inside = false;

    return ray;
}

void main() {

    int x = int(gl_LocalInvocationID.x);
    int y = int(gl_LocalInvocationID.y);
    int z = int(gl_LocalInvocationID.z);
    samples[x][y][z] = vec3(0.0);
    // initialize PRNG state
    randomState[x][y][z] = nof_samples + seed;

    ivec3 global_id = ivec3(gl_GlobalInvocationID.xyz); // global thread index

    if (all(lessThan(global_id, ivec3(nx, ny, spp)))) {

        IntersectResult best, candidate;

        vec3 sample_color = vec3(0.0);


        // Initial (camera) ray
        Ray ray = generateRay(global_id.xy, ivec2(nx, ny));

        // total albedo from current position to camera
        vec3 albedo = vec3(1.0);

        for (int d = 0; d < recursion_depth; d++) {

            best.hit = false;
            best.t = tmax;

            for (int i = 0; i < nof_objects; i++) { // find closest intersection

                candidate = intersect(ray, objects[i], tmin, tmax);

                if (candidate.hit && candidate.t < best.t) {
                    best = candidate;
                }
            }

            Material material = materials[best.material_idx];

            if (best.hit && material.type != EMISSIVE) { // ray is scattered

                // Stack contribution of this material with the total albedo
                albedo *= material.color.xyz;

                ray = scatter(ray, best);

            } else if (material.type == EMISSIVE) { // ray is absorbed by a black body
                // Add illuminant contribution of this material (multiplied
                // by the total albedo from this point to camera) to final color
                sample_color += albedo * material.color.xyz;
                break;
            } else { // ray goes to infinity
                sample_color += albedo * getBackgroundColor(ray);
                break;

            }
        }

        samples[x][y][z] = sample_color/SIZE_Z;
        barrier(); // synchronize writing to shared memory

        if (z==0) { // zeroth sampler of each pixel writes to output
            for (int i = 1; i < SIZE_Z; i++) {
                samples[x][y][0] += samples[x][y][i];
            }
            vec4 old = imageLoad(output_image, global_id.xy);
            imageStore(output_image, global_id.xy,
                       (vec4(samples[x][y][0], 0.0)
                        + (nof_samples) * old) / (nof_samples + 1));
        }
    }
}
)";
const char quad_frag[] = R"(#version 460 core

in vec2 tex_coord;
out vec4 color;

uniform sampler2D render;

void main() {
    color = texture(render, tex_coord).rgba;
}
)";
const char quad_vert[] = R"(#version 460 core

layout(location=0) in vec3 coordinate;

out vec2 tex_coord;

void main() {
    gl_Position = vec4(coordinate, 1);
    tex_coord = 0.5 * (coordinate.xy + 1.0);
}
)";
