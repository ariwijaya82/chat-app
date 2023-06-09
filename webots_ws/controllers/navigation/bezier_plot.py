# python 3.9

import numpy as np
from scipy.spatial import Voronoi, voronoi_plot_2d
import random
import networkx as nx
import math
import bezier
import csv
import sys
from matplotlib import pyplot as plt, patches

num_points = 5
# random_seed = [11, 22, 33, 44, 55]
seed = int(sys.argv[2])
def generate_rand(lower, upper):
    global seed
    seed *= 7
    random.seed(seed)
    return random.uniform(lower, upper)

enemy = [None] * 5
enemy[0] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[1] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[2] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[3] = [generate_rand(-2,2), generate_rand(-2,2)]
enemy[4] = [generate_rand(-2,2), generate_rand(-2,2)]
x = [item[0] for item in enemy]
y = [item[1] for item in enemy]

x_edge = [-4.5,-4.5,-4.5,-2.25,0,2.25,4.5,4.5,4.5,2.25,0,-2.25]
y_edge = [-3,0,3,3,3,3,3,0,-3,-3,-3,-3]

start_point = [generate_rand(-3,-2),generate_rand(-1,0)]
goal_point = [generate_rand(3,4),generate_rand(0,1)]

coordinates_x = [start_point[0]]+x+x_edge+[goal_point[0]]
coordinates_y = [start_point[1]]+y+y_edge+[goal_point[1]]
coordinates = np.array([coordinates_x, coordinates_y]).T

vor = Voronoi(coordinates)

def heuristic(coord1, coord2):
    x1, y1 = coord1
    x2, y2 = coord2
    return math.sqrt((x2 - x1)**2 + (y2 - y1)**2)

def point_inside_field(point, width=9, height=6):
    x, y = point
    return (-width/2 <= x <= width/2) and (-height/2 <= y <= height/2)

# Create a graph
graph = nx.Graph()
for a, b in vor.ridge_vertices:
    if -1 not in (a, b) and point_inside_field(vor.vertices[a]) and point_inside_field(vor.vertices[b]):
        graph.add_edge(a, b, weight=heuristic(vor.vertices[a], vor.vertices[b]))

region_index = vor.point_region[np.argmin(np.linalg.norm(np.array(coordinates) - np.array(start_point), axis=1))]
region_vertices = vor.regions[region_index]
for item in region_vertices:
    if item != -1:
        graph.add_edge('start', item, weight=heuristic(start_point, vor.vertices[item]))

region_index = vor.point_region[np.argmin(np.linalg.norm(np.array(coordinates) - np.array(goal_point), axis=1))]
region_vertices = vor.regions[region_index]
for item in region_vertices:
    if item != -1:
        graph.add_edge('goal', item, weight=heuristic(goal_point, vor.vertices[item]))

path = nx.astar_path(graph, 'start', 'goal')

x_path = []
y_path = []
for index in range(len(path)):
    if path[index] == 'start':
        point = vor.points[0]
    elif path[index] == 'goal':
        point = vor.points[-1]
    else:
        point = vor.vertices[path[index]]
    x_path.append(point[0])
    y_path.append(point[1])

nodes = np.asfortranarray([x_path, y_path])
curve = bezier.Curve(nodes, len(x_path)-1)

def generate_csv():
    result = curve.evaluate_multi(np.linspace(0.0, 1.0, 100))
    data = result.T

    with open('path.csv', 'w', encoding='UTF8', newline='') as f:
        writer = csv.writer(f)
        writer.writerows(data)

def generate_plot():
    # draw plot
    _, ax = plt.subplots()
    rect = patches.Rectangle((-4.5,-3), 9, 6, linewidth=1, edgecolor='black', facecolor='green')
    ax.add_patch(rect)
    
    plt.scatter(x, y, color='red', marker='o')
    for i in range(len(x)):
        circle = patches.Circle((x[i], y[i]), 0.5, color='red', fill=False)
        ax.add_patch(circle)

    plt.scatter(
      [start_point[0], goal_point[0]],
      [start_point[1], goal_point[1]],
      color='black',
      marker='o'
    )
    
    voronoi_plot_2d(vor, ax=ax, show_points=False, line_colors='blue', line_alpha=0.5)
    
    index = 0
    for i, j in vor.vertices:
        if point_inside_field((i, j)):
          plt.text(i, j, str(index), ha='center', va='bottom')
        index += 1
    
    index = 0
    for i, j in coordinates:
        if point_inside_field((i, j)):
          plt.text(i, j, str(index), ha='center', va='bottom')
        index += 1
    
    # draw path
    plt.plot(x_path, y_path, marker='o', color='pink')
    curve.plot(100, ax=ax)
    
    ax.set_aspect('equal')
    ax.set_xlim(-5, 5)
    ax.set_ylim(-3.5, 3.5)
    
    plt.show()

args = sys.argv[1]
if args == 'plot':
    generate_plot()
elif args == 'csv':
    generate_csv()
else:
    print("please provide argument plot or csv")