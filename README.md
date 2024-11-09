# Rutile
Rutile is four rendering engines combined into one, it can be broken into two main components:
## Renderers
Currently Rutile supports four renderers:
### OpenGl
The OpenGl renderer uses the standard OpenGl rendering pipeline and implements both phong and solid shading
### CPU Ray Tracing
The CPU Ray tracer is mostly a proof of concept, it represents each object in the scene as a sphere and draws them with solid shading.
### GPU Ray Tracing
This is the primary renderer of Rutile, it implements a few key features:
* Uses both top level, and bottom level acceleration sturctures to improove rendering times.
* A complex material system including:
  * Diffuse materials
  * Both one way, and regular mirrors
  * Dielectrics
  * Emmisive materials
* Accumulation of pixel data across multiple frames

This is one of my favrioute scenes rendered with the GPU Ray Tracer, it features a modified Cornell box, that has all of its wall replaced with one way mirrors, it allows us to see in, but all the rays still bounce around inside creating what looks like dragons and teapots that go on forever.
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box%202.0.png "Cornell Box 2.0")
### Voxel Ray Tracing
The voxel ray tracer uses a sparse octree to store a voxelized version of each scene, and then applies the same materials present in the GPU ray tracer.
The current voxelization algorithim is a work in progress, but works decently well when triangles are small.

This is the same dragon model as seen elsewhere, except that it is voxelized to be 1024 voxels wide. This image shows off some of the flaws with the voxelization algorithim, with many small bumps being visible across the dragon. 
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/SilverDragon1024.png "Cornell Box 2.0")

### Hot Swapping
Hot swapping is a key feature of Rutile, with just a click of a button, you can change from one renderer to another, while still keeping the same camera position, FOV, and direction, aswell as which scene is selcted.
This makes it really easy to show off the different renderers side by side, here are some examples:

## 80k Dragon
This is a very famous dragon model, which has been simplified down to only have 80 thousand triangles, instead of the normal 800 thousand.
### Phong
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/80k%20Dragon/80K%20Dragon%20Phong.png "80k Dragon Phong")
### GPU Ray Traced
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/80k%20Dragon/80K%20Dragon%20Ray%20Traced.png "80k Dragon Ray Traced")
### Voxel Ray Traced
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/80k%20Dragon/80K%20Dragon%20Voxel%20Traced.png "80k Dragon Voxel Traced")

## Cornell Box
The classic Cornell Box.
### Phong
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box/OpenGl.png "Cornell Box Phong")
### GPU Ray Traced
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box/GPU-Ray-Tracing.png "Cornell Box Ray Traced")
### Voxel Ray Traced
This image shows off some of the issues with the current voxelization algorithim.
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box/Voxel-Ray-Tracing.png "Cornell Box Voxel Traced")

## Cornell Box 2.0
This is my custom take on the Cornell Box, with a little bit more complexity
### Phong
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box%202.0/OpenGL.png "Cornell Box 2.0 Phong")
### GPU Ray Traced
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box%202.0/GPUTraced.png "Cornell Box 2.0 Traced")
### Voxel Ray Traced
This image shows off some of the issues with the current voxelization algorithim.
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/Cornell%20Box%202.0/Voxel.png "Cornell Box 2.0 Voxel Traced")

## Framework
The framework holds the renderers together, it includes a basic GUI that facilitates many things including:
* Hot swapping between renderers
* Changing framework specific settings
* Changing renderer specific settings
* Modifiing the current scene
* Modifiing current materials

### Overall GUI:
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/framework/Overall%20Gui.png "Overall GUI")
### Scene Options:
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/framework/Scene%20Modification.png "Scene Options")
### Shadow Settings:
![alt text](https://github.com/Ben-Bingham/Rutile/raw/main/gallery/framework/Shadow%20Settings.png "Shadeow Settings")
