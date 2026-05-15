#pragma once
#include "Particle.hpp"
#include "BackgroundGas.hpp"
#include <string>
#include <vector>

/*
Класс реализует разреженный газ как массив легких частиц
движущихся и сталкиващихся с фоновым газом со временем
Аргументы:
    particles_ - вектор частиц
    gas_ - фоновый газ
    dt - шаг по времени
    t - время
*/

class LightGas {
 private: 
  std::vector<Particle> particles_;
  BackgroundGas gas_;

 public:
  double dt = 0.0;
  double t = 0.0;

  /*
  Конструктор по умолчанию
  */
  LightGas() = default;
  /*
  Параметрический конструктор
  */
  LightGas(std::vector<Particle>, double, BackgroundGas, double);

  /*
  Инициализирует источник молекул из конфиг файла
  Flat source - плоский источник
  Point source - точеченый источник
  Volume source - объемный источник
  Во всех источниках молекулы распределены равномерно по площади/объему
  */
  static LightGas InitSource(const std::string&);

  /*
  Метод сохраняет координаты точек и скорости в формате vtp,
  чтобы потом запускать симуляцию в paraview
  */
  void SaveToVTP(const std::string& filename) const;

  /*
  Метод обновляет координаты молекул,
  смотрит было ли столкновение и если было, меняет компоненты скоростей
  */
  void Update();

  /*
  Возвращает среднее квадратичное расстояние,
  пройденное всеми молекулами
  */
  double GetAverageQuadraticRadius();

  /*
  Запускает симуляцию
  Аргументы:
        filename - имя файлов которые сохраняем в vtp
        json_file_name - конфиг файл
        vtk_path - путь до папки куда сохраняем vtp-файлы
  */
  void LaunchSimulation(const std::string&, const std::string&, const std::string&);
};