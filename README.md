# Voxelized Shadows
This is a final year research project investigating Voxelized Shadows for compressing precomputed shadow map textures for large outdoor scenes. It allows very high resolution shadow maps to be stored with compression rates of up to 4000 to 1, while retaining the ability to sample filtered shadows in real time.

This is based on the work by Sintorn et al in "Compact Precomputed Voxelized Shadows" (ACM Transactions on Graphics 2014).

![screenshot of the application](Readme/screenshot-top.jpg?raw=true)

## Development

This project was created using C++ with the OpenGL and Qt libraries. It runs on Mac and Linux.

## Implemented Features

- Cascaded Shadow Mapping with 1, 2 or 4 cascades
- Static Shadow Maps compressed using Voxelised Shadows
- "Combined" shadowing mode mixing static and dynamic shadows
- Extensive configuration of the above techniques from the user interface
- A number of debugging modes to visualize the rendering techniques 

## How To Run

- Clone the project
- Ensure qt is enabled (In DEC-10 use module add qt/5.3.1)
- Run the install.sh script
- The voxelised-shadows application can now be run from the terminal

## Settings

- Specify the voxel tree resolution from the terminal (eg ./voxelised-shadows 64k)
- Add the -precompute flag to build the tree before the application starts. This is faster. (eg ./voxelised-shadows 128k -precompute)
- Other settings can be toggled from the UI

## Camera Controls

- Click and drag to rotate the camera
- Hold w, a, s and d to move the camera forwards, backwards, left and right
- Hold q and e to move the camera up and down
- Hold shift to move faster

## Debug Overlays

The application contains a number of debugging overlays to visualize aspects of the rendering process:

- Shadow Map: Displays the shadow map texture(s) in the corner of the screen
- Scene Depth: Shows the output from the scene depth pass
- Shadow Mask: Shows the output from the shadow mask pass
- Cascade Splits: Shows the area contained in each Cascaded Shadow Mapping cascade
- Voxel Tree Depth: Shows the depth of the voxelised shadows tree in each locaton
