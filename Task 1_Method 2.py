import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation


# KALMAN FILTER

class KalmanFilter:

    def __init__(self, initial_value):
        self.estimate = initial_value
        # error of the sensor
        self.error = 1.0
        # amount of noise in the sensor.
        # (The larger the measurement_noise value we enter, smoother the graph will be)
        self.measurement_noise = 50.0
        # constantly injects a small amount of uncertainty so the Kalman gain keeps paying attention to incoming sensor data.
        self.process_noise = 1.0


    def update(self, measurement):
        self.error = self.error + self.process_noise

        kalman_gain = (self.error /(self.error + self.measurement_noise))

        self.estimate = (self.estimate +kalman_gain *(measurement - self.estimate))

        self.error = ((1 - kalman_gain) *self.error)

        return self.estimate


# 1. Read the data

data = pd.read_csv("Depth Data.csv")

# Get the Depth column
depth = pd.to_numeric(
    data["Depth (m)"],
    errors="coerce"
)

# 2. CREATE TIME VALUES

time = list(range(len(depth)))


# 3. KALMAN FILTER
first_value =  depth.iloc[0]

# Create the filter
kalman = KalmanFilter(first_value)

# 4. PREPARE LISTS FOR THE GRAPH
time_data = []
filtered_data = []

# 5. CREATE GRAPH
fig, ax = plt.subplots(figsize=(10, 6))

ax.set_title("Real-Time Ship Depth Monitoring")

ax.set_xlabel("Time (seconds)")

ax.set_ylabel("Depth (m)")

ax.grid(True, alpha=0.3)


# Create an empty line
line, = ax.plot(
    [],
    [],
    linewidth=2,
    label="Kalman Filtered Depth"
)
ax.legend()

# Set graph limits
ax.set_xlim(0,len(time) - 1)
ax.set_ylim(-500,-50)

# 6. UPDATE FUNCTION
def update(frame):
    measurement = depth.iloc[frame]

    if pd.isna(measurement) or measurement == 0 or measurement<-500:
        filtered_value = kalman.estimate

    else:
        filtered_value = kalman.update(measurement)

    time_data.append(time[frame])

    filtered_data.append(filtered_value)

    line.set_data(
        time_data,
        filtered_data
    )

    return line,


# 7. START ANIMATION

animation = FuncAnimation(
    fig,
    update,
    frames=len(depth),
    interval=1000,
    repeat=False
)

plt.tight_layout()

plt.show()
