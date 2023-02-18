#ifndef SIMPLEX_NOISE_H
# define SIMPLEX_NOISE_H

#include <cstddef>  // size_t
#include <random>
#include <numeric>
#include <iostream>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <cstdint>  // int32_t/uint8_t
#include <algorithm>

class SimplexNoise {

public:
    // 1D Perlin simplex noise
    float noise(float x);
    // 2D Perlin simplex noise
    float noise(float x, float y);
    // 3D Perlin simplex noise
    float noise(float x, float y, float z);

    void SetSeed(unsigned int seed);


    // Fractal/Fractional Brownian Motion (fBm) noise summation
    float fractal(size_t octaves, float x);
    float fractal(size_t octaves, float x, float y);
    float fractal(size_t octaves, float x, float y, float z);

    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)
     */
    SimplexNoise(float frequency = 10.0f,
                          float amplitude = 1.0f,
                          float lacunarity = 7.0f,
                          float persistence = 1.5f) :
        mFrequency(frequency),
        mAmplitude(amplitude),
        mLacunarity(lacunarity),
        mPersistence(persistence) {
    }

private:
    std::vector<int> perm;
    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of noise
    float mFrequency;   ///< Frequency ("width") of the first octave of noise (default to 1.0)
    float mAmplitude;   ///< Amplitude ("height") of the first octave of noise (default to 1.0)
    float mLacunarity;  ///< Lacunarity specifies the frequency multiplier between successive octaves (default to 2.0).
    float mPersistence; ///< Persistence is the loss of amplitude between successive octaves (usually 1/lacunarity)


    inline int32_t fastfloor(float fp);

    inline uint8_t hash(int32_t i);

    float grad(int32_t hash, float x);

    float grad(int32_t hash, float x, float y);

    float grad(int32_t hash, float x, float y, float z);
};

#endif