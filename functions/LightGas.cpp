#include "LightGas.hpp"
#include "Particle.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>  

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkFloatArray.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>

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

void LightGas::LaunchPointSourceSimulation(const std::string& filename, const std::string& json_file_name,
                                          const std::string& vtk_path) {
  std::ifstream jsonfile(json_file_name);

  nlohmann::json j;
  jsonfile >> j;

  int32_t iterations = j["Constants"].value("iterations", 10000);
  int32_t step = j["Constants"].value("Step", 100);

  std::ofstream file(filename);

  file << "r**2,t * 1e9\n";
  file.flush();

  for (int32_t i = 0; i < iterations; i++) {
    Update();
    file << GetAverageQuadraticRadius() << "," << t * 1e9 << "\n";
    if ((i + 1) % step == 0) {
      std::ostringstream oss;
      oss << vtk_path << "diffusion_" << std::setw(5) << std::setfill('0') << (i / step) << ".vtp";
      SaveToVTP(oss.str());
    }
  }

  file.close();
}

void LightGas::SaveToVTP(const std::string& filename) const {
  size_t N = particles_.size();
  
  auto points = vtkSmartPointer<vtkPoints>::New();
  points->SetDataType(VTK_DOUBLE);
  points->SetNumberOfPoints(N);

  auto vel = vtkSmartPointer<vtkFloatArray>::New();
  vel->SetName("velocity");
  vel->SetNumberOfComponents(3);
  vel->SetNumberOfTuples(N);

  auto speed = vtkSmartPointer<vtkFloatArray>::New();
  speed->SetName("speed");
  speed->SetNumberOfComponents(1);
  speed->SetNumberOfTuples(N);

  double px, py, pz, vx, vy, vz;
  for (size_t i = 0; i < N; ++i) {
    px = particles_[i].GetX();
    py = particles_[i].GetY();
    pz = particles_[i].GetZ();
    vx = particles_[i].GetVx();
    vy = particles_[i].GetVy();
    vz = particles_[i].GetVz();
    points->SetPoint(i, px, py, pz);
    vel->SetTuple3(i, vx, vy, vz);
    speed->SetTuple1(i, std::sqrt(vx*vx + vy*vy + vz*vz));
  }

  auto poly = vtkSmartPointer<vtkPolyData>::New();
  poly->SetPoints(points);
  poly->GetPointData()->AddArray(vel);
  poly->GetPointData()->SetActiveVectors("velocity");
  poly->GetPointData()->AddArray(speed);

  auto verts = vtkSmartPointer<vtkCellArray>::New();
  for (vtkIdType i = 0; i < N; ++i) {
      verts->InsertNextCell(1, &i);
  }
  poly->SetVerts(verts);

  auto writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
  writer->SetFileName(filename.c_str());
  writer->SetInputData(poly);
  writer->SetDataModeToBinary();
  writer->SetCompressorTypeToZLib();
  writer->Write();
}