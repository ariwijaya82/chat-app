# python 3.9

import numpy as np
import random
import csv
import sys
from matplotlib import pyplot as plt, patches


# random_seed = [11, 22, 33, 44, 55]
seed = 123
if len(sys.argv) > 2: seed = int(sys.argv[2])
def generate_rand(lower, upper):
    global seed
    seed *= 7
    random.seed(seed)
    return random.uniform(lower, upper)

num_points = 5
enemy = [None] * num_points
enemy[0] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[1] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[2] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[3] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[4] = [generate_rand(-2,2), generate_rand(-2,2)]
x = [item[0] for item in enemy]
y = [item[1] for item in enemy]

start_point = [generate_rand(-3,-2),generate_rand(-1,0)]
goal_point = [generate_rand(3,4),generate_rand(0,1)]


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