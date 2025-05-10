import pandas as pd
import matplotlib.pyplot as plt


csv_file = "mean_results.csv"

data = pd.read_csv(csv_file)

grouped_data = data.groupby('Threads')[['Runtime (s)', 'Speedup']].mean().reset_index()

threads = grouped_data['Threads']
runtime_data = grouped_data['Runtime (s)']
speedup_data = grouped_data['Speedup']

table_data = []
table_data.append(["T_p"] + [f"{val:.2f}" for val in runtime_data])
table_data.append(["S_p"] + [f"{val:.2f}" for val in speedup_data])

headers = ["Количество потоков / Показатели"] + [str(t) for t in threads]

fig, ax = plt.subplots(figsize=(24, 4))
ax.axis('tight')
ax.axis('off')

table = ax.table(cellText=table_data, colLabels=headers, loc="center", cellLoc="center")
table.auto_set_font_size(False)
table.set_fontsize(12)
table.scale(1.5, 1.5)
plt.title("Таблица времени выполнения и ускорения", fontsize=16)
plt.savefig("formatted_table.png", bbox_inches="tight", dpi=300)
print("Таблица сохранена как formatted_table.png")

plt.figure(figsize=(10, 6))
plt.plot(grouped_data['Threads'], grouped_data['Speedup'], marker='o', label="Ускорение")
plt.plot(grouped_data['Threads'], grouped_data['Threads'], label="y = x", linestyle='--')

plt.xlabel("Количество потоков", fontsize=14)
plt.ylabel("Ускорение (S_p)", fontsize=14)
plt.title("Ускорение в зависимости от количества потоков", fontsize=16)
plt.legend(fontsize=12)
plt.grid(True)


plt.savefig("speedup_graph.png", bbox_inches="tight", dpi=300)
print("График сохранен как speedup_graph.png")

plt.show()