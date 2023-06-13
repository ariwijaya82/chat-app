import numpy as np
import random
import csv
import sys
from matplotlib import pyplot as plt, patches
import math

random_seed = [123, 234, 987, 56, 82]
seed = 123
if len(sys.argv) > 2: seed = int(sys.argv[2])
def generate_rand(lower, upper):
    global seed
    seed *= 7
    random.seed(seed)
    return random.uniform(lower, upper)

num_enemy = 5
position = [None] * (num_enemy+2)
for i in range(len(position)):
    if i == 0:
        position[0] = [generate_rand(-4,-2), generate_rand(-2,2)]
        continue

    x_range = (-2,2) if i <= num_enemy else (2,4)
    y_range = (-2,2)

    new_position = None
    other_position = position[:i]
    collision = [True] * len(other_position)
    while True in collision:
        new_position = [generate_rand(x_range[0],x_range[1]), generate_rand(y_range[0],y_range[1])]
        collision = [math.sqrt((new_position[0]-other[0])**2 + (new_position[1]-other[1])**2) < 1
                        for other in other_position]
    position[i] = new_position

enemy = position[1:1+num_enemy]
x = [item[0] for item in enemy]
y = [item[1] for item in enemy]

start_point = position[0]
goal_point = position[-1]

def generate_plot():
    # draw plot
    _, ax = plt.subplots()
    rect = patches.Rectangle((-5,-3.5), 10, 7, facecolor='green')
    ax.add_patch(rect)
    
    plt.scatter(x, y, color='red', marker='o')
    for i in range(len(x)):
        circle = patches.Circle((x[i], y[i]), 0.5, color='red', fill=False)
        ax.add_patch(circle)

    plt.scatter([start_point[0], goal_point[0]],
      [start_point[1], goal_point[1]],
      color=['black', 'pink'],
      marker='o'
    )

    plt.plot([-4.5, -4.5, 4.5, 4.5, -4.5, 0, 0], [-3, 3, 3, -3, -3, -3, 3], color='white')
    ax.add_patch(patches.Circle((0,0), 1, color='white', fill=False))
    plt.plot([-4.5, -3, -3, -4.5], [-2, -2, 2, 2], color='white')
    plt.plot([4.5, 3, 3, 4.5], [-2, -2, 2, 2], color='white')
    
    ax.set_aspect('equal')
    ax.set_xlim(-5, 5)
    ax.set_ylim(-3.5, 3.5)
    
    plt.show()

def generate_world():
    # [ball, myRobot, enemy1, enemy2, enemy3, enemy4, enemy5]
    line_index = [32, 36, 52, 61, 70, 79, 88]
    x_ = [goal_point[0], start_point[0]] + x
    y_ = [goal_point[1], start_point[1]] + y
    for i in range(len(line_index)):
        lines = open('../webots_ws/worlds/soccer.wbt', 'r').readlines()
        if lines[line_index[i]-1].find('translation') != -1:
            lines[line_index[i]-1] = f'  translation {x_[i]} {y_[i]} {0.236 if i != 0 else 0.03}\n'
            file = open('../webots_ws/worlds/soccer.wbt', 'w')
            file.writelines(lines)
            file.close()

    # position csv
    file = open('../webots_ws/data/position.csv', 'w')
    writer = csv.writer(file)
    writer.writerows(np.array([x_, y_]).T)

args = sys.argv[1]
if args == 'plot':
    generate_plot()
elif args == 'world':
    generate_world()
else:
    print("please provide argument plot or world")