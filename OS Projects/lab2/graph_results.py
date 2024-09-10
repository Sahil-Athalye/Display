import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file into a pandas DataFrame
# df = pd.read_csv('results.csv')

user_program = None
# set this if using arr search results
user_program = "arr_search"
actual_time = "binary search time" #"matrix_mul_time"

# NOTE: the df and CSV still uses matrix_mul_time for both programs

demand_df = pd.read_csv(f"demand_{user_program}.csv")
prepage_df = pd.read_csv(f"prepage_{user_program}.csv")

def group_data(df, paging):
    grouped = df.drop(columns=["trial"]).groupby("size")
    print(paging)
    summary = grouped.agg(['min', 'max', 'mean', 'median', 'std'])
    # print(summary)
    for column in summary.columns.levels[0]:
        print(column)
        print(summary[column])

    return grouped.agg(
    {"mmap_time": ["mean", "std"], "matrix_mul_time": ["mean", "std"]}
)

def text_user_program():
    return "Array Search" if user_program == "arr_search" else "Dense Matrix Multiply"

DEMAND = "Demand Paging"
PREPAGE = "Pre-Paging"
all_grouped_data = [
    (group_data(demand_df, DEMAND), DEMAND),
    (group_data(prepage_df, PREPAGE), PREPAGE)
]

# Plotting
plt.figure(figsize=(10, 6))

def plot_all_mmap():
    for (grouped_data, paging) in all_grouped_data:
        plot_mmap(grouped_data, paging)

def plot_all_matrix_mul():
    for (grouped_data, paging) in all_grouped_data:
        plot_matrix_mul(grouped_data, paging)


def color(paging):
    return "b" if paging == "Demand Paging" else "g"

def plot_mmap(grouped_data, paging):
    plt.errorbar(
        x=grouped_data.index,
        y=grouped_data[("mmap_time", "mean")],
        yerr=grouped_data[("mmap_time", "std")],
        label=f"mmap_time {paging}",
        fmt="--o",
        capsize=5,
        color=color(paging),
    )

def plot_matrix_mul(grouped_data, paging):
    plt.errorbar(
        x=grouped_data.index,
        y=grouped_data[("matrix_mul_time", "mean")],
        yerr=grouped_data[("matrix_mul_time", "std")],
        label=f"matrix_mul_time {paging}",
        fmt="-o",
        capsize=5,
        color=color(paging),
    )

plot_all_mmap()
plt.xlabel("Size")
plt.ylabel("mmap_time (ms)")
plt.title(f"mmap_time vs. Size for {text_user_program()}")
plt.legend()
plt.yscale("log")
plt.grid(True)
plt.savefig(f"mmap_{user_program}.png")
plt.show()

plot_all_matrix_mul()
plt.xlabel("Size")
plt.ylabel(f"{actual_time} (ms)")
plt.title(f"{actual_time} vs. Size for {text_user_program()}")
plt.legend()
plt.yscale("log")
plt.grid(True)
plt.savefig(f"actual_{user_program}.png")
plt.show()

plot_all_mmap()
plot_all_matrix_mul()
plt.title(f"Size vs. Duration for {text_user_program()}")
plt.xlabel('Size')
plt.ylabel('Duration (ms)')
plt.legend()
plt.yscale("log")
plt.grid(True)
plt.savefig(f"combined_{user_program}.png")
plt.show()
