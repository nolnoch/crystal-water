crystal-water
=============

OpenGL Concept Exploration - Ray Tracing and Particle Systems

**Premise**: The current, shader-based GPU pipeline in OpenGL makes ray tracing
in real time prohibitively complex and inflexible, resulting in an industry
avoidance of OpenGL for this kind of rendering.  By using OpenCL and its
OpenGL interoperability to reconfigure this pipeline, we will achieve real-time
ray tracing with an OpenGL program on a modest desktop computer.  Further, we may
approximate this functionality with greater ease and integration with the OpenGL
pipeline by exploring the new compute shaders when the OpenGL 4.3 standard is available.

By completion, this project will ideally render a crystal cube or sphere
suspended in a textured skybox and surrounded by or interacting with a
particle system-based water feature.

**Working branch**: opencl

___

**Required libraries**: GLEW, Freeglut, GLM, SOIL, Assimp

**Stage 1** (complete): Establish controls and render primitive object in a skybox.  
 - Challenge : *Which modern libraries are robust, supported, and commonly
                        in use? How do we use the mouse wheel to zoom? How can we
                        adapt arcball rotation for a more application-specific
                        control system?*  
 - Solution : *GLM math library, SOIL image library, and AssImp model
                        are ideal for this and future projects. Rotation is disc-
                        based with a vertical slide. Zoom uses mousewheel in spite
                        of Freeglut's fail in recognizing the mousewheel funcion.*  

**Stage 2** (skipped) : Ray trace the cube to give it a crystal appearance using GLSL.  
 - Challenge : *Ray-plane intersections should be done on the GPU, but after
                        calculating the destination vertex, how do we get the assoc.
                        tex coord from the VBO?  Shaders do not provide access to a
                        Vertex Buffer element from within another.*  
 - Solution : ~~Send a Uniform Buffer with vertex/texCoord pairings to the
                        shaders for real-time lookup.  Unfortunately, this increases
                        set-up time and is not scalable to larger systems or scenes.~~  
 - Problem : *While the Uniform Buffer solution is demonstrably possible
                        for small systems, it is hacky, complicated, and extremely
                        ugly.  And it will never port to anything greater.  Gave up
                        on doing this with GLSL.*

**Stage 3** (current) : Integrate OpenCL 1.2 to handle ray tracing in GPU kernels. [branched]  
 - Challenge : *How can we use the more ideal OpenCL API to solve our GLSL
                        problems?  How can we accomplish this within the context of
                        an OpenGL application?*  
 - Solution : *Unfortunately, this solution will be somewhat tedious.  OpenCL
                        can release its shared-memory buffers to OpenGL, but there seems
                        no way to integrate smoothly with the GLSL shading pipeline. We
                        must perform all transformations and fragment calculations and
                        render the output to a frame buffer to be copied over and swapped
                        out after the fragment shader step. We have no GLSL functions to
                        call and must rely on crafting the math from C.*

**Stage 4** (planned) : Add some simulation of water to this project.  
 - Challenge : *Water can be mesh-based or particle-based. Let's learn the
                        concept of particle systems and use that to simulate water
                        in our ray-traced scene.*  

**Stage 5** (planned) : Implement compute shaders to handle ray tracing in GPU kernels.  
 - Challenge : *OpenCL made parallel computation with global data possible
                        where it otherwise had not been. Khronos has now introduced
                        compute shaders using GLSL semantics to approximate the usage
                        of OpenCL with greater integration in the OpenGL program. This
                        is still brand new and must be explored.*

___

This project is an exploration of modern OpenGL concepts including:
  - GLSL 4.20
  - OpenGL 4.20
  - OpenCL 1.2
  - Freeglut Extended features
  - Modern libraries
  - Vertex/Uniform/Index/Frame Buffers
  - Lighting models
  - Mesh loading
  - Arcball-like control
  - Using a skybox
  - Texture loading, mapping, and sampling
  - Ray tracing**
  - Particle Systems**
  - Compute Shaders**

(**) denotes concepts yet to be implemented.

___

Design Hardware: &nbsp; Athlon II X4 640, 8GB RAM, Sapphire Radeon HD 6870  
Design Software: &nbsp; Eclipse CDT Juno on Kubuntu 12.10 Quantal

___

**Notes**:

Custom Program class
  - Loads user-specified shader files as Shaderobj objects.
  - Simplifies the most common calls in setup and execution.
  - Makes multiple shader programs much simpler.

Custom Quaternion class
  - Need to avoid gimbal lock.
  - Allows slerping (Spherical Linear intERPolation).
  - Understood just enough to put it together and make it robust.

Custom Mesh class
  - Uses AssImp to import object/model files.
  - Uses SOIL to load and bind the texture files read in from AssImp.
  - Automatically prepares VBO/IBOs from the imported scene data.

Matrix management
  - Deprecation of matrix stack and GLSL built-in variables.
  - All maintained by hand (with help from GLM and my quaternions).
  - School has only taught direct/immediate mode thus far.
