#pragma once
#include "rtweekend.h"
#include "perlin.h"
#include "vec3.h"
class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture {
public:
    constant_texture() {}

    constant_texture(vec3 c) : color(c) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        return color;
    }
public:
    vec3 color;
};

class checker_texture : public texture {
public:
    checker_texture() {}
    checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    texture* odd;
    texture* even;
};
class noise_texture : public texture {
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    virtual vec3 value(float u, float v, const vec3& p) const {
        return vec3(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turb(p)));
    }

public:
    perlin noise;
    double scale;
};
#include "texture.h"

class image_texture : public texture {
public:
    image_texture() {}
    image_texture(unsigned char* pixels, int A, int B)
        : data(pixels), nx(A), ny(B) {}

    ~image_texture() {
        delete data;
    }

    virtual vec3 value(float u, float v, const vec3& p) const {
        // If we have no texture data, then always emit cyan (as a debugging aid).
        if (data == nullptr) {
            //std::cerr << "\nload image error.\n";
            return vec3(0, 1, 1);
            
        }
        auto i = static_cast<int>((u)*nx);
        auto j = static_cast<int>((1 - v) * ny - 0.001);

        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i > nx - 1) i = nx - 1;
        if (j > ny - 1) j = ny - 1;

        auto r = static_cast<int>(data[3 * i + 3 * nx * j + 0]) / 255.0;
        auto g = static_cast<int>(data[3 * i + 3 * nx * j + 1]) / 255.0;
        auto b = static_cast<int>(data[3 * i + 3 * nx * j + 2]) / 255.0;

        return vec3(r, g, b);
    }

public:
    unsigned char* data;
    int nx, ny;
};