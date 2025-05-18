#include "Random.h"

// Define and initialize the static member gen
// It's default-initialized here. init() will seed it.
std::mt19937 Random::gen;

void Random::init() {
    // Seed the static member gen
    Random::gen.seed(std::random_device()());
}

float Random::range(float min, float max) {
    if (min >= max) return min;
    std::uniform_real_distribution<float> dist(min, max);
    return dist(Random::gen);
}

int Random::range(int min, int max) {
    if (min >= max) return min; 
    std::uniform_int_distribution<int> dist(min, max);
    return dist(Random::gen);
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