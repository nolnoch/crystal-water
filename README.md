crystal-water
=============

OpenGL Concept Exploration - Ray Tracing and Particle Systems

By completion, this project will ideally render a crystal cube or sphere
suspended in a textured skybox and surrounded by or interacting with a
particle system-based water feature.

Required libraries: GLEW, Freeglut, GLM, SOIL

Stage 1 (complete): Establish controls and render primitive object in a skybox.
       ^Challenge : Which modern libraries are robust, supported, and commonly
                    in use? How do we use the mouse wheel to zoom? How can we
                    adapt arcball rotation for a more application-specific
                    control system?
        ^Solution : GLM math library, SOIL image library, and AssImp model
                    are ideal for this and future projects. Rotation is disc-
                    based with a vertical slide. Zoom uses mousewheel in spite
                    of Freeglut fail in recognizing the mousewheel funcion.

Stage 2 (current) : Ray trace the floating cube to give it a crystal appearance.
       ^Challenge : Ray-plane intersections should be done on the GPU, but after
                    calculating the destination vertex, how do we get the assoc.
                    tex coord from the VBO?  Shaders do not provide access to an
                    Vertex Buffer element from within another.
        ^Solution : Send a Uniform Buffer with vertex/texCoord pairings to the
                    shaders for real-time lookup.  Unfortunately, this increases
                    set-up time and is not scalable to larger systems or scenes.

Stage 3 (planned) : Integrate OpenCL 1.2 to handle ray tracing in GPU kernels.
       ^Challenge : How can we use the more ideal OpenCL API to solve our GLSL
                    problems?  How can we accomplish this within the context of
                    an OpenGL application?

Stage 4 (planned) : Add some simulation of water to this project.
       ^Challenge : Water can be mesh-based or particle-based. Let's learn the
                    concept of particle systems and use that to simulate water
                    in our ray-traced scene.

This project is an exploration of modern OpenGL concepts including:
  - GLSL 4.20
  - OpenGL 4.20
  - OpenCL 1.2**
  - Freeglut Extended features
  - Vertex Buffers and Element Arrays
  - Lighting models
  - Mesh loading
  - Arcball-like control
  - Using a skybox
  - Texture loading, mapping, and sampling
  - Modern libraries
  - Ray tracing**
  - Uniform Buffers**
  - Particle Systems**

(**) denotes concepts yet to be implemented.


Design Hardware: Athlon II X4 640, 8GB RAM, Sapphire Radeon HD 6870
Design Software: Eclipse CDT Juno on Kubuntu 12.10 Quantal


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

