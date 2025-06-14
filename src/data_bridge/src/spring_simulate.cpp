#include "spring_simulate.hpp"

trajectory simulate(const Eigen::Vector3d& x0_vec, double k, double c, const Eigen::Vector3d& m, const Eigen::Vector3d& f_ext) {
    double v0 = 0.0;
    double dt = 0.001;
    double T = 20.0;
    int n_steps = static_cast<int>(T / dt);

    std::vector<Eigen::Vector3d> positions(n_steps);
    std::vector<Eigen::Vector3d> velocities(n_steps);
    std::vector<Eigen::Vector3d> accelerations(n_steps);

    Eigen::Vector3d x = x0_vec;
    Eigen::Vector3d v = Eigen::Vector3d::Constant(v0);
    Eigen::Vector3d a = Eigen::Vector3d::Zero();

    positions[0] = x;
    velocities[0] = v;
    accelerations[0] = a;

    auto dx_dt = [](const Eigen::Vector3d& v) {
        return v;
    };

    auto dv_dt = [&](const Eigen::Vector3d& x, const Eigen::Vector3d& v) {
        return (-c * v - k * x + f_ext).array() / m.array();
    };

    for (int i = 1; i < n_steps; ++i) {
        Eigen::Vector3d k1_x = dx_dt(v);
        Eigen::Vector3d k1_v = dv_dt(x, v);

        Eigen::Vector3d k2_x = dx_dt(v + 0.5 * dt * k1_v);
        Eigen::Vector3d k2_v = dv_dt(x + 0.5 * dt * k1_x, v + 0.5 * dt * k1_v);

        Eigen::Vector3d k3_x = dx_dt(v + 0.5 * dt * k2_v);
        Eigen::Vector3d k3_v = dv_dt(x + 0.5 * dt * k2_x, v + 0.5 * dt * k2_v);

        Eigen::Vector3d k4_x = dx_dt(v + dt * k3_v);
        Eigen::Vector3d k4_v = dv_dt(x + dt * k3_x, v + dt * k3_v);

        x += (dt / 6.0) * (k1_x + 2 * k2_x + 2 * k3_x + k4_x);
        v += (dt / 6.0) * (k1_v + 2 * k2_v + 2 * k3_v + k4_v);
        a = dv_dt(x, v);  // compute acceleration from latest x and v

        positions[i] = x;
        velocities[i] = v;
        accelerations[i] = a;
    }

    return {positions, velocities, accelerations};
}
