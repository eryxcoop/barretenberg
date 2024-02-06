#include "barretenberg/common/assert.hpp"
#include "barretenberg/ecc/curves/bn254/bn254.hpp"
#include "barretenberg/ecc/scalar_multiplication/scalar_multiplication.hpp"
#include "barretenberg/polynomials/polynomial_arithmetic.hpp"
#include "barretenberg/srs/factories/file_crs_factory.hpp"

#include "barretenberg/proof_system/circuit_builder/ultra_circuit_builder.hpp"
#include "barretenberg/ultra_honk/ultra_composer.hpp"

#include <chrono>
#include <cstdlib>

using namespace bb;

constexpr size_t NUM_POINTS = 1 << 16;
std::vector<fr> scalars;
static bb::evaluation_domain small_domain;
static bb::evaluation_domain large_domain;

const auto init = []() {
    small_domain = bb::evaluation_domain(NUM_POINTS);
    large_domain = bb::evaluation_domain(NUM_POINTS * 4);

    fr element = fr::random_element();
    fr accumulator = element;
    scalars.reserve(NUM_POINTS * 4);
    for (size_t i = 0; i < NUM_POINTS * 4; ++i) {
        accumulator *= element;
        scalars.emplace_back(accumulator);
    }

    return 1;
};

auto reference_string = std::make_shared<bb::srs::factories::FileProverCrs<curve::BN254>>(NUM_POINTS, "/tmp/ignition");

int pippenger()
{
    scalar_multiplication::pippenger_runtime_state<curve::BN254> state(NUM_POINTS);
    std::chrono::steady_clock::time_point time_start = std::chrono::steady_clock::now();
    g1::element result = scalar_multiplication::pippenger_unsafe<curve::BN254>(
        &scalars[0], reference_string->get_monomial_points(), NUM_POINTS, state);
    std::chrono::steady_clock::time_point time_end = std::chrono::steady_clock::now();
    std::chrono::microseconds diff = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
    std::cout << "run time: " << (double)diff.count() / 1000. << "ms" << std::endl;
    std::cout << result.x << std::endl;
    return 0;
}

int main()
{
    bb::srs::init_crs_factory("/tmp/ignition");
    std::cout << "initializing" << std::endl;
    init();
    std::cout << "executing pippenger algorithm" << std::endl;
    pippenger();
    pippenger();
    pippenger();
    pippenger();
    pippenger();
    return 0;
}
