#pragma once
#include <string>
#include "BackgroundGas.hpp"
#include <nlohmann/json.hpp>
#include <vector>

class Particle {
 private:
  double mu_ = 0.0;
  double d_ = 0.0;

  double x_ = 0.0;
  double y_ = 0.0;
  double z_ = 0.0;

  double v_x_ = 0.0;
  double v_y_ = 0.0;
  double v_z_ = 0.0;

  double T_ = 0.0;

 public:
  double R = 0.0;

  Particle(double, double, double, double, double, double, double, double, double, double);
  Particle() = default;
  static Particle InitPointSource(const std::string&);

  double GetQuadraticRadius() const;
  void Collision(const BackgroundGas&);
  void InitializeVelocitities();
  bool CheckCollision(const BackgroundGas&, double);

  void Update(const BackgroundGas&, double);
};