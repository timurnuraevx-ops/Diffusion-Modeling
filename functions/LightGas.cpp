#include "LightGas.hpp"
#include "Particle.hpp"
#include <nlohmann/json.hpp>
#include <fstream>

LightGas::LightGas(std::vector<Particle> particles, double dt, BackgroundGas gas, double t) 
                    : particles_(particles), dt(dt), gas_(gas), t(t) {
}

LightGas LightGas::InitPointSource(const std::string& json_file_name) {
  std::ifstream file(json_file_name);

  nlohmann::json j;
  file >> j;
  BackgroundGas gas = BackgroundGas::FromJson(json_file_name);

  double dt = j["Constants"].value("dt", 1e-12);
  int32_t N = j["Constants"].value("N", 100);
  std::vector<Particle> particles(N);
  for (int32_t i = 0; i < N; i++) {
    particles[i] = Particle::InitPointSource(json_file_name);
  }
  return LightGas(particles, dt, gas, 0.0);
} 

void LightGas::Update() {
  for (size_t i = 0; i < particles_.size(); i++) {
    particles_[i].Update(gas_, dt);
  }
  t += dt;
}

double LightGas::GetAverageQuadraticRadius() {
  double r = 0;
  for (auto& p : particles_) {
    r += p.GetQuadraticRadius();
  }
  r /= static_cast<double>(particles_.size());
  return r;
}

void LightGas::LaunchPointSourceSimulation(const std::string& filename, const std::string& json_file_name) {
  std::ifstream jsonfile(json_file_name);

  nlohmann::json j;
  jsonfile >> j;

  int32_t iterations = j["Constants"].value("iterations", 10000);

  std::ofstream file(filename);

  file << "r**2,t * 1e9\n";
  file.flush();

  for (int32_t i = 0; i < iterations; i++) {
    Update();
    file << GetAverageQuadraticRadius() << "," << t * 1e9 << "\n";
  }

  file.close();
}