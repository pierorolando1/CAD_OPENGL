## 3D CAD System

![image](https://i.imgur.com/lK8crX3.png)

### **Program Execution**
Using the makefile, the program can be executed via:
* `all`: Run `make`

To remove the executable run:
* `clean`: Run `make clean`

### **Main Program**
**External Libraries Used**:
* iostream
    * Used for basic input and output
* random
    * Used in the generation of random values within a passed range
* cmath
    * Used for mathematical functions
* cfloat
    * Used for floating point numbers

#### **Notes:**
**PLEASE NOTE: The bindings are CASE SENSITIVE**

A list of the key bindings and the action they pertain to:

**MOUSE COMMANDS**
* Left-click - Select object
* Right-click - Delete object

**KEYBOARD COMMANDS (case sensitive)**
* q and Q - Quit Program
* '-' - Toggle camera movement mode on and off
* 1 - Change global material to material 1
* 2 - Change global material to material 2
* 3 - Change global material to material 3
* 4 - Change global material to material 4
* 5- Change global material to material 5
* r - Reset the scene
* v - Draw a new cube at the origin
* b - Draw a new sphere at the origin
* n - Draw a new cone at the origin
* m - Draw a new teapot at the origin
* , - Draw a new tetrahedron at the origin
* . - Draw a new icosahedron at the origin
* / - Draw a new snowman at the origin (bonus feature)
* s - Save the current scene to a file
* l - Load a scene from a previously saved file
* i - Move camera in the negative X direction (if camera movement mode enabled)
* o - Move camera in the positive X direction (if camera movement mode enabled)
* p - Move camera in the negative Y direction (if camera movement mode enabled)
* [ - Move camera in the positive Y direction (if camera movement mode enabled)
* ] - Move camera in the negative Z direction (if camera movement mode enabled)
* \ - Move camera in the positive Z direction (if camera movement mode enabled)
* M - Apply global material to selected object
* x - Move selected object in positive X direction
* X - Move selected object in negative X direction
* y - Move selected object in positive Y direction
* Y - Move selected object in negative Y direction
* z - Move selected object in positive Z direction
* Z - Move selected object in negative Z direction
* Ctrl + x - Scale selected object in positive X direction
* Ctrl + Shift + x - Scale selected object in negative X direction
* Ctrl + y - Scale selected object in positive Y direction
* Ctrl + Shift + y - Scale selected object in negative Y direction
* Ctrl + z - Scale selected object in positive Z direction
* Ctrl + Shift + z - Scale selected object in negative Z direction
* Alt + x - Rotate selected object in positive X direction
* Alt + Shift + x - Rotate selected object in negative X direction
* Alt + y - Rotate selected object in positive Y direction
* Alt + Shift + y - Rotate selected object in negative Y direction
* Alt + z - Rotate selected object in positive Z direction
* Alt + Shift + z - Rotate selected object in negative Z direction
* UP_ARROW - Rotate entire scene negative 5 units on the Z axis
* DOWN_ARROW - Rotate entire scene positive 5 units on the Z axis
* RIGHT_ARROW - Rotate entire scene negative 5 units on the Y axis
* LEFT_ARROW - Rotate entire scene positive 5 units on the Y axis
#### **Features:**
**CAD System**
* This program implements a fully functioning CAD system
* Shapes can be added and deleted, rotated, scaled and translated
* 5 materials available for the objects
* Scene can be saved and loaded

**Additional Custom Shape**
* Additional Custom Shape feature (number 13 in list)
* Clicking '/' adds a snowman shape to the scene
* Snowman is loaded from an obj file

**Light Source Ray Picking**
* Additional feature of light source ray picking (number 12 in list)
* Lights are not moved with keyboard, they can be selected with mouse and and translated the same way that objects can (with the x, X, y, Y, z, Z keys)
* Bounding box is a wire sphere instead of a wire cube

**Save/Load GUI**
* Rather than typing the name of the file for saving and loading files in the terminal,
a GUI has been implemented
* When s or l are clicked, a GUI appears and the letters the user types appear in the window
* To exit window, click outside of it
* Backspace also works to delete characters
* Do not need to append ".txt" when loading and saving files, just type the name of the file (e.g if file is hello.txt, type hello)
* Click Enter key when done typing text

#### **Design Decisions**
* Loading clears the current scene and loads the other scene
* .txt is automatically appended to the filenames when saving and loading
* To remove a character when typing the file name for save/load must click backspace key
* Objects can not be translated outside of the walls in the x and z directions
* Objects cannot be translated below the floor, but they can be infinitely moved upwards in the y as there is no roof
* The 3 textures are applied as follows: one to the floor (stone), one to the walls (brick), and one to any teapots that are drawn in the scene (marble)
* There are 5 materials to choose from, select 1-5 on keyboard, selecting a material
will draw any subsequently added objects to the scene with the selected material
* An object does not need to be selected to delete it, just right-click on it even if it is not selected and it will delete
* Lights cannot be moved with the keyboard due to the additional feature of ray-sphere intersection
* Lights can be selected via the mouse, and translated the same way that objects can (with the x, X, y, Y, z, Z keys)