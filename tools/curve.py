import csv
import bezier
import numpy as np
import matplotlib.pyplot as plt

file = open('../webots_ws/data/path.csv')
lines = csv.reader(file)
points = []
for line in lines:
    points.append([float(item) for item in line])

degree = len(points)-1
nodes = np.array(points).T
bez = bezier.Curve(nodes=nodes, degree=degree)
curve_points = bez.evaluate_multi(np.linspace(0.0, 1.0, 100))

file = open('../webots_ws/data/curve.csv', 'w')
writer = csv.writer(file)
writer.writerows(curve_points.T)