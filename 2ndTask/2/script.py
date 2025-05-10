import subprocess
import os


executable_path = "./main"

num_runs = int(input("Enter the number of runs: "))

for run in range(num_runs):
    try:
        print(f"Running program (Run {run + 1}/{num_runs})...")
        result = subprocess.run([executable_path], check=True, text=True, capture_output=True)
        print(f"Program output for Run {run + 1}:")
        print(result.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error running the program on Run {run + 1}: {e}")
        print(f"Stderr: {e.stderr}")