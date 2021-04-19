/* Maze 3D - A simplistic random maze generator.
 * Copyright (C) 2016 Armend Murtishi

 * This file is part of Maze 3D.

 * Maze 3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Maze 3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Maze 3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MAZE_H
#define MAZE_H

#include <stack>
#include <vector>
#include <algorithm>
#include <vmath.h>
#include <cstdlib>
#include <iostream>

constexpr unsigned MazeWidth = 10, MazeHeight = 10;
constexpr float WallSize = 0.15f;
static bool removeWall(vmath::vec2 currentCell, vmath::vec2 newCell, std::vector<vmath::vec3>& walls);
std::vector<vmath::vec3> GenerateMaze();

#endif /* end of include guard: MAZE_H */
