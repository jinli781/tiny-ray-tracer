#pragma once 
#include <iostream>
class vec3 {
public:
	vec3() :e{ 0,0,0 } { };
	vec3(double e0, double e1, double e2) :e{ e0,e1,e2 } {};
	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }
	double at(int x) const { return e[x]; }
	vec3 operator -() const{
		return vec3(-e[0],-e[1],-e[2]);
	}
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(const vec3& v) {
		e[0] += v.x();
		e[1] += v.y();
		e[2] += v.z();
		return *this;
	}
	vec3& operator*=(const double t) {
		e[0] *= t;
		e[1] *= t;
		e[2] *= t;
		return *this;
	}
	vec3& operator/=(const double t) {
		e[0] /= t;
		e[1] /= t;
		e[2] /= t;
		return *this;
	}
	double length_squared() const {
		return x() * x() + y() * y() + z() * z();

	}
	double length() const {
		return sqrt(length_squared());
	}
	void write_color(std::ostream& out, int samples_per_pixel) {
		double scale = 1.0 / samples_per_pixel;
		double r = sqrt(scale * x());
		double g = sqrt(scale * y());
		double b = sqrt(scale * z());
		// Write the translated [0,255] value of each color component.
		out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
			<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
			<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
	}


private:
	double e[3];
};
inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.x() << ' ' << v.y() << ' ' << v.z();
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.x() + v.x(), u.y() + v.y(), u.z() + v.z());
}
inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.x() - v.x(), u.y() - v.y(), u.z() - v.z());
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.x() * v.x(), u.y() * v.y(), u.z() * v.z());
}

inline vec3 operator*(double t, const vec3& v) {
	return vec3(t * v.x(), t * v.y(), t * v.z());
}

inline vec3 operator*(const vec3& v, double t) {
	return t * v;
}

inline vec3 operator/(vec3 v, double t) {
	return (1 / t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u.x() * v.x()
		+ u.y() * v.y()
		+ u.z() * v.z();
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(u.y() * v.z() - u.z() * v.y(),
		u.z() * v.x() - u.x() * v.z(),
		u.x() * v.y() - u.y() * v.x());
}

inline vec3 unit_vector(vec3 v) {
	return v / v.length();
}
inline static vec3 random() {
	return vec3(random_double(), random_double(), random_double());
}

inline static vec3 random(double min, double max) {
	return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}
vec3 random_in_unit_sphere() {
	while (true) {
		auto p = random(-1, 1);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}
vec3 random_unit_vector() {
	auto a = random_double(0, 2 * pi);
	auto z = random_double(-1, 1);
	auto r = sqrt(1 - z * z);
	return vec3(r * cos(a), r * sin(a), z);
}
vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}
vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
	auto cos_theta = dot(-uv, n);
	vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
	vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.length_squared()) * n;
	return r_out_parallel + r_out_perp;
}
double schlick(double cosine, double ref_idx) {
	auto r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	return r0 + (1 - r0) * pow((1 - cosine), 5);
}
vec3 random_in_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared() >= 1) continue;
		return p;
	}
}
vec3 random_cosine_direction() {
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}