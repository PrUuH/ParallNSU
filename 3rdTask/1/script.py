import pandas as pd
import matplotlib.pyplot as plt


csv_file = "results.csv"

data = pd.read_csv(csv_file)

grouped_data = data.groupby(['Threads', 'Matrix Size'])[['Runtime', 'SpeedUp']].mean().reset_index()

pivot_runtime = grouped_data.pivot(index='Matrix Size', columns='Threads', values='Runtime')
pivot_speedup = grouped_data.pivot(index='Matrix Size', columns='Threads', values='SpeedUp')

threads = pivot_runtime.columns
matrix_sizes = pivot_runtime.index

table_data = []
for size in matrix_sizes:
    row_runtime = [f"{size} (T_p)"] + [f"{val:.2f}" for val in pivot_runtime.loc[size]]
    row_speedup = [""] + [f"{val:.2f}" for val in pivot_speedup.loc[size]]
    table_data.append(row_runtime)
    table_data.append(row_speedup)

headers = ["Размер матрицы / Потоки"] + [str(t) for t in threads]

fig, ax = plt.subplots(figsize=(20, 6))
ax.axis('tight')
ax.axis('off')

table = ax.table(cellText=table_data, colLabels=headers, loc="center", cellLoc="center")
table.auto_set_font_size(False)
table.set_fontsize(12)
table.scale(1.5, 1.5)
plt.title("Таблица времени выполнения и ускорения", fontsize=16)
plt.savefig("formatted_table_combined.png", bbox_inches="tight", dpi=300)
print("Таблица сохранена как formatted_table_combined.png")

plt.figure(figsize=(10, 6))

for size in matrix_sizes:
    subset = grouped_data[grouped_data['Matrix Size'] == size]
    plt.plot(subset['Threads'], subset['SpeedUp'], marker='o', label=f"{size}x{size}")

plt.plot(threads, threads, linestyle='--', color='black', label="y = x")

plt.xlabel("Количество потоков", fontsize=14)
plt.ylabel("Ускорение (S_p)", fontsize=14)
plt.title("Ускорение в зависимости от количества потоков", fontsize=16)
plt.legend(title="Размер матрицы", fontsize=12, title_fontsize=14)
plt.grid(True)

plt.savefig("speedup_graph_combined.png", bbox_inches="tight", dpi=300)
print("График сохранен как speedup_graph_combined.png")

plt.show()