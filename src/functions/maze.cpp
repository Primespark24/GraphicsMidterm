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
 *
 * Edited by Kyle Shepard and Brycen Martin
 * changes include converting changing over from glm to vmath
 */

#include "maze.h"

static void initWalls(std::vector<vmath::vec3>& array)
{
    for (unsigned x = 0; x < MazeWidth; x++)
        for (unsigned y = 0; y < MazeHeight; y++)
            for (unsigned orientation = 0; orientation < 4; orientation++)
                array.push_back(vmath::vec3(x, y, orientation));
}

static bool getCell(int x, int y, bool cells[][MazeHeight])
{
    return x >= 0 && x < (int)MazeWidth && y >= 0 && y < (int)MazeHeight ? cells[y][x] : true;
}

static std::vector<vmath::vec2> getUnvisitedNeighbors(vmath::vec2 pos, bool cells[][MazeHeight])
{
    std::vector<vmath::vec2> result;
    if (!getCell(pos[0] - 1, pos[1], cells))
        result.push_back(vmath::vec2(pos[0] - 1, pos[1]));
    if (!getCell(pos[0], pos[1] - 1, cells))
        result.push_back(vmath::vec2(pos[0], pos[1] - 1));
    if (!getCell(pos[0] + 1, pos[1], cells))
        result.push_back(vmath::vec2(pos[0] + 1, pos[1]));
    if (!getCell(pos[0], pos[1] + 1, cells))
        result.push_back(vmath::vec2(pos[0], pos[1] + 1));
    return result;
}

/**
 * removes necessary walls
 * @param currentCell cell we were just now in
 * @param newCell cell we are travelling to
 * @param walls list of walls passed by reference
 */
static bool removeWall(vmath::vec2 currentCell, vmath::vec2 newCell, std::vector<vmath::vec3>& walls)
{
    int size = walls.size();
    auto remove = [&](unsigned wall, unsigned second)
    {
        for (auto iter = walls.cbegin(); iter < walls.cend(); /*iter++*/)
        {
            vmath::vec3 vec = *iter;
            if (
                (vec[0] == currentCell[0] && vec[1] == currentCell[1] and vec[2] == wall) ||
                (vec[0] == newCell[0] && vec[1] == newCell[1] and vec[2] == wall))
            {
                iter = walls.erase(iter);
            }
            else
            {
                ++iter;
            }
}
        // walls.erase(std::remove(walls.begin(), walls.end(), vmath::vec3(currentCell[0], currentCell[1], wall)), walls.end());
        // walls.erase(std::remove(walls.begin(), walls.end(), vmath::vec3(newCell[0], newCell[1], second)), walls.end());
    };

    if (newCell[0] < currentCell[0]){
        remove(0, 2);
    }
    else if (newCell[0] > currentCell[0]){
        remove(2, 0);
    }
    if (newCell[1] < currentCell[1]){
        remove(1, 3);
    }
    else if (newCell[1] > currentCell[1]){
        remove(3, 1);
    }

    //return if removal was successful
    return size > walls.size();
}

std::vector<vmath::vec3> GenerateMaze()
{
    std::vector<vmath::vec3> walls;
    initWalls(walls);

    vmath::vec2 currentCell(0, 0);
    bool cells[MazeWidth][MazeHeight] = { { 0 } };
    cells[(int)currentCell[1]][(int)currentCell[0]] = true;
    unsigned visitedCells = 1;

    std::stack<vmath::vec2> used;

    while (visitedCells < MazeWidth * MazeHeight) {
        std::vector<vmath::vec2> neighbors = getUnvisitedNeighbors(currentCell, cells);
        if (neighbors.size() != 0) {
            vmath::vec2 newCell = neighbors[std::rand() % neighbors.size()];
            used.push(currentCell);
            if(!removeWall(currentCell, newCell, walls)){
                // int y = 7/0; // force a crash
            }
            currentCell = newCell;
            cells[(int)currentCell[1]][(int)currentCell[0]] = true;
            visitedCells++;
        }
        else if (used.size() != 0) {
            currentCell = used.top();
            used.pop();
        }
    }

    return walls;
}
