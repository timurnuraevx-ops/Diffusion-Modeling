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

  double x0_ = 0.0;
  double y0_ = 0.0;
  double z0_ = 0.0;

  double v_x_ = 0.0;
  double v_y_ = 0.0;
  double v_z_ = 0.0;

  double T_ = 0.0;
  double R_ = 0.0;

 public:

  Particle(double, double, double, double, double, double,
          double, double, double, double, double, double, double);
  Particle() = default;
  static Particle InitSource(double, double, double, double, double, double, double);

  double GetVelocityModule() const;
  double GetX() const;
  double GetY() const;
  double GetZ() const;
  double GetVx() const;
  double GetVy() const;
  double GetVz() const;

  double GetQuadraticRadius() const;
  void Collision(const BackgroundGas&);
  void InitializeVelocitities();
  bool CheckCollision(const BackgroundGas&, double) const;

  void Update(const BackgroundGas&, double);
};