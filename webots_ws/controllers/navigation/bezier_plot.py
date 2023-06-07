import numpy as np
from scipy.spatial import Voronoi
import random
import networkx as nx
import math
import bezier
import csv

num_points = 5
random.seed(98)
x = [random.uniform(-4.5,4.5) for _ in range(num_points)]
y = [random.uniform(-3,3) for _ in range(num_points)]
print('position', np.array([x, y]).T)

x_edge = [-4.5,-4.5,-4.5,-2.25,0,2.25,4.5,4.5,4.5,2.25,0,-2.25]
y_edge = [-3,0,3,3,3,3,3,0,-3,-3,-3,-3]

start_point = [-4,-2.5]
goal_point = [3.5,1]

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

result = curve.evaluate_multi(np.linspace(0.0, 1.0, 100))

header = ['x', 'y']
data = result.T

with open('path.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)

    writer.writerow(header)
    writer.writerows(data)
    print(len(data))