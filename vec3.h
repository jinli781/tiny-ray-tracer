#pragma once 
#include <iostream>

class vec3 {
public:
	vec3() :e{ 0,0,0 } { };
	vec3(double e0, double e1, double e2) :e{ e0,e1,e2 } {};
	double x() const { return e[0]; }
	double y() const { return e[1]; }
	double z() const { return e[2]; }
	vec3 operator -() const{
		return vec3(-e[0],-e[1],-e[2]);
	}
	double operator[](int i) const { return e[i]; }
	double& operator[](int i) { return e[i]; }

	vec3& operator+=(vec3& v) {
		e[0] += v.x();
		e[0] += v.y();
		e[0] += v.z();
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
	void write_color(std::ostream& out) {
		// Write the translated [0,255] value of each color component.
		out << static_cast<int>(255.999 * x()) << ' '
			<< static_cast<int>(255.999 * y()) << ' '
			<< static_cast<int>(255.999 * z()) << '\n';
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