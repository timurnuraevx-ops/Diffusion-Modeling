import numpy as np 
import matplotlib.pyplot as plt 
import pandas as pd 


df = pd.read_csv("/home/timurn/ВычФиз/diffusion project/results/results.csv")
R = np.array(df["r**2"])
t = np.array(df["t * 1e9"]) / 1e9
plt.scatter(t, R)

plt.title("Зависимость среднего квадрата радиуса от времени")
plt.xlabel("Время, с")
plt.ylabel("Среднее квадратичное расстояния до центра, м^2")

coeffs = np.polyfit(t, R, deg=1)
print(f"D = {coeffs[0] / 6} м^2 / c")

x = np.linspace(t[0], t[-1], 100)
y = np.polyval(coeffs, x)
plt.plot(x, y)
plt.savefig("plot.png", dpi=150)

n = 2.414323855e+25
sigma = np.pi * (3.6e-10 + 2.2e-10) ** 2 / 4
lambda_ = 1 / (2 ** 0.5 * n * sigma)
v_av = (8 * 8.31 * 300 / (np.pi * 0.004)) ** 0.5

print(f"D теоретически равен {v_av * lambda_ / 3}")