#include "Random.h"

void Random::init() {
    // Seed the generator explicitly if called.
    // The static generator in generator() will self-seed on first use anyway.
    generator().seed(std::random_device()());
}

std::mt19937& Random::generator() {
    // Using a static variable inside a function ensures it's initialized on first use.
    // C++11 guarantees this is thread-safe.
    // It's seeded with std::random_device for a non-deterministic start.
    static std::mt19937 gen(std::random_device()());
    return gen;
}

float Random::range(float min, float max) {
    if (min >= max) return min; // Or handle error, or swap
    std::uniform_real_distribution<float> dist(min, max);
    return dist(generator());
}

int Random::range(int min, int max) {
    if (min >= max) return min; // Or handle error, or swap. For int, max is inclusive.
    std::uniform_int_distribution<int> dist(min, max);
    return dist(generator());
}

glm::vec3 Random::range_vec3(float min_val, float max_val) {
    return glm::vec3(
        range(min_val, max_val),
        range(min_val, max_val),
        range(min_val, max_val)
    );
}

glm::vec3 Random::range_vec3(const glm::vec3& min_vec, const glm::vec3& max_vec) {
    return glm::vec3(
        range(min_vec.x, max_vec.x),
        range(min_vec.y, max_vec.y),
        range(min_vec.z, max_vec.z)
    );
} 