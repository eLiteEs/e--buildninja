# e--buildninja
Builder for C++
## How to use this
### Requirements
- 64-Bit Windows
- ```g++``` command should be useable from console.
- Good confidence about us because the ```.exe``` file isn't signed.
### Set up eBuildNinja and compile the project
1. Download the file "buildn.exe" to your project folder.
2. Set up the project tree like this:
    project :
    - src:
      - main.cpp
    - target:
    - build.ninja.inf
3. On build.ninja.inf file enter:
```
ex("main.exe" "main.cpp" "-static")
```
Explanation:
- ```ex``` : is a function for defining source files to compile.
- ```"main.exe"``` : is the output .exe file for
- ```"main.cpp"``` : the C++ source code.
- ```"-static"``` : adds an argument to ```g++``` compiler.
4. Now on the same directory where ```build.ninja.inf``` file is located, run ```buildn``` command.
5. You can see, if all the code is right, that inside the ```target``` folder is the compiled ```main.cpp``` file.

## Where is eBuildNinja used?
eBuildNinja is used on the IDE AliCode, for compiling C^8 and LaQTeOS and if you want, you can implement it on your C++ project.
