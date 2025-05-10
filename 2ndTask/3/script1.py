import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.table import Table


data = pd.read_csv('results.csv')

data = data.dropna(subset=['Schedule'])

schedules = data['Schedule'].unique()

threads = sorted(data['Threads'].unique())

time_data = {}
speedup_data = {}

for schedule in schedules:
    filtered_data = data[data['Schedule'] == schedule]
    
    single_thread_time = filtered_data[filtered_data['Threads'] == 1]['Time (s)'].values
    if len(single_thread_time) == 0:
        print(f"No single-thread data for schedule: {schedule}")
        continue
    single_thread_time = single_thread_time[0]
    
    time_values = {}
    speedup_values = {}
    
    for thread in threads:
        time = filtered_data[filtered_data['Threads'] == thread]['Time (s)'].values
        if len(time) > 0:
            time_values[str(thread)] = round(time[0], 4) 
            speedup_values[str(thread)] = round(single_thread_time / time[0], 4)  
        else:
            time_values[str(thread)] = None
            speedup_values[str(thread)] = None
    
    time_data[schedule] = time_values
    speedup_data[schedule] = speedup_values

time_df = pd.DataFrame(time_data)
speedup_df = pd.DataFrame(speedup_data)

time_df.index.name = "Количество потоков"
speedup_df.index.name = "Количество потоков"

print("Таблица времени выполнения:")
print(time_df)
print("\nТаблица ускорения:")
print(speedup_df)

fig, ax = plt.subplots(figsize=(16, 8))

ax.axis('off')

combined_data = []
for row in threads:
    combined_row = []
    for col in schedules:
        row_str = str(row) 
        time_val = time_df.at[row_str, col]
        speedup_val = speedup_df.at[row_str, col]
        combined_row.append(f"{time_val}\n({speedup_val})")
    combined_data.append(combined_row)

table = ax.table(
    cellText=combined_data,
    rowLabels=[f"{t}" for t in threads],
    colLabels=schedules,
    cellLoc='center',
    rowLoc='center',
    colLoc='center',
    bbox=[0, 0, 1, 1]  
)

table.auto_set_font_size(False)
table.set_fontsize(8)

for (row, col), cell in table.get_celld().items():
    if (row == 0) or (col == -1):  
        cell.set_facecolor('#D3D3D3')  

plt.savefig('performance_table_with_speedup.png', dpi=300, bbox_inches='tight')

plt.show()