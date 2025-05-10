import pandas as pd
import matplotlib.pyplot as plt


data = pd.read_csv('results.csv')

data = data.dropna(subset=['Schedule'])

schedules = data['Schedule'].unique()

colors = {
    'static': 'red',
    'auto': 'orange',
    'guided': 'blue',
    'dynamic': 'pink',
    'parallel': 'black'
}

data['Speedup'] = None

for schedule in schedules:
    filtered_data = data[data['Schedule'] == schedule]
    
    if filtered_data.empty:
        print(f"No data for schedule: {schedule}")
        continue
    
    single_thread_time = filtered_data[filtered_data['Threads'] == 1]['Time (s)'].values
    if len(single_thread_time) == 0:
        print(f"No single-thread data for schedule: {schedule}")
        continue
    single_thread_time = single_thread_time[0]
    
    data.loc[data['Schedule'] == schedule, 'Speedup'] = single_thread_time / filtered_data['Time (s)']

plt.figure(figsize=(10, 6))

for schedule in schedules:
    filtered_data = data[data['Schedule'] == schedule]
    
    filtered_data = filtered_data.sort_values(by='Threads')
    
    threads = filtered_data['Threads']
    speedup = filtered_data['Speedup']
    
    plt.plot(threads, speedup, marker='o', label=schedule, color=colors.get(schedule, 'black'))

plt.title('Scheduling Methods')
plt.xlabel('Number of Threads')
plt.ylabel('Speedup (T1/Tp)')
plt.legend(title='Scheduling Method')
plt.grid(True)
plt.tight_layout()

plt.savefig('speedup.png')

plt.show()