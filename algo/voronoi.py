import numpy as np
from scipy.spatial import Voronoi, voronoi_plot_2d
from matplotlib import pyplot as plt

# points = np.array([
#   [0,0], [0, 1], [0, 2],
#   [1,0], [1, 1], [1, 2],
#   [2,0], [2, 1], [2, 2]
# ])

# plt.scatter(points[:,0], points[:,1])

# vor = Voronoi(points)

# vor_vertices = vor.vertices
# print(vor_vertices)

# vor_regions = vor.regions
# print(vor_regions)

# fig = voronoi_plot_2d(vor)
# plt.show()

coords = np.random.rand(10, 2)
print(coords)
vor2 = Voronoi(coords)
plt.scatter(coords[:,0], coords[:,1])
print(vor2.vertices)
fig2 = voronoi_plot_2d(vor2)
plt.show()