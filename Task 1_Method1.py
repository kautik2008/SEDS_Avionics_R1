import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


# 1. Read the data

data = pd.read_csv("Depth Data.csv")

depth = pd.to_numeric(
    data["Depth (m)"],
    errors="coerce"
)


# 2. Replace sensor errors

depth_clean = depth.copy()

for i in range(1, len(depth_clean) - 1):
    previous_value = depth_clean.iloc[i - 1]
    next_value = depth_clean.iloc[i + 1]
    avg = (previous_value + next_value) / 2

    if depth_clean.iloc[i] == 0:
        depth_clean.iloc[i] = avg

    if depth_clean.iloc[i] < -500:
        depth_clean.iloc[i] = avg

# 3. Reduce noise

depth_smooth = (
    depth_clean
    .rolling(window=5, center=True, min_periods=1)
    .mean()
)


# 4. Create time values

time = pd.to_numeric(
    data["Point"],
    errors="coerce"
)



# 5. Create the graph

fig, ax = plt.subplots(figsize=(10, 6))

ax.set_title("Ship Depth Monitoring")
ax.set_xlabel("Time (seconds)")
ax.set_ylabel("Depth (metres)")

ax.grid(True, alpha=0.3)

line, = ax.plot(
    [],
    [],
    linewidth=2,
    label="Filtered Depth"
)

ax.legend()


# Set the limits of the graph

ax.set_xlim(0, len(time))

ax.set_ylim(
    min(depth_smooth) - 50,
    max(depth_smooth) + 50
)


# 6. Animation function

def update(frame):

    current_time = time[:frame+1]
    current_depth = depth_smooth.iloc[:frame+1]

    line.set_data(
        current_time,
        current_depth
    )

    return line,


# -----------------------------------------
# 7. Start the animation
# -----------------------------------------

animation = FuncAnimation(
    fig,
    update,
    frames=len(time),
    interval=1000,
    repeat=False
)


plt.tight_layout()
plt.show()
