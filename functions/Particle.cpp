#include "Particle.hpp"
#include <fstream>
#include <random>
#include <string>
#include <nlohmann/json.hpp>
#include <cmath>

Particle::Particle(double mu, double d, double x, double y, double z,
                        double v_x, double v_y, double v_z, double T, double R) :
                        mu_(mu), d_(d), x_(x), y_(y), z_(z), v_x_(v_x), v_y_(v_y), v_z_(v_z), T_(T), R(R) {
}

Particle Particle::InitPointSource(const std::string& json_file_path) {
  std::ifstream file(json_file_path);

  nlohmann::json j;
  file >> j;

  double mu = j["Light gas"].value("mu", 0.004);
  double T = j["Light gas"].value("T", 300);
  double R = j["Constants"].value("R", 8.31);
  double d = j["Light gas"].value("d", 2.2e-10);

  Particle p(mu, d, 0, 0, 0, 0, 0, 0, T, R);
  p.InitializeVelocitities();
  return p;
}

void Particle::InitializeVelocitities() {
  std::random_device rd;
  std::mt19937 gen(rd());

  double scale = std::pow(R * T_ / mu_, 0.5);
  std::normal_distribution<> dist(0.0, scale);
  v_x_ = dist(gen);
  v_y_ = dist(gen);
  v_z_ = dist(gen);
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


void Particle::Collision(const BackgroundGas& gas) {
  double back_v_x = gas.GetVelocityComp();
  double back_v_y = gas.GetVelocityComp();
  double back_v_z = gas.GetVelocityComp();

  double center_v_x = (v_x_ * mu_ + back_v_x * gas.GetMu()) / (mu_ + gas.GetMu());
  double center_v_y = (v_y_ * mu_ + back_v_y * gas.GetMu()) / (mu_ + gas.GetMu());
  double center_v_z = (v_z_ * mu_ + back_v_z * gas.GetMu()) / (mu_ + gas.GetMu());

  v_x_ = 2 * center_v_x - v_x_;
  v_y_ = 2 * center_v_y - v_y_;
  v_z_ = 2 * center_v_z - v_z_;
}

bool Particle::CheckCollision(const BackgroundGas& gas, double dt) {
  std::random_device rd;
  std::mt19937 gen(rd());

  double v_abs = (v_x_ * v_x_ + v_y_ * v_y_ + v_z_ * v_z_);
  v_abs = std::pow(v_abs, 0.5);
  double sigma = 3.141592 * std::pow(d_ + gas.GetD(), 2) / 4;
  double P = gas.GetN() * sigma * v_abs * dt;
  std::uniform_real_distribution<> dist(0.0, 1.0);
  double r = dist(gen);
    
  return r < P;
}

void Particle::Update(const BackgroundGas& gas, double dt) {
  x_ += v_x_ * dt;
  y_ += v_y_ * dt;
  z_ += v_z_ * dt;
  if (CheckCollision(gas, dt)) {
    Collision(gas);
  }
}

double Particle::GetQuadraticRadius() const {
  return x_ * x_ + y_ * y_ + z_ * z_;
}