<div align='center'>

### Humanoid Robot Simulation 
---
![](assets/soccer_1.png?raw=true)

 </div>
 
This project is about simulation humanoid robot soccer path planning with A* algorithm and bezier curves

<br>

Requirement:
- c++ compiler
- Webots 2023a
- Qt 6

Add dynamic linking library
```
export WEBOTS_HOME=/usr/local/webots
export RESOURCES_PATH=$WEBOTS_HOME/projects/robots/robotis/darwin-op
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RESOURCES_PATH/libraries/managers/:$WEBOTS_HOME/lib/controller/:$RESOURCES_PATH/libraries/robotis-op2/
```