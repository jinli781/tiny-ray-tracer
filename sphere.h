//sphere.h
#ifndef SPHERE_H
#define SPHERE_H
 
#include "hittable.h"
#include "vec3.h"
void get_sphere_uv(const vec3& p, double& u, double& v) {
    auto phi = atan2(p.z(), p.x());
    auto theta = asin(p.y());
    u = 1 - (phi + pi) / (2 * pi);
    v = (theta + pi / 2) / pi;
}
class sphere : public hittable {
public:
    sphere() { center = vec3(); radius = 0; }
     sphere(vec3 cen, double r, shared_ptr<material> m)
         : center(cen), radius(r),mat_ptr(m) {};

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
    virtual float pdf_value(const vec3& o, const vec3& v) const;
    virtual vec3 random(const vec3& o) const;
public:
    vec3 center;
    double radius;
    shared_ptr<material> mat_ptr;
};
    float sphere::pdf_value(const vec3& o, const vec3& v)const {
        hit_record rec;
        if (this->hit(ray(o, v), 0.001, infinity, rec)) {
        float cos_theta_max = sqrt(1 - radius*radius/(center - o).length_squared());
        float solid_angle = 2 * pi * (1 - cos_theta_max);
        return 1 / solid_angle;
        }
        else
        {
            return 0;
        }
}
    vec3 sphere::random(const vec3& o) const {
        vec3 direction = center - o;
        float distance_squared = direction.length_squared();
        onb uvw;
        uvw.build_from_w(direction);
        return uvw.local(random_to_sphere(radius,distance_squared));
    }
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
            get_sphere_uv((rec.hittedPoint - center) / radius, rec.u, rec.v);
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp <= t_max && temp >= t_min) {
            rec.t = temp;
            rec.hittedPoint = r.at(rec.t);
            vec3 outward_normal = (rec.hittedPoint - center) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
             get_sphere_uv((rec.hittedPoint - center) / radius, rec.u, rec.v);
            return true;
        }
    }
    return false;
}
bool sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    output_box = aabb(
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius));
    return true;
}
class moving_sphere : public hittable {
public:
    moving_sphere() {}
    moving_sphere(
        vec3 cen0, vec3 cen1, double t0, double t1, double r, shared_ptr<material> m)
        : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat_ptr(m)
    {};

    virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
    vec3 center(double time) const;

public:
    vec3 center0, center1;
    double time0, time1;
    double radius;
    shared_ptr<material> mat_ptr;
};

vec3 moving_sphere::center(double time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}
bool moving_sphere::hit(
    const ray& r, double t_min, double t_max, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;

    if (discriminant > 0) {
        auto root = sqrt(discriminant);

        auto temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.hittedPoint = r.at(rec.t);
            vec3 outward_normal = (rec.hittedPoint - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.hittedPoint - center(r.time())) / radius, rec.u, rec.v);
            return true;
        }

        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            rec.t = temp;
            rec.hittedPoint = r.at(rec.t);
            vec3 outward_normal = (rec.hittedPoint - center(r.time())) / radius;
            rec.set_face_normal(r, outward_normal);
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.hittedPoint - center(r.time())) / radius, rec.u, rec.v);
            return true;
        }
    }
    return false;
}
bool moving_sphere::bounding_box(double t0, double t1, aabb& output_box) const {
    aabb box0(
        center(t0) - vec3(radius, radius, radius),
        center(t0) + vec3(radius, radius, radius));
    aabb box1(
        center(t1) - vec3(radius, radius, radius),
        center(t1) + vec3(radius, radius, radius));
    output_box = surrounding_box(box0, box1);
    return true;
}
#endif
