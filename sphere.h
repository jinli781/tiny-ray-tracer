//sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() { center = vec3(); radius = 0; }
     sphere(vec3 cen, double r, shared_ptr<material> m)
         : center(cen), radius(r),mat_ptr(m) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;

public:
    vec3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 co = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(r.direction(), co);
    auto c = co.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;
    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root) / a;
        if (temp <= t_max && temp >= t_min) {
            rec.t = temp;
            rec.hittedPoint = r.at(temp);
            vec3 outward_normal = (rec.hittedPoint - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp <= t_max && temp >= t_min) {
            rec.t = temp;
            rec.hittedPoint = r.at(rec.t);
            vec3 outward_normal = (rec.hittedPoint - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}
#endif
