import numpy as np 
import matplotlib.pyplot as plt 
import pandas as pd 
import json
import os
import subprocess

class DiffusionAnalytic:
    def __init__(self, json_file_name: str, results_file_name: str):
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
        self.d_light = self.config["Light gas"]["d"]
        self.d_back = self.config["BackgroundGas"]["d"]
        self.T_back = self.config["BackgroundGas"]["T"]
        self.T_light = self.config["Light gas"]["T"]
        self.n = self.config["BackgroundGas"]["n"]
    
    @property
    def TheorD(self):
        sigma = np.pi * (self.d_back + self.d_light) ** 2
        v_av = (2 * np.pi * self.R * self.T_back / ((self.mu_back * self.mu_light) / (self.mu_light + self.mu_back))) ** 0.5
        return v_av * 3 / (sigma * self.n * 8)
    
    def TimeRadiusGraph(self, graphic_name: str):
        plt.scatter(self.t, self.average_quadratic_radius)
        plt.title("График среднего квадратического расстояния, пройденного молекулами от времени")
        plt.xlabel("Время, с")
        plt.ylabel("Среднее квалратическое расстояние м^2")
        coeffs = np.polyfit(self.t, self.average_quadratic_radius, 1)
        x = np.linspace(self.t[0], self.t[-1], 100)
        y = np.polyval(coeffs, x)
        plt.plot(x, y)

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
    
    @property
    def GetD(self):
        coeffs = np.polyfit(self.t, self.average_quadratic_radius, 1)
        return coeffs[0] / 6
    
    def DiffusionTemperatureGraph(self, T_final: float, n: int, cpp_file_name: str,
                                json_file_name: str, results_file_name: str, graphic_name: str):
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        T = np.linspace(self.T_back, T_final, n)
        D = []
        for t in T:
            self.config["BackgroundGas"]["T"] = t
        
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
        plt.scatter(T, D)

        coeffs = np.polyfit(T, D, 1)
        x = np.linspace(T[0], T[-1], 100)
        y = np.polyval(coeffs, x)

        plt.plot(x, y)
        plt.title("График зависимости коэффициента диффузии от температуры")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Температура, К")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)
        return coeffs[0]
    
    def DiffusionNGraph(self, n_final: str, N: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
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
    
    def DiffusionMuGraph(self, mu_final: str, n: int, cpp_file_name: str,
                        json_file_name: str, results_file_name: str, graphic_name: str):
        folder = "../build"
        full_path = os.path.join(folder, cpp_file_name)

        mu = np.linspace(self.mu_back, mu_final, n)
        D = []
        for i in mu:
            self.config["BackgroundGas"]["mu"] = i
        
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
        plt.plot(mu, D)

        plt.title("График зависимости коэффициента диффузии от молярной массы фонового газа")
        plt.ylabel("Коэффициент диффузии")
        plt.xlabel("Молярная масса, кг/моль")

        folder = 'graphics/'
        full_path = os.path.join(folder, graphic_name)
        plt.savefig(full_path)

diff = DiffusionAnalytic("config.json", "results.csv")
print(diff.DiffusionMuGraph(0.5, 11, "main", "config.json", "results.csv", "график диффузии от молярной массы"))
