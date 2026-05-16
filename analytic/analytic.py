import numpy as np 
import matplotlib.pyplot as plt 
import pandas as pd 
import json
import os
import subprocess

class DiffusionAnalytic:
    '''
    Класс реализует анализ и построение графиков готовой модели
    '''
    def __init__(self, json_file_name: str, results_file_name: str):
        '''
        Инициализация:
            Аргументы: 
                json_file_name - название конфиг файла
                results_file_name - файл с итоговыми результатами модели
            
            Поля класса совпадают с полями конфиг файла
        '''
        folder_json = "../config"
        full_path_json = os.path.join(folder_json, json_file_name)

        folder_res = "../results"
        full_path_res = os.path.join(folder_res, results_file_name)

        df = pd.read_csv(full_path_res)
        with open(full_path_json, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        self.json_file = full_path_json
        self.results_file = full_path_res

        self.average_quadratic_radius = np.array(df["r**2"])
        self.t = np.array(df["t * 1e9"])
        self.t /= 1e9
        self.mu_light = self.config["Light gas"]["mu"]
        self.mu_back = self.config["BackgroundGas"]["mu"]
        self.R = self.config["Constants"]["R"]
        self.T_back = self.config["BackgroundGas"]["T"]
        self.T_light = self.config["Light gas"]["T"]
        self.n = self.config["BackgroundGas"]["n"]
        self.T0 = self.config["gas-kinetic cross-section"]["T0"]
        self.omega = self.config["gas-kinetic cross-section"]["omega"]
        self.sigma0 = self.config["gas-kinetic cross-section"]["sigma0"]
    
    @property
    def TheorD(self):
        '''
        Метод возвращает теоретическое значение коэффициента диффузии
        '''
        T = self.T_back
        mu_red = self.mu_back * self.mu_light / (self.mu_back + self.mu_light)
        sigma = self.sigma0 * np.power(self.T0 / T, self.omega)
        return np.pow(self.R * T / (3 * mu_red), 0.5) / (self.n * sigma) /2
    
    def TimeRadiusGraph(self, graphic_name: str):
        '''
        Класс строит график зависимости средне-квадратического расстояния,
        пройденного молекулами от времени. График сохраняется в /graphics
        Аргументы: 
            graphic_name - итоговое имя графика
        '''
        plt.scatter(self.t, self.average_quadratic_radius)
        plt.title("График среднего квадратического расстояния, пройденного молекулами от времени")
        plt.xlabel("Время, с")
        plt.ylabel("Среднее квадратическое расстояние м^2")
        coeffs = np.polyfit(self.t, self.average_quadratic_radius, 1)
        x = np.linspace(self.t[0], self.t[-1], 100)
        y = np.polyval(coeffs, x)
        plt.plot(x, y)

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)

    @property
    def GetD(self):
        '''
        Возвращает итоговый коэффициент диффузии, посчитанный через
        коэффициент наклона прямой в графике зависимости r**2(t)
        '''
        coeffs = np.polyfit(self.t, self.average_quadratic_radius, 1)
        return coeffs[0] / 6
    
    def DiffusionTemperatureGraph(self, T_final: float, n: int, cpp_file_name: str,
                                json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Меняет конфиг файл, начинает симуляцию, после чего записывает
        коэффициент диффузии и строит график зависимости коэффициента диффузии от температуры
        сохраняет график в graphics
        Аргументы: 
            T_final - итоговая температура
            n - количество точек
            cpp_file_name - имя бинарника
            json_file_name - имя конфиг файла
            results_file_name - файл с итоговыми точками
            graphic_name - название графика
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        T = np.linspace(self.T_back, T_final, n)
        D = []
        for t in T:
            self.config["BackgroundGas"]["T"] = t
            self.config["Light gas"]["T"] = t
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        D = np.array(D)
        plt.plot(T, D)

        plt.title("График зависимости коэффициента диффузии от температуры")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Температура, К")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
    
    def LinearDiffusionTemperatureGraph(self, T_final: float, n: int, cpp_file_name: str,
                                json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Аналогичен предыдущему пункту, но на этот раз линеаризует
        график коэффициента диффузии от температуры, D ~ T ** (3/2 + omega)
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        T = np.linspace(self.T_back, T_final, n)
        D = []
        for t in T:
            self.config["BackgroundGas"]["T"] = t
            self.config["Light gas"]["T"] = t
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        D = np.array(D)
        T = np.power(T, 0.5 + self.omega)
        plt.scatter(T, D)

        coeffs = np.polyfit(T, D, 1)
        x = np.linspace(T[0], T[-1], 100)
        y = np.polyval(coeffs, x)

        plt.plot(x, y)
        plt.title("График зависимости коэффициента диффузии от корня температуры")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("корень температуры, К ** 0.5")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coeffs[0]

    def LogarifmicDiffusionTemperatureGraph(self, T_final: float, n: int, cpp_file_name: str,
                                json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Аналогичен предыдущему пункту, но на этот раз линеаризует
        график коэффициента диффузии от температуры, ln(D) ~ ln(T)
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        T = np.linspace(self.T_back, T_final, n)
        D = []
        for t in T:
            self.config["BackgroundGas"]["T"] = t
            self.config["Light gas"]["T"] = t
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        D = np.array(D)
        T = np.log(T)
        D = np.log(D)
        plt.scatter(T, D)

        coeffs = np.polyfit(T, D, 1)
        x = np.linspace(T[0], T[-1], 100)
        y = np.polyval(coeffs, x)

        plt.plot(x, y)
        plt.title("График зависимости ln(D)(ln(T))")
        plt.ylabel("ln(D)")
        plt.xlabel("ln(T)")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coeffs[0]

    def DiffusionNGraph(self, n_final: float, N: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Строит график зависимости коэффициента диффузии от концентрации
        Все остальное аналогично предыдущему методу
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        n = np.linspace(self.n, n_final, N)
        D = []
        for i in n:
            self.config["BackgroundGas"]["n"] = i
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        D = np.array(D)
        plt.plot(n, D)

        plt.title("График зависимости коэффициента диффузии от концентрации")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Концентрация, м^-3")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
    
    def LinearDiffusionNGraph(self, n_final: float, N: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Метод аналогичен предыдущему, но линеаризует зависимость 
        диффузии от концентрации как D(1/n), возвращает коэффициент наклона
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        n = np.linspace(self.n, n_final, N)
        D = []
        for i in n:
            self.config["BackgroundGas"]["n"] = i
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        n = np.power(n, -1)
        D = np.array(D)
        plt.scatter(n, D)

        coeffs = np.polyfit(n, D, 1)
        x = np.linspace(n[0], n[-1], 100)
        y = np.polyval(coeffs, x)
        plt.plot(x, y)

        plt.title("График зависимости коэффициента диффузии от концентрации")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Обратная концентрация, м^3")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coeffs[0]

    def LogarifmicDiffusionNGraph(self, n_final: float, N: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Метод аналогичен предыдущему, но линеаризует зависимость 
        диффузии от концентрации как ln(D)(ln(n)), возвращает коэффициент наклона
        '''

        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        n = np.linspace(self.n, n_final, N)
        D = []
        for i in n:
            self.config["BackgroundGas"]["n"] = i
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        n = np.log(n)
        D = np.array(D)
        D = np.log(D)
        plt.scatter(n, D)

        coeffs = np.polyfit(n, D, 1)
        x = np.linspace(n[0], n[-1], 100)
        y = np.polyval(coeffs, x)
        plt.plot(x, y)

        plt.title("График зависимости ln(D)(ln(n))")
        plt.ylabel("ln(D)")
        plt.xlabel("ln(n)")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coeffs[0]

    def DiffusionMuGraph(self, mu_final: float, n: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Аналогично предыдущим методам, только строит 
        график зависимости коэффициента диффузии от приведенной массы газа
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        mu = np.linspace(self.mu_light, mu_final, n)
        D = []
        for i in mu:
            self.config["Light gas"]["mu"] = i

            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        mu = mu * self.mu_back / (self.mu_back + mu)
        D = np.array(D)
        plt.plot(mu, D)

        plt.title("График зависимости коэффициента диффузии от молярной массы")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Молярная масса, кг/моль")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)

    def LogarifmicDiffusionMuGraph(self, mu_final: float, n: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        '''
        Аналогично предыдущим методам, только строит 
        график зависимости ln(D)(ln(mu))
        '''
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        mu = np.linspace(self.mu_light, mu_final, n)
        D = []
        for i in mu:
            self.config["Light gas"]["mu"] = i
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        mu = mu * self.mu_back / (self.mu_back + mu)
        D = np.array(D)
        D = np.log(D)
        mu = np.log(mu)
        plt.scatter(mu, D)
        x = np.linspace(mu[0], mu[-1], 100)
        coefs = np.polyfit(mu, D, 1)
        y = np.polyval(coefs, x)
        plt.plot(x, y)
        plt.title("График зависимости ln(D)(ln(mu))")
        plt.ylabel("ln(D)")
        plt.xlabel("ln(mu)")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coefs[0]

    def LinearDiffusionMuGraph(self, mu_final: float, n: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        mu = np.linspace(self.mu_light, mu_final, n)
        D = []
        for i in mu:
            self.config["Light gas"]["mu"] = i
        
            with open(self.json_file, 'w') as f:
                json.dump(self.config, f, indent=4)
            
            result = subprocess.run(
                [full_path],
                capture_output=True,
                text=True
            )
            self.__init__(json_file_name, results_file_name)
            D.append(self.GetD)
        
        mu = mu * self.mu_back / (self.mu_back + mu)
        D = np.array(D)
        mu = np.power(mu, -0.5)
        plt.scatter(mu, D)
        x = np.linspace(mu[0], mu[-1], 100)
        coefs = np.polyfit(mu, D, 1)
        y = np.polyval(coefs, x)
        plt.plot(x, y)
        plt.title("График зависимости (D)(1/mu^0.5)")
        plt.ylabel("D")
        plt.xlabel("mu^-0.5")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coefs[0]

diff = DiffusionAnalytic("config.json", "results.csv")
print(diff.LogarifmicDiffusionMuGraph(0.3, 10, "main", "config.json", "results.csv", "График зависимости D(mu0.5).png"))
