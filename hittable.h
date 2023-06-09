#ifndef HITTABLE_H
#define HITTABLE_H
#include "texture.h"
#include "ray.h"
#include "onb.h"
#include "pdf.h"
class aabb {
public:
    aabb() {}
    aabb(const vec3& a, const vec3& b) { _min = a; _max = b; }

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    bool hit(const ray& r, double tmin, double tmax) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.direction()[a];
            auto t0 = (min()[a] - r.origin()[a]) * invD;
            auto t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            tmin = t0 > tmin ? t0 : tmin;
            tmax = t1 < tmax ? t1 : tmax;
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

    vec3 _min;
    vec3 _max;
};

class material;
struct hit_record {
	vec3 hittedPoint;
	vec3 normal;
    shared_ptr<material> mat_ptr;
    double t;
    double u;
    double v;
    bool front_face;
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
	virtual bool hit(const ray& r,double t_min,double t_max,hit_record& rec) const = 0;
    virtual bool bounding_box(double t0, double t1, aabb& output_box) const = 0;
    virtual float pdf_value(const vec3& o, const vec3& v) const { return 0.0; }
    virtual vec3 random(const vec3& o) const { return vec3(1, 0, 0);}
};
struct scatter_record {
    ray specular_ray;
    bool is_specular;
    vec3 attenuation;
    pdf* pdf_ptr;
};
class material {
public:
    virtual bool scatter(
        const ray& r_in,  hit_record& hrec, scatter_record& srec) const = 0;
    virtual float scattering_pdf(const ray& r_in, hit_record& rec, ray& scattered) {
        return 0.0;
    }
    virtual vec3 emitted(const ray& r_in, const hit_record& rec,double u, double v, const vec3& p) const {
        return vec3(0, 0, 0);
    }
};

class lambertian :public material {
public:
    lambertian( texture* a) : albedo(a) {}
    virtual float scattering_pdf(const ray& r_in, hit_record& rec, ray& scattered) {
        float cosine = dot(rec.normal, unit_vector(scattered.direction()));
        if (cosine < 0) cosine = 0;
        return cosine / pi;
    }
    virtual bool scatter(
        const ray& r_in, hit_record& hrec, scatter_record& srec
    ) const {
        /*
        onb uvw;
        uvw.build_from_w(rec.normal);
        vec3 scatter_direction = uvw.local(random_cosine_direction());
        scattered = ray(rec.hittedPoint, scatter_direction,r_in.time());
        attenuation = albedo->value(rec.u,rec.v,rec.hittedPoint);
        pdf = dot(uvw.w(), scattered.direction()) / pi;
        return true;
        */
        srec.is_specular = false;
        srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.hittedPoint);
        srec.pdf_ptr = new cosine_pdf(hrec.normal);
        return true;
    }
public:
    texture* albedo;
};
class metal :public material {
public:
    metal(const vec3& a , double f) : albedo(a),fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const ray& r_in, hit_record& hrec, scatter_record& srec
    ) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
        srec.specular_ray = ray(hrec.hittedPoint, reflected + fuzz * random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.is_specular = true;
        srec.pdf_ptr = 0;
       // return (dot(scattered.direction(), rec.normal) > 0);
        return  true;
    }

public:
    vec3 albedo;
    double fuzz;

};

class dielectric : public material {
public:
    dielectric(double ri) : ref_idx(ri) {}

    virtual bool scatter(
        const ray& r_in,  hit_record& rec, vec3& attenuation, ray& scattered, float& pdf
    ) const {
        attenuation = vec3(1.0, 1.0, 1.0);
        double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = ffmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (etai_over_etat * sin_theta > 1.0) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.hittedPoint, reflected);
            return true;
        }
        double reflect_prob = schlick(cos_theta, etai_over_etat);
        if (random_double() < reflect_prob)
        {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = ray(rec.hittedPoint, reflected);
            return true;
        }
        vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
        scattered = ray(rec.hittedPoint, refracted);
        return true;
    }

public:
    double ref_idx;
};
/*
class lambertian :public material {
public:
    lambertian( texture* a) : albedo(a) {}
    virtual bool scatter(
        const ray& r_in,  hit_record& rec, vec3& attenuation, ray& scattered
    ) const {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = ray(rec.hittedPoint, scatter_direction,r_in.time());
        attenuation = albedo->value(rec.u,rec.v,rec.hittedPoint);
        return true;
    }
public:
    texture* albedo;
};
*/

class diffuse_light : public material {
public:
    diffuse_light(texture* a) : emit(a) {}

    virtual bool scatter(
        const ray& r_in, hit_record& rec, vec3& attenuation, ray& scattered, float& pdf
    ) const {
        return false;
    }

    virtual vec3 emitted(const ray& r_in, const hit_record& rec, double u, double v, const vec3& p) const {
        if (dot(rec.normal, r_in.direction()) < 0.0)
            return emit->value(u, v, p);
        else
            return vec3(0, 0, 0);
    }

public:
    texture* emit;
};


#endif