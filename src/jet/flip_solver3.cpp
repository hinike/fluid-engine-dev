// Copyright (c) 2016 Doyub Kim

#include <pch.h>
#include <jet/flip_solver3.h>

using namespace jet;

FlipSolver3::FlipSolver3() {
}

FlipSolver3::~FlipSolver3() {
}

void FlipSolver3::transferFromParticlesToGrids() {
    PicSolver3::transferFromParticlesToGrids();

    // Store snapshot
    _delta.set(*gridSystemData()->velocity());
}

void FlipSolver3::transferFromGridsToParticles() {
    auto flow = gridSystemData()->velocity();
    auto positions = particleSystemData()->positions();
    auto velocities = particleSystemData()->velocities();
    size_t numberOfParticles = particleSystemData()->numberOfParticles();

    // Compute delta
    flow->parallelForEachUIndex([&](size_t i, size_t j, size_t k) {
        _delta.u(i, j, k) = flow->u(i, j, k) - _delta.u(i, j, k);
    });

    flow->parallelForEachVIndex([&](size_t i, size_t j, size_t k) {
        _delta.v(i, j, k) = flow->v(i, j, k) - _delta.v(i, j, k);
    });

    flow->parallelForEachWIndex([&](size_t i, size_t j, size_t k) {
        _delta.w(i, j, k) = flow->w(i, j, k) - _delta.w(i, j, k);
    });

    // Transfer delta to the particles
    parallelFor(kZeroSize, numberOfParticles, [&](size_t i) {
        velocities[i] += _delta.sample(positions[i]);
    });
}
