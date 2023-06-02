import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib import animation
import numpy as np
import random

n_particles = 40
b_particles = 5
box_width = 10
dt = 0.001

fig, ax = plt.subplots()
ax.set_xlim(0, 10)
ax.set_ylim(0, 10)
ax.set_aspect('equal')

def get_initial_coordinates():
    x = np.random.uniform(2, 3, n_particles)
    y = np.random.uniform(2, 7, n_particles)
    return x, y

def get_initial_velocities():
    xv = np.zeros(n_particles)
    yv = np.zeros(n_particles) - 20
    return xv, yv

def take_step(x, y, xv, yv):
    for i in range(n_particles):
        x[i] += xv[i] * dt
        y[i] += yv[i] * dt
        if y[i] <= random.uniform(-1, 3):
            xv[i] = 20
            yv[i] = 0
        if x[i] >= random.uniform(7, 10):
            xv[i] = 0
            yv[i] = 20
        if y[i] >= 7 and 7 < x[i] < 9:
            xv[i] = 0
            yv[i] = 0
        if y[i] >= 7 and x[i] >= 7:
            y[i] = 10000
    return x, y, xv, yv

x, y = get_initial_coordinates()
xv, yv = get_initial_velocities()
e, = ax.plot(x, y, 'ro')