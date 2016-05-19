
Welcome to the Shader project for CS 384g Computer Graphics (Fall 2012)

In this project, you'll write GLSL shaders.  These shaders will:

  *  roll up a flat 2D grid into the shape a torus (doughnut) in a
     vertex shader

  *  implement ambient, diffuse, and specular light contribution

  *  sample a surface decal stored in a 2D texture

  *  implement per-fragment normal mapping

  *  implement reflection mapping into a cube map environment

Build instructions (lab machines):

  in this directory

  make debug      # builds debug version
  make release    # builds release version
  make both       # build both versions

Build instructions (Apple):

  in this directory

  make CFG=debug -f MACmakefile     # builds debug version
  make CFG=release -f MACmakefile   # builds release version

  Note:  You'll need the boost libraries to compile this on your
  machine.  If you don't already have them, you can use Macports to
  get them or build them yourself.  These build instructions assume a
  Macports installation or equivalent.

Build instructions (Windows 7, VS2010):  Make sure you have installed boost
  in C:\Program Files\boost (the current solution file assumes that it is
  version 1.51, so the C++ Additional Include Files project property may
  need to be modified from its current C:\Program Files\boost\boost_1_51 if
  you install another version or put it somewhere else).  Also make sure
  you've installed a recent OpenGl SDK from the manufacturer of your graphics
  card if it isn't already there (the current solution file assumes it is
  the NVIDIA OpenGl SDK, I used version 10.36 but other recent ones should
  work).  Build from this directory where the solution file is, and note
  that the Release build creates a shader_scene.exe file in this directory, and
  the Debug build creates a shader_scene_debug.exe file here.  If you use
  the copies in the Release and Debug subdirectories, they won't find the
  shaders and textures that are located in the glsl and tga subdirectories.

Starting the project:

  Run the project (making sure the executable is up-to-date):

  make rrun     # run the release version
  make drun     # run the debug version
  make gdb      # run the debug version in the gdb debugger

  Or run the executables directly

  ./bin.release64/shader_scene
  ./bin.debug/shader_scene -gldebug

  (If you see a red square, that's good; your mission is to "fix"
  that red square and make it a torus with the proper shading.)

Controlling the project:

  Left mouse button rotates eye point around object

  Ctrl+Left mouse button spins the object itself

  Middle mouse button rotates light source around object

  Right mouse button shows pop-up menu

  'w' key toggles wireframe

  'v' key toggles vertical refresh synchronization (on by default)

  '0' through '9' keys switch shaders (also possible from menu)

What you need to do:

  *  Fix the vertex shader to fold the grid into a torus

  *  Fix the vertex shader to send the varying values needed by
     your fragment shaders

  *  Fix the fragment shaders glsl/0*_*.frag as described in the PDF

  *  Fix texture.c's computeNormal routine to convert a height map
     texel into a normal map texel (by computing gradient with neighbor
     height field texels)

How to do this?

  *  Edit fragment shaders in glsl/*.frag

  *  Edit the glsl/torus.vert

  *  Fix computeNormal

  Other than fixing computeNormal, you should not need to modify the
  C/C++ code -- but you are free to do so.

Questions?

  Ask on piazza.com

- Don Fussell
  November 7, 2012
