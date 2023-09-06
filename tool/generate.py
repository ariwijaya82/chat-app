import sys
import random
import json
import math
from matplotlib import pyplot as plt, patches

world_file_path = 'webots_ws/worlds/soccer.wbt'
position_file_path = 'config/position.json'

random_seed = [123, 234, 987, 56, 82]
seed = 123
if len(sys.argv) > 2:
    seed = random_seed[int(sys.argv[2])]
def generate_rand(lower, upper):
    global seed
    seed *= 7
    random.seed(seed)
    return random.uniform(lower, upper)

# generate random position
num_enemy = 5
random_position = [None for i in range(num_enemy+2)]
for i in range(num_enemy+2):
    if i == 0:
        random_position[0] = [generate_rand(-4,-2), generate_rand(-2,2)]
        continue

    x_range = (-2,2) if i <= num_enemy else (2,4)
    y_range = (-2,2)

    new_position = None
    other_position = random_position[:i]
    collision = [True] * len(other_position)
    # avoid collision between position
    while True in collision:
        new_position = [generate_rand(x_range[0],x_range[1]), generate_rand(y_range[0],y_range[1])]
        collision = [math.sqrt((new_position[0]-other[0])**2 + (new_position[1]-other[1])**2) < 1
                        for other in other_position]
    random_position[i] = new_position

# draw random position with matplotlib
def draw_position():
    # init data
    enemy = random_position[1:1+num_enemy]
    x = [item[0] for item in enemy]
    y = [item[1] for item in enemy]

    start_point = random_position[0]
    goal_point = random_position[-1]

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

# update world and position file with generated random position
def update_world_position():
    # [ball, myRobot, enemy1, enemy2, enemy3, enemy4, enemy5]
    line_index = [32, 36, 52, 66, 80, 94, 108]
    position = [random_position[-1], random_position[0]] + random_position[1:num_enemy+1]
    updated_file = open(world_file_path, 'r').readlines()
    for i in range(len(line_index)):
        if updated_file[line_index[i]-1].find('translation') != -1:
            updated_file[line_index[i]-1] = f'  translation {position[i][0]} {position[i][1]} {0.236 if i != 0 else 0.03}\n'
    # updated world file
    world_file = open(world_file_path, 'w')
    world_file.writelines(updated_file)
    world_file.close()

    # generate json
    data = {
        'ball': None,
        'robot': None,
        'enemy': [],
    }
    for i in range(len(line_index)):
        # read position value
        coordinate = {
            'x': position[i][0],
            'y': position[i][1],
        }
        if i == 0: # ball
            data['ball'] = coordinate
        elif i == 1: # robot
            data['robot'] = coordinate
        else: # enemy
            data['enemy'].append(coordinate)
    position_file = open(position_file_path, 'w')
    json_object = json.dumps(data)
    position_file.write(json_object)

# update position file from world file
def get_position_from_world():
    # [ball, myRobot, enemy1, enemy2, enemy3, enemy4, enemy5]
    line_index = [32, 36, 52, 66, 80, 94, 108]
    # open world file
    world_file = open(world_file_path, 'r').readlines()
    # format position_file_pathta
    data = {
        'ball': None,
        'robot': None,
        'enemy': [],
    }
    for i in range(len(line_index)):
        # read position value
        value = world_file[line_index[i]-1].split()
        coordinate = {
            'x': float(value[1]),
            'y': float(value[2]),
        }
        if i == 0: # ball
            data['ball'] = coordinate
        elif i == 1: # robot
            data['robot'] = coordinate
        else: # enemy
            data['enemy'].append(coordinate)

    # generate json
    json_object = json.dumps(data, indent=4)
    position_file = open(position_file_path, 'w')
    position_file.write(json_object)

args = sys.argv[1]
if args == 'plot':
    draw_position()
elif args == 'world':
    update_world_position()
elif args == 'copy':
    get_position_from_world()
else:
    print("input format generate.py [plot|world|copy] [null|0-5]")