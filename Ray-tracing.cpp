#include <iostream>
#include <algorithm>	
#include <cmath>

class Vec {
public:
	double x = 0;
	double y = 0;
	double z = 0;

public:
	Vec() = default;

	Vec(double const x, double const y, double const z) :
			x(x),
			y(y),
			z(z){}

public:
	Vec operator + (const Vec &v) const{
		return Vec(
			x + v.x,
			y + v.y,
			z + v.z
		);
	}

	Vec operator - (const Vec &v) const{
		return Vec(
			x - v.x,
			y - v.y,
			z - v.z
		);
	}

	Vec operator * (double const scalar) const{
		return Vec(
			x * scalar,
			y * scalar,
			z * scalar
		);
	}

	Vec operator / (double const scalar) const{
		return Vec(
			x / scalar,
			y / scalar,
			z / scalar
		);
	}

	double magnitude2() const{
		return
			x * x +
			y * y +
			z * z
		;
	}

	double magnitude() const{
		return sqrt( magnitude2() );
	}

	Vec normalize() const{
		double mg = magnitude();
		return Vec(
			x / mg,
			y / mg,
			z / mg
		);
	}

	double dot(const Vec &v) const{
		return
			x * v.x +
			y * v.y +
			z * v.z;
	}

};

inline double dot(const Vec &a, const Vec &b){
	return a.dot(b);
}

// ========================

class Ray {
public:
	Vec o; // origin
	Vec d; // direction

public:
	Ray(const Vec &o, const Vec &d) :
			o(o),
			d(d){}

	Ray(Vec &&o, Vec &&d) :
			o(std::move(o)),
			d(std::move(d)){}

};

// ========================

class Sphere {
public:
	Vec	c; // Center position
	double	r; // Radius

public:
	Sphere(const Vec &c, double const r) :
			c(c),
			r(r){}
public:
	Vec getNormal(const Vec &pi) const{
		return (pi - c) / r;
	}

	bool intersect3(const Ray &ray, double &t) const{
		const Vec &oc = ray.o - c;

		double const b = 2 * dot(oc, ray.d);

		double const c = dot(oc, oc) - r * r;

		double const discriminant = b * b - 4 * c;

		if (discriminant < 0)
			return false;

		double const sqrt_discriminant = sqrt(discriminant);

		double const t0 = -b - sqrt_discriminant;
		double const t1 = -b + sqrt_discriminant;

		using std::min;

		t = min(t0, t1);

		return true;
	}

private:

};

// ========================

class Color {
public:
	static constexpr char OS_DELIMITER = '\n';

public:
	static const Color BLACK;
	static const Color WHITE;
	static const Color RED;

public:
	double r = 0;
	double g = 0;
	double b = 0;

public:
	constexpr Color() = default;

	constexpr Color(double const r, double const g, double const b) :
			r(r),
			g(g),
			b(b){}

public:
	Color operator * (double const d) const{
		return Color(
			r * d,
			g * d,
			b * d
		);
	}

	Color operator + (const Color &c) const{
		return Color(
			(r + c.r) / 2,
			(g + c.g) / 2,
			(b + c.b) / 2);
		}

	void cap(){
		clamp__(r);
		clamp__(g);
		clamp__(b);
	};

private:
	static void clamp__(double &a){
		if (a < 0x00){
			a = 0x00;
		}else if (a > 0xff){
			a = 0xff;
		}
	}
};

constexpr Color Color::BLACK = { 0x00, 0x00, 0x00 };
constexpr Color Color::WHITE = { 0xff, 0xff, 0xff };
constexpr Color Color::RED   = { 0xff, 0x00, 0x00 };

inline std::ostream& operator<<(std::ostream& os, const Color& c){
	os	<< int(c.r) << c.OS_DELIMITER
		<< int(c.g) << c.OS_DELIMITER
		<< int(c.b) << c.OS_DELIMITER
	;

	return os;
}


// ========================

#include <fstream>

//#include <iostream>

constexpr const char *FILE_NAME = "out.ppm";
constexpr unsigned WIDTH = 640;
constexpr unsigned HEIGHT = 480;

int main(){
	std::ofstream out(FILE_NAME);
	out << "P3\n" << WIDTH << '\n' << HEIGHT << '\n' << "255\n";

	const Color &white = Color::WHITE;
	const Color &red   = Color::RED;

	const Sphere sphere(Vec(WIDTH / 2, HEIGHT / 2, 50), 150);
	const Sphere light(Vec(0, 0, 0), 1);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			// Send a ray through each pixel
			const Ray ray(Vec(x, y, 0), Vec(0, 0, 1));

			double t = 0;

			Color pixel = Color::BLACK;

			// Check for intersections
			if (sphere.intersect3(ray, t)){
				// Point of intersection
				const Vec &pi = ray.o + ray.d * t;

				// Color the pixel
				const Vec &L = light.c - pi;
				const Vec &N = sphere.getNormal(pi);

				double const dt = dot(L.normalize(), N.normalize());

				pixel = red + (white * dt) * 1.0;
				pixel.cap();
			}

			out << pixel;
		}
	}
}
