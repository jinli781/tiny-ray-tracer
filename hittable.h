#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"
class material;
struct hit_record {
	vec3 hittedPoint;
	vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    bool front_face;

    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
	virtual bool hit(const ray& r,double t_min,double t_max,hit_record& rec) const = 0;
};
class material {
public:
    virtual bool scatter(
        const ray& r_in,  hit_record& rec, vec3& attenuation, ray& scattered
    ) const = 0;
};
class lambertian :public material {
public:
    lambertian(const vec3& a) : albedo(a) {}
    virtual bool scatter(
        const ray& r_in,  hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = ray(rec.hittedPoint, scatter_direction);
        attenuation = albedo;
        return true;
    }
public:
    vec3 albedo;
};
class metal :public material {
public:
    metal(const vec3& a , double f) : albedo(a),fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in,  hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.hittedPoint, reflected + fuzz * random_in_unit_sphere());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    vec3 albedo;
    double fuzz;

};
#endif