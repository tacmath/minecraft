#ifndef NOISE_CLASS_H
#define NOISE_CLASS_H

#include <random>
#include <numeric>
#include <iostream>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>

class Noise {
public:
	//permutations list
	std::vector<int> permutation;

	Noise(unsigned int seed) {
		// set size to 256
		permutation.resize(256);

		// fill with numbers from 0 to 255
		std::iota(permutation.begin(), permutation.end(), 0);

		std::default_random_engine engine(seed);

		std::shuffle(permutation.begin(), permutation.end(), engine);

		//duplique la liste
		permutation.insert(permutation.end(), permutation.begin(), permutation.end());
		
		/*
		for (int n = 0; n < 256; n++) {
			std::cout << permutation[n] << std::endl;
		}
		std::cout << "seed == " << seed << std::endl;*/
	//	for (int n = 1000; n < 1200; n++)
		//	std::cout << "noise == " <<	noise(n * (1.0f / 37.5f)) << std::endl;
			
	}

	float noise(float x, float y) {
		glm::vec2 p(x, y);
		/* Calculate lattice points. */
		glm::vec2 p0 = floor(p);
		glm::vec2 p1 = p0 + glm::vec2(1.0, 0.0);
		glm::vec2 p2 = p0 + glm::vec2(0.0, 1.0);
		glm::vec2 p3 = p0 + glm::vec2(1.0, 1.0);

		/* Look up gradients at lattice points. */
		glm::vec2 g0 = grad(p0);
		glm::vec2 g1 = grad(p1);
		glm::vec2 g2 = grad(p2);
		glm::vec2 g3 = grad(p3);

		float t0 = p.x - p0.x;
		float fade_t0 = fade(t0); /* Used for interpolation in horizontal direction */

		float t1 = p.y - p0.y;
		float fade_t1 = fade(t1); /* Used for interpolation in vertical direction. */

		/* Calculate dot products and interpolate.*/
		float p0p1 = (1.0f - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1)); /* between upper two lattice points */
		float p2p3 = (1.0f - fade_t0) * dot(g2, (p - p2)) + fade_t0 * dot(g3, (p - p3)); /* between lower two lattice points */

		/* Calculate final result */
		return (1.0f - fade_t1) * p0p1 + fade_t1 * p2p3;
	}

private:

	float fade(float t) {
		return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
	}

	glm::vec2 grad(glm::vec2 p) {
		return glm::vec2(permutation[(int)floor(p.x) & 255] > 127 ? 1.0f : -1.0f,
			permutation[(int)floor(p.y) & 255] > 127 ? 1.0f : -1.0f);
	}
};

#endif // !NOISE_CLASS_H
