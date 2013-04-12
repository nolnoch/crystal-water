crystal-water
=============

OpenGL Concept Exploration - Ray Tracing and Particle Systems

By completion, this project will ideally render a crystal cube or sphere
suspended in a textured skybox and surrounded by or interacting with a
particle system-based water feature.

This project is an exploration of modern OpenGL concepts including:
  - GLSL 4.20
  - OpenGL 4.20 Core
  - Freeglut Extended features
  - Vertex Buffer and Element Arrays
  - Lighting models (Phong vs Blinn, etc)
  - Mesh loading
  - Arcball-like control (modified here)
  - Using a skybox
  - Texture loading, mapping, and sampling
  - GLM math library
  - SOIL image library
  - AssImp model library*
  - Ray tracing*
  - Uniform Buffers*
  - Transform Feedback Buffers*
  - Particle Systems*

* denotes concepts yet to be implemented.
  

Notes:

Custom Program class
  - Loads user-specified shader files as Shaderobj objects.
  - Simplifies the most common calls in setup and execution.
  - Makes multiple shader programs much simpler.

Custom Quaternion class
  - Need to avoid gimbal lock.
  - Allows slerping (Spherical Linear intERPolation).
  - Understood just enough to put it together and make it robust.

Matrix management
  - Deprecation of matrix stack and GLSL built-in variables.
  - All maintained by hand (with help from GLM and my quaternions).
  - School has only taught direct/immediate mode thus far.
