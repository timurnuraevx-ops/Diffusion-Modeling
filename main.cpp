#include "BackgroundGas.hpp"
#include "LightGas.hpp"
#include "Particle.hpp"
#include <iostream>
#include <string>


int main() {
  LightGas helium = LightGas::InitPointSource("/home/timurn/ВычФиз/diffusion project/config/config.json");
  helium.LaunchPointSourceSimulation("/home/timurn/ВычФиз/diffusion project/results/results.csv", 
                                    "/home/timurn/ВычФиз/diffusion project/config/config.json");
  return 0;
}