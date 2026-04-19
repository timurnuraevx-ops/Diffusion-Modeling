#include "BackgroundGas.hpp"
#include "LightGas.hpp"
#include "Particle.hpp"
#include <iostream>
#include <string>


int main() {
  LightGas helium = LightGas::InitSource("/home/timurn/ВычФиз/diffusion project/config/config.json");
  helium.LaunchSimulation("/home/timurn/ВычФиз/diffusion project/results/results.csv", 
                                    "/home/timurn/ВычФиз/diffusion project/config/config.json",
                                    "/home/timurn/ВычФиз/diffusion project/results/vtk/");
  return 0;
}