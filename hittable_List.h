#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) {
        add(object);
    }
    void clear() { objects.clear(); }
    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }
    
   virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const ;
   virtual bool bounding_box(double t0, double t1, aabb& output_box) const;
public:
    std::vector<shared_ptr<hittable>> objects;
};
bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
    double closest_so_far = t_max;
    hit_record temp_rec ;
    bool hit_anything = false;
    for (const auto& object : objects) {
        if (object->hit(r, t_min, closest_so_far, temp_rec)) {
            closest_so_far = temp_rec.t;
            hit_anything = true;
            rec = temp_rec;
        }
    
    }
    return hit_anything;
}
aabb surrounding_box(aabb box0, aabb box1) {
    vec3 small(ffmin(box0.min().x(), box1.min().x()),
        ffmin(box0.min().y(), box1.min().y()),
        ffmin(box0.min().z(), box1.min().z()));
    vec3 big(ffmax(box0.max().x(), box1.max().x()),
        ffmax(box0.max().y(), box1.max().y()),
        ffmax(box0.max().z(), box1.max().z()));
    return aabb(small, big);
}

bool hittable_list::bounding_box(double t0, double t1, aabb& output_box) const {
    if (objects.empty()) return false;

    aabb temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(t0, t1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

#endif
