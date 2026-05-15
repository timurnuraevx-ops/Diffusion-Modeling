#pragma once
#include <string>

class BackgroundGas {
  /*
  Класс реализует фоновый газ.
  Поля класса:
      T_ - температура газа
      mu_ - молярная масса
      n_ - концентрация
      R_ - универсальная газовая постоянная
  */
 private:
  double T_ = 0.0;
  double mu_ = 0.0;
  double n_ = 0.0;
  double R_ = 0.0;

 public:
  /*
  Конструктор класса от параметров
  */
  BackgroundGas(double, double, double, double);
  /*
  Конструктор по умолчанию
  */
  BackgroundGas() = default;

  /*
  Конструктор от конфиг файла
  */
  static BackgroundGas FromJson(const std::string&);

  /*
  Геттеры
  */
  double GetT() const;
  double GetMu() const;
  double GetN() const;

  /*
  Возвращает случаную компоненту скорости по координате
  используя максвелловское распределение
  */
  double GetVelocityComp() const;
};