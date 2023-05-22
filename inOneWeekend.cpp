#include "rtweekend.h"
#include "camera.h"
#include "hittable_list.h"
#include "bvh.h"
#include "rectangle.h"
#include "sphere.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(new constant_texture(vec3(0.65, 0.05, 0.05)));
    auto white = make_shared<lambertian>(new constant_texture(vec3(0.73, 0.73, 0.73)));
    auto green = make_shared<lambertian>(new constant_texture(vec3(0.12, 0.45, 0.15)));
    auto light = make_shared<diffuse_light>(new constant_texture(vec3(15, 15, 15)));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<hittable> box1 = make_shared<box>(vec3(0, 0, 0), vec3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(vec3(0, 0, 0), vec3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);
    return objects;
}
hittable_list earth() {
    int nx, ny, nn;
    unsigned char* texture_data = stbi_load("C:/Users/Administrator/Pictures/earth.jpg", &nx, &ny, &nn, 0);

    auto earth_surface =
        make_shared<lambertian>(new image_texture(texture_data, nx, ny));
    auto globe = make_shared<sphere>(vec3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = new noise_texture();
    objects.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(vec3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}
bvh_node random_scene() {
    hittable_list world;
    
   
    auto checker = new checker_texture(
        new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9))
        );
        
    //texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
      //  new constant_texture(vec3(0.9, 0.9, 0.9)));
    
    world.add(make_shared<sphere>(vec3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));
    
    //world.add(make_shared<sphere>(
       // vec3(0, -1000, 0), 1000, make_shared<lambertian>(vec3(0.5, 0.5, 0.5))));

    int i = 1;
    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
            if ((center - vec3(4, .2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random() * random();
                    world.add(make_shared<moving_sphere>(
                        center, center + vec3(0, random_double(0, .5), 0), 0.0, 1.0, 0.2,
                        make_shared<lambertian>(new constant_texture(albedo))));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    world.add(
                        make_shared<sphere>(center, 0.2, make_shared<metal>(albedo, fuzz)));
                }
                else {
                    // glass
                    world.add(make_shared<sphere>(center, 0.2, make_shared<dielectric>(1.5)));
                }
            }
        }
    }

    world.add(make_shared<sphere>(vec3(0, 1, 0), 1.0, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        vec3(-4, 1, 0), 1.0, make_shared<lambertian>(new constant_texture(vec3(0.4, 0.2, 0.1)))));
    world.add(make_shared<sphere>(
        vec3(4, 1, 0), 1.0, make_shared<metal>(vec3(0.7, 0.6, 0.5), 0.0)));
     return bvh_node(world, 0, 1);
    // return world;
}
/*
vec3 ray_color(const ray& r,const hittable& world,int depth) {
    hit_record rec;
    vec3 color;
    if (depth <= 0)
        return vec3(0, 0, 0);
    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        vec3 attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return vec3(0, 0, 0);
        //vec3 target = rec.hittedPoint + rec.normal + random_in_unit_sphere();
        //return 0.5 * ray_color(ray(rec.hittedPoint,target-rec.hittedPoint),world,depth-1);
        //return vec3(1, 1, 1);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    //return vec3(1, 1, 1);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}
*/
vec3 ray_color(const ray& r, const vec3& background, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return vec3(0, 0, 0);

    // If the ray hits nothing, return the background color.
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(r,rec,rec.u, rec.v, rec.hittedPoint);
    float pdf_val;
    vec3 albedo;
    if (rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf_val)) {
        hittable* light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
        hittable_pdf p0(light_shape, rec.hittedPoint);
        cosine_pdf p1(rec.normal);
        mixture_pdf p(&p0, &p1);
        scattered = ray(rec.hittedPoint, p.generate(), r.time());
        pdf_val = p.value(scattered.direction());
        return emitted + albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_color(scattered, background, world, depth - 1) / pdf_val;
    }
    else
        return emitted;
    //return emitted + attenuation * ray_color(scattered, background, world, depth - 1);
}
int main() {
    auto world = cornell_box();
    const int image_width = 1280;
    const int image_height = 720;
    const int samples_per_pixel = 5;
    int depth = 15;
    vec3 background(0, 0, 0);
    const auto aspect_ratio = double(image_width) / image_height;
    vec3 lookfrom(278, 278, -800);
    vec3 lookat(278, 278, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; i++) {
            vec3 color(0,0,0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    auto x = (i+random_double()) / image_width;
                    auto y = (j+random_double()) / image_height;
                    ray r = cam.get_ray(x, y);
                    color += ray_color(r, background,world,10);
                }
            color.write_color(std::cout,samples_per_pixel);
        }
    }
    std::cerr << "\nDone.\n";
}