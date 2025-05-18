#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <glm/glm.hpp>

class Random {
public:
    // Initializes the random number generator (optional, can be called once at startup)
    static void init();

    // Generates a random float between min (inclusive) and max (exclusive)
    // For max inclusive, the caller can adjust or use a slightly larger max.
    static float range(float min, float max);

    // Generates a random int between min (inclusive) and max (inclusive)
    static int range(int min, int max);

    // Generates a random glm::vec3 with components between min and max
    static glm::vec3 range_vec3(float min_val, float max_val);
    static glm::vec3 range_vec3(const glm::vec3& min_vec, const glm::vec3& max_vec);

private:
    static std::mt19937 gen;
};

#endif // RANDOM_H 