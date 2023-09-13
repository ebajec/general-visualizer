# general-visualizer
Basic 3D rendering components I have made from scratch with OpenGL. Currently, this is able to generate and render meshes for parameterized shapes in 3D. The parameterization must be explicity specified in main.cpp, but I plan to change this.  My apologies for the lack of documentation.

My goal here was to create a generalized framework in which I can visualize geometric objects.     

**To compile:** In the solution file or makefile, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and properly linked in CMakeLists.txt. I have included the headers and some of the binaries for portability purposes.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene. There are a few other commands, and I will probably add more. 

*close:* closes the scene being rendered.

*exit:* exits the program.
