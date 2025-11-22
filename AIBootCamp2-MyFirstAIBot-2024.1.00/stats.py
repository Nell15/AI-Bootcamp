import numpy as np
import matplotlib.pyplot as plt

def analyze_numbers(file_path):
    # load all numbers as floats (ignores whitespace)
    data = np.loadtxt(file_path)

    min_val = int(data.min())
    max_val = int(data.max())
    print(f"Minimum: {min_val}")
    print(f"Maximum: {max_val}")
    print(f"Mean: {data.mean()}")

    n : int = 1000
    # Count values under n
    count_under_n = np.sum(data < n)
    total_count = data.size
    percentage = (count_under_n / total_count) * 100
    print(f"Percentage of values under {n}: {percentage:.2f}%")

    # Plot histogram (reduced, from min to 1000, bars width = 10)
    plt.figure(figsize=(12, 6))
    plt.hist(data, bins=range(min_val, n, 10), color='skyblue', edgecolor='black')  # +2 so last number is included
    plt.title("Distribution des durées des tours")
    plt.xlabel("Durée (microsecondes)")
    plt.ylabel("Frequence")
    plt.grid(True, linestyle='--', alpha=0.5)
    plt.show()

    # Plot histogram base (all values, small bars)
    # plt.figure(figsize=(12, 6))
    # plt.hist(data, bins=range(min_val, max_val), color='skyblue', edgecolor='black')  # +2 so last number is included
    # plt.title("Distribution des durées des tours")
    # plt.xlabel("Durée (microsecondes)")
    # plt.ylabel("Frequence")
    # plt.grid(True, linestyle='--', alpha=0.5)
    # plt.show()

    # Plot histogram base (all values, big bars)
    # plt.figure(figsize=(10, 6))
    # plt.hist(data, bins=30, color='skyblue', edgecolor='black')  # adjust bins as needed
    # plt.title("Distribution of Numbers")
    # plt.xlabel("Value")
    # plt.ylabel("Frequency")
    # plt.grid(True, linestyle='--', alpha=0.5)
    # plt.show()


# Example usage
analyze_numbers("measures/measures.txt")
