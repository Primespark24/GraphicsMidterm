# Midterm Project - Maze
Whitworth University - CS-357 Computer Graphics  
Last edited: 3/31/2021 by Scott  
Based on work by Kent Jones

## Overview
Your overall goal is to make a game / simulation of a maze. You are going to have to map key bindings and mouse positions to interact with your game state. The project is broken up in to smaller chunks to aid in development, but you should always be thinking back to your larger design. As we progress through this project, there will be various 'check in' points. Pay attention to the due dates and project specifications along the way.

You are welcome (and encouraged) to use provided code, both from me and from sb7 source code. You can use external code for utility, but not for the main focus of your work (i.e. you cannot copy someone elses' collision code, but you can use someone elses' PNG loading framework). You are going to need to be able to explain all code that you submit. Always be clear about your citations.

As this is a 300 level course, I expect adherence to style and appropriate and complete commenting.

As always: you can do this assignment in whatever Graphics Language you like, but you need to approach the theme of the original assignment if not doing it in OpenGL.

### Deliverable Dates
Demos for part 1 and 2: April 9th (midnight)
Final Demo and documentation: April 16th (midnight)

## Grade

| Portion               | Points Possible |
|-----------------------|-----------------|
| Skycube Set Up        | 15              |
| Collision Set Up      | 20              |
| World / Maze Creation | 40              |
| Complete Demo         | 15              |
| Documentation         | 10              |

As you work through this, keep each part separated inside your repository. You can use branches, but I would suggest keeping different folders for each sub-section. I should be able to look at your part 1 and see the changes into part 4. 

Please only upload original code and documentation! I don't want to see whole libraries uploaded or zip files unrelated to what you did. You should use a good `.gitignore` file.

## Part 1: Skycube Set Up (15 pts)
Start by developing an application that loads a skycube of your own design, along with a sample object, just for positional reference (could be a cube).

You program should:
* Keep user's perspective in the center of the skycube
* Allow user to move and look around. Traditional WASD + mouse is fine, but alternate movement is also accepted. User should be locked into planar movement (no flying) but if you want to implement jumping, that is fine
* Generate / Create your own skycube textures. These should be developed by the team, not downloaded or sourced externally. You can use photos, you can draw it, be creative.
  * Remember that BMP files need to be a certain size 
  * Also OpenGL has size limitations. Trying to fit a 2MB sized photo into a bitmap is not going to work without some massaging.
* Scale skycube to be appropriately sized for the images you are using for the texture. This should be where you decide on a relative 'scale' for your world.

On April 9th (by midnight) you will need to submit video proof of your part 1 working as well as post it to the designated area on our Discord server. This video should briefly show all elements outlined above working and operational. You may want to load at least one non-cube object to give reference to the world motion of the camera (because the sky cube should not be translating).

## Part 2: Collision Set Up (20 pts)
This is a stepping stone, don't over think this (yet). Keep things simple and get them working in a base case before moving on.

Draw a single cube in your world that is about the same 'size' as the player. Demonstrate that the player cannot 'walk' through the cube (object collision).

Suggestions:
* Think about how you want to define interaction space. This should be parallel, but slightly different from your world space.
* You probably only need to do the computation when the player moves, and you may want to treat this as the inputs being a 'request to move' and another function allowing for that movement based on game state.
* If you use bounding boxes, calculating interactions should not be too challenging.
  * Think about simplifying space (subtracting offsets might help) down to the basic interaction space.
  * Remember that the length of things might be helpful in determining how close you are to something else.
* You could also approach this in other ways, on the micro level (https://en.wikipedia.org/wiki/Liang%E2%80%93Barsky_algorithm) or macro (maze state overworld control)

On April 9th (by midnight) you will need to submit video proof of your part 2 working as well as post it to the designated area on our Discord server. This video should briefly show the collision functionality. I am only looking for one object, but you are free to load more to show your program.

## Part 3: World / Maze Creation (40 pts)
Fill it in! You are going to randomly generate a maze to move through. This should be either based on a seed (for reproducibility) or time (for random generation).

All of the above part requirements must still be met (skycube, player control, collision).

## Part 4: Complete Program and Final Demo (15 pts)
Add some polish to your program. This portion should utilize texture mapping and object loading for a more robust visualization. You need to implement at least one meaningful feature not identified in this document.

All of the other parts should be demo-ed here. Random generation, collision detection, skycube.

On April 16th (by midnight) you will need to submit a video presentation of your final program working. You can think of this as a game trailer showing off all the functionality of your program. You will also need to post this to the designated area on our Discord server. This is not a code review, everyone is working on an adjacent program, so focus on what makes your program unique. Technical descriptions are welcome if they help show off your work. It should show a full 'run' of one of your mazes (time-lapse is appropriate if you have a really big or complicated maze). 

## Part 5: Documentation (10 pts)
Half of this is a program document and half is a project reflection. 

In your repository on the 16th you should have:
* all source files for code and graphical elements
* a collection of development screenshots of the graphical elements with descriptions of how they are used
* a basic flowchart of how the maze is generated and what the different parts of your program are and how they interact.
* manual for the features of the game
* build instructions for your code
* Well functionalized and documented code (no spaghetti code and every line should be referenced by at least one meaningful comment)

Each individual will need to submit via email a ~500 word personal reflection on the project. How did it go? How did you plan? How did you split work? What would you do differently if you did it again? What are your thoughts on what you made? 