import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

csv_file = "results.csv"

data = pd.read_csv(csv_file)

grouped_data = data.groupby('Threads')[['Runtime (s)', 'Speedup']].mean().reset_index()

print("Average values:")
print(grouped_data)

grouped_data.to_csv("mean_results.csv", index=False)
print("Mean values saved into new .csv")

plt.figure(figsize=(10, 6))

plt.plot(grouped_data['Threads'], grouped_data['Speedup'], marker='o', label="Speedup")

plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.title("Speedup vs Number of Threads")
plt.legend()
plt.grid()

plt.savefig("speedup_graph.png", bbox_inches="tight", dpi=300)
print("Graph saved as speedup_graph.png")

plt.show()