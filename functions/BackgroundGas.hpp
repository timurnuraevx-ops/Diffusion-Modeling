#pragma once
#include <string>

class BackgroundGas {
 private:
  double T_ = 0.0;
  double mu_ = 0.0;
  double n_ = 0.0;
  double d_ = 0.0;

 public:
  double R = 0.0;
  BackgroundGas(double, double, double, double, double);
  BackgroundGas() = default;

  static BackgroundGas FromJson(const std::string&);

  double GetT() const;
  double GetMu() const;
  double GetN() const;
  double GetD() const;

  double GetVelocityComp() const;
};