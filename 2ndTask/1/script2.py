import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


csv_file = "results.csv"

data = pd.read_csv(csv_file)

grouped_data = data.groupby(['Threads', 'Matrix Size'])

mean_results = grouped_data[['Runtime (s)', 'Speedup']].mean().reset_index()

print("Average values:", mean_results)

mean_results.to_csv("mean_results.csv", index = False)
print("Mean values saved into nes .csv")

for matrix_size in mean_results['Matrix Size'].unique():
    subset = mean_results[mean_results['Matrix Size'] == matrix_size]
    plt.plot(subset['Threads'], subset['Speedup'], marker = 'o', label = f"{matrix_size}x{matrix_size}")

    plt.xlabel("Num of threads")
    plt.ylabel("Speedup")
    plt.title("Speedup(n)")
    plt.legend()
    plt.grid()
    plt.show()