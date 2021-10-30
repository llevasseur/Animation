system partSys dim 8
system partSys particle 0 1 -4 4 0 0 0 0
system partSys particle 1 1 -3 4 0 0 0 0
system partSys particle 2 1 -2 4 0 0 0 0
system partSys particle 3 1 -1 2 -2 0 0 0
system partSys particle 4 1 0 3 1 0 0 0
system partSys particle 5 1 1 1 3 0 0 0
system partSys particle 6 1 2 4 0 0 0 0
system partSys particle 7 1 3 3 0 0 0 0
simulator partSim link partSys 4
simulator partSim spring 0 1 20 0.1 0.5
simulator partSim spring 1 2 20 0.1 0.5
simulator partSim spring 6 7 20 0.1 0.5
simulator partSim fix 0
simulator partSim fix 7
simulator partSim integration symplectic 0.01
