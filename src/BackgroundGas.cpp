#include "BackgroundGas.hpp"
#include <fstream>
#include <random>
#include <string>
#include <nlohmann/json.hpp>

BackgroundGas::BackgroundGas(double T, double mu, double n, double R) : T_(T), mu_(mu), n_(n), R_(R) {
}

BackgroundGas BackgroundGas::FromJson(const std::string& json_file_path) {
  std::ifstream file(json_file_path);

  nlohmann::json j;
  file >> j;

  double T = j["BackgroundGas"].value("T", 300.0);
  double mu = j["BackgroundGas"].value("mu", 0.029);
  double n = j["BackgroundGas"].value("n", 2.414323855e+25);
  double R = j["Constants"].value("R", 8.31);
    
  return BackgroundGas(T, mu, n, R);
}

double BackgroundGas::GetT() const {
  return T_;
}

double BackgroundGas::GetMu() const {
  return mu_;
}

double BackgroundGas::GetN() const {
  return n_;
}

double BackgroundGas::GetVelocityComp() const {
  static thread_local std::mt19937 gen(std::random_device{}());
  static thread_local std::normal_distribution<> dist(0.0, 1.0);

  double scale = std::pow(R_ * T_ / mu_, 0.5);
  return dist(gen) * scale;
}