#include "Particle.hpp"
#include <fstream>
#include <random>
#include <string>
#include <nlohmann/json.hpp>
#include <cmath>

Particle::Particle(double mu, double x, double y, double z,
                  double x0, double y0, double z0, double v_x, double v_y, double v_z, double T, double R,
                  double sigma0, double sigma, double T0, double omega) :
                  mu_(mu), x_(x), y_(y), z_(z), x0_(x0), y0_(y0), z0_(z0), v_x_(v_x), v_y_(v_y), v_z_(v_z), T_(T), R_(R),
                  sigma0_(sigma0), sigma_(sigma), T0_(T0), omega_(omega) {
}

Particle Particle::InitSource(double T, double T_back, double mu, double R, double sigma0,
                              double T0, double omega, double a, double b, double c) {
  static thread_local std::mt19937 gen(std::random_device{}());
  static thread_local std::uniform_real_distribution<> dist(0.0, 1.0);
  double x = dist(gen);
  double y = dist(gen);
  double z = dist(gen);
  x *= a;
  y *= b;
  z *= c;
  double sigma = sigma0 * std::pow(T0 / T, omega);
  Particle p(mu, x, y, z, x, y, z, 0.0, 0.0, 0.0, T, R, sigma0, sigma, T0, omega);
  p.InitializeVelocitities();
  return p;
}

void Particle::InitializeVelocitities() {
  static thread_local std::mt19937 gen(std::random_device{}());
  static thread_local std::normal_distribution<> dist(0.0, 1.0);

  double scale = std::pow(R_ * T_ / mu_, 0.5);
  v_x_ = dist(gen) * scale;
  v_y_ = dist(gen) * scale;
  v_z_ = dist(gen) * scale;
}

double Particle::GetVelocityModule() const {
  return std::pow(v_x_ * v_x_ + v_y_ * v_y_ + v_z_ * v_z_, 0.5);
}

double Particle::GetX() const {
  return x_;
}

double Particle::GetY() const {
  return y_;
}

double Particle::GetZ() const {
  return z_;
}

double Particle::GetVx() const {
  return v_x_;
}

double Particle::GetVy() const {
  return v_y_;
}

double Particle::GetVz() const {
  return v_z_;
}

void Particle::Collision(const BackgroundGas& gas, std::array<double, 3> arr) {
  double back_v_x = arr[0];
  double back_v_y = arr[1];
  double back_v_z = arr[2];

  double center_v_x = (v_x_ * mu_ + back_v_x * gas.GetMu()) / (mu_ + gas.GetMu());
  double center_v_y = (v_y_ * mu_ + back_v_y * gas.GetMu()) / (mu_ + gas.GetMu());
  double center_v_z = (v_z_ * mu_ + back_v_z * gas.GetMu()) / (mu_ + gas.GetMu());

  v_x_ = 2 * center_v_x - v_x_;
  v_y_ = 2 * center_v_y - v_y_;
  v_z_ = 2 * center_v_z - v_z_;
}

std::optional<std::array<double, 3>> Particle::CheckCollision(const BackgroundGas& gas, double dt) const {

  double v_x_back = gas.GetVelocityComp();
  double v_y_back = gas.GetVelocityComp();
  double v_z_back = gas.GetVelocityComp();

  double v_x = (v_x_ - v_x_back);
  double v_y = (v_y_ - v_y_back);
  double v_z = (v_z_ - v_z_back);
  double v = std::sqrt(v_x * v_x + v_y * v_y + v_z * v_z); // относительная скорость

  double P = 1 - std::exp(-gas.GetN() * sigma_ * v * dt);

  static thread_local std::mt19937 gen(std::random_device{}());
  static thread_local std::uniform_real_distribution<> dist(0.0, 1.0);
  double r = dist(gen);
  if (r < P) {
    return std::array<double, 3>{v_x_back, v_y_back, v_z_back};
  }
  return std::nullopt;
}

void Particle::UpdateSigma(const BackgroundGas& gas) {
  sigma_ = sigma0_ * std::pow(T0_ / gas.GetT(), omega_);
}

void Particle::Update(const BackgroundGas& gas, double dt) {
  x_ += v_x_ * dt;
  y_ += v_y_ * dt;
  z_ += v_z_ * dt;
  UpdateSigma(gas);
  auto back_v = CheckCollision(gas, dt);
  if (back_v) {
    Collision(gas, *back_v);
  }
}

double Particle::GetQuadraticRadius() const {
  double x = (x_ - x0_);
  double y = (y_ - y0_);
  double z = (z_ - z0_);
  return x * x + y * y + z * z;
}