import numpy as np 
import matplotlib.pyplot as plt 
import pandas as pd 
import json
import os

class DiffusionAnalytic:
    def __init__(self, json_file_name: str, results_file_name: str):
        folder_json = "../config"
        full_path_json = os.path.join(folder_json, json_file_name)

        folder_res = "../results"
        full_path_res = os.path.join(folder_res, results_file_name)

        df = pd.read_csv(full_path_res)
        with open(full_path_json, 'r', encoding='utf-8') as f:
            self.config = json.load(f)
        self.json_file_name = json_file_name
        self.results_file_name = results_file_name

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

if __name__ == 'main':
    diff = DiffusionAnalytic("config.json", "results.csv")
    diff.TimeRadiusGraph("график 1")
    print(diff.GetD, diff.TheorD)
