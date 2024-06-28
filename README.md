## Viewer for triangular mesh .ply files
__A small project to learn c++.__
The program can parse .ply files for triangular mesh,
and plot the data.

__Capabilities:__
- flat shading and smooth shading
- Use different colormaps to show the scalar mean curvature
- zoom and rotate with the mouse

## Requirements
- opengl 4.6
- glfw

## Compilation

        make -C sources

## Usage

        ./meshviewer meshes/deform.ply
