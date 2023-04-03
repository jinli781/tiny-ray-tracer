//sphere.h
#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

class sphere : public hittable {
public:
    sphere() {}
     sphere(vec3 cen, double r )
         : center(cen), radius(r) {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;

public:
    vec3 center;
    double radius;
};
bool sphere::hit(const ray& r, double tmin, double tmax, hit_record& rec) const {
    vec3 co = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(r.direction(), co);
    auto c = co.length_squared() - radius * radius;
    auto discriminant = half_b * half_b - a * c;
    if (discriminant > 0) {
        auto root = sqrt(discriminant);
        auto temp = (-half_b - root) / a;
        if (temp <= tmax && temp >= tmin) {
            rec.t = temp;
            rec.hittedPoint = r.at(temp);
            vec3 outward_normal = (rec.hittedPoint - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp <= tmax && temp >= tmin) {
            rec.t = temp;
            rec.hittedPoint = r.at(rec.t);
            vec3 outward_normal = (rec.hittedPoint - center) / radius;
            rec.set_face_normal(r, outward_normal);
            return true;
        }
    }
    return false;
}
#endif
