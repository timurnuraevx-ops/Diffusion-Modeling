#pragma once
#include "Particle.hpp"
#include "BackgroundGas.hpp"
#include <string>
#include <vector>

class LightGas {
 private: 
  std::vector<Particle> particles_;
  BackgroundGas gas_;

 public:
  double dt = 0.0;
  double t = 0.0;

  LightGas() = default;
  LightGas(std::vector<Particle>, double, BackgroundGas, double);

  static LightGas InitSource(const std::string&);
  void SaveToVTP(const std::string& filename) const;
  void Update();
  double GetAverageQuadraticRadius();
  void LaunchSimulation(const std::string&, const std::string&, const std::string&);
};