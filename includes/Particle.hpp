#pragma once
#include <string>
#include "BackgroundGas.hpp"
#include <nlohmann/json.hpp>
#include <vector>
#include <optional>

/*
Класс реализует одну частицу молекулы легкого газа
Поля:
    mu_ - молярная масса молекулы
    d_ - диаметр молекулы
    x_, y_, z_ - координаты
    x0_, y0_, z0_ - начальные координаты
    v_x_, v_y_, v_z_ - координаты скоростей молекул
    T_, R_ - температура и универсальная газовая постоянная
    sigma_ - газокинетическое сечение
    sigma0_ - сечение при референсной температуре
    omega_, T0_ - коэффициенты для рассчета газокинетического сечения
*/
class Particle {
 private:
  double mu_ = 0.0;

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

  double sigma_ = 0.0;
  double sigma0_ = 0.0;
  double T0_ = 0.0;
  double omega_ = 0.0;

 public:

  /*
  Параметрический конструктор
  */
  Particle(double, double, double, double, double, double,
          double, double, double, double, double, double, double, double, double, double);
 
  /*
  Конструктор по умолчанию
  */
  Particle() = default;
  /*
  Конструктор который в зависимости от источника 
  генерирует начальное положение молекулы
  */
  static Particle InitSource(double, double, double, double, double, double, double, double, double, double);

  /*
  Геттеры
  */
  double GetVelocityModule() const;
  double GetX() const;
  double GetY() const;
  double GetZ() const;
  double GetVx() const;
  double GetVy() const;
  double GetVz() const;
  double GetQuadraticRadius() const;

  /*
  Метод принимает фоновый газ и просчитывает столкновение,
  меняет координаты по скоростям
  */
  void Collision(const BackgroundGas&, std::array<double, 3>);

  /*
  Инициализирует начальные компоненты скоростей молекул
  случайным образом исходя из максвеллевского распределения
  */
  void InitializeVelocitities();

  /*
  Случайным образом просчитывает произошло ли столкновение с фоновым газом
  Аргументы:
      gas - фоновый газ
      dt - шаг по времени
  */
  std::optional<std::array<double, 3>> CheckCollision(const BackgroundGas&, double);
  /*
  Обновляет координаты молекулы, проверяет было ли столкновение
  и если было то меняет скорость молекулы
  */
  void Update(const BackgroundGas&, double);
};