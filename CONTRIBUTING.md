# CONTRIBUTING

# HOW TO CONTRIBUTE

1. Read the coding standards below.
2. Keep PR simple and focused - one PR per feature.
3. Make a Pull Request.
4. Check back in any revisions are requested.
5. ???
6. Profit :)

#### Tips

Feel free to contribute bug fixes or documentation fixes as pull request.

If you are looking for ideas what to work on, head to [Issues](https://github.com/aeris170/neodoa/issues) and checkout out open tickets or start a conversation. It is best to start conversation if you are going to make major changes to the engine or add significant features to get advice on how to approach it.

Try to keep PR focused on a single feature, small PR's are easier to review and will get merged faster. If your PR is too large, we'll want it get broken down to chunks of small features.

# CODING STANDARDS

### Keep it simple, stupid

Simple code is always better. Write for people not for the machine. The compiler will take care of the tiny optimizations for you.

### Use American English spelling

For example, use "Initialize" instead of "Initialise", and "color" instead of "colour".

### Use uniform initialization for initialization

```cpp
glm::vec3 *vector{ new glm::vec3(1, 1, 1) };
```
instead of 
```cpp
glm::vec3 *vector = new glm::vec3(1, 1, 1);
```

### Opening braces should be on the same line as the statement

For example:

```cpp
// Notice there is no new line before the opening brace
int main() {
    return 0;
}
```

Also use the following style for 'if' statements:

```cpp
if(true) {
    // do something
} else {
    // do something else
}
```

Always use braces, this applies to where braces can be omitted:

```cpp
// Notice the braces
if(true) {
    then();
}
```

### Use tabs for indentation

This is pretty much self explanatory. We prefer tabs over spaces.

### Remove all trailing white spaces and ending line

```cpp
// Replace dots with spaces, WRONG
if (x == 0) {........
    then();.....
}.....
```

```cpp
// Replace dots with spaces, CORRECT
if (x == 0) {
    then();
}
```

On save, set your text editor to remove trailing white spaces and ensure there is no empty line at the end of the file.

### Use spaces between operators and commas

```cpp
int x = 16 + 32 / 4;

int x = RandomNumberBetween(0, 100);
```

### Prefer ++i over i++ when i is not needed during incrementation process

```cpp
// Here we don't care about i's previous value, we care about it's current value
for(int i = 0; i < 5; ++i) {
    // do something
}
```

### No spaces between (), [] and {} brackets, unless using uniform initialization

```cpp
foo();
bar(1, 2);
while(true);
for(;;)
if(false) {}
int b[5] {8, 20, 25, 9, 14};
int a[10];
glm::vec3 *vector{ new glm::vec3(1, 1, 1) };
```

### Define variables where they are first needed in a function

Variable declarations should only appear in a function when the variable itself is needed.

Variables should be declared one per line. (This one is permissive, there are times where you can break this rule, we leave it up to you.) 

```cpp
void foo() {
    std::string x = "I'm a string";
    cout << x << "\n";
    
    int x = 3;
    cout << x << "\n";
}
```

### Prefer "\n" over std::endl

Performance wise \n is a tiiiiiiny bit faster.

```cpp
void foo() {
    std::string x = "I'm a string";
    cout << x << std::endl; // don't do this
    
    std::string x = "I'm a string";
    cout << x << "\n"; // do this
}
```

### Use only one return on logic

Don't exit functions early, let it naturally fall into the return statement at the end.

```cpp
// Don't do this
void IncrementIfNotZero(int x) {
    if(x == 0) {
        return x;
    }
    return x + 1;
};
```

```cpp
// Do this
void IncrementIfNotZero(int x) {
    if(x != 0) {
        ++x;
    }
    return x;
};
```

## Naming

The entire NeoDoa API must be declared under the ```doa``` and ```internal``` namespaces.

### Capitalization

Follow these rules when working under ```doa``` namespace.

```cpp
// Namespace should have short, one word lowercase names
namespace doa::camera {}

// Global variables should be snake_case
namespace doa::camera {
    float cam_x = 1.f;
}

// Global functions should be PascalCase
namespace doa::camera {

    float cam_x = 1.f;

    void UselessFunction() {}
}

// Classes should be PascalCase
class SampleClass {
    ...
};

// Member variables should start with m_ prefix and be snake_case
class IntArray {
private:
    int* m_array_of_ints;
};

// Member functions should be PascalCase
class Direction {
public:
    void Foo();
};

// Constants should be ALL_CAPITALS separated by underscores.
int THIS_IS_A_CONSTANT_NUMBER = 5;
```

Follow these rules when working under ```internal``` namespace.

```cpp
// Namespace should have short, one word lowercase names
namespace internal::camera {}

// Global variables should be snake_case
namespace internal::camera {
    float some_internal_data_the_end_user_will_never_need = 1.f;
}

// Global functions should be snake_case
namespace internal::camera {
    
    float some_internal_data_the_end_user_will_never_need = 1.f;

    void useless_function() {}
}

// Constants should be ALL_CAPITALS separated by underscores.
int THIS_IS_A_CONSTANT_NUMBER = 5;
```

### Acronyms, if variable, should not be upper-case, otherwise they must be upper-case.

```cpp
int id = 1; // not "ID";

int GetID() { ... }; // not "GetId()"

CRUD *c = new CRUD(); // not "Crud";
```

### Cache the 'this' reference as '\_this'. This is the only time beginning a name with "\_" is allowed.

It is sometimes useful to be able to cache the 'this' object (for effortless singleton). If you need to do this, cache it in a variable called '\_this'.

```cpp
_this = this;
```

## Privacy

### Make member variables private, else make the class a struct

Variables should be privatized. If you don't want to provide getters and setters, yet still want to access to the variables, make the class a struct.
```cpp
class Point2D {
private:
    int x;
    int y;
};
```
or
```cpp
struct Point2D {
    int x;
    int y;
};
```
It is better to choose struct over a class when representing smaller data, like a point. Use classes for complex types such as a Player or an NPC.

## Namespaces and Classes

Again, the entire NeoDoa API must be declared under the ```doa``` and ```internal``` namespaces.  
All the classes, functions and variables under ```doa``` should be exposed to outside with ```DOA_API``` keyword.  
Any and all unexposed parts must go to ```internal``` namespace.  
```internal``` namespace can only have functions and variables. Therefore **don't** define any classes there.  
```internal``` namespace should only be used when a function or a variable is needed for the engine coder but not for the game developer.  
Take a look at the example below:

```cpp
// Namespace doa has the functionality the game dev needs.
// Create a new shader
// Get a shader by name
namespace doa::shader {

    typedef GLuint ShaderProgram;

    extern DOA_API std::map<std::string, ShaderProgram*> SHADERS;

    DOA_API ShaderProgram* const CreateShaderProgram(const std::string& name, const std::string& pathToVertexShader, const std::string& pathToFragmentShader);

    DOA_API ShaderProgram* const Get(const std::string& name);
}

//Namespace internal has the functionality no game dev needs.
//Linking and validating shaders are not the job of the game dev, but it is the job of the engine.
namespace internal::shader {

    typedef GLuint ShaderProgram;

    GLuint add_shader_to(ShaderProgram shaderProgram, const std::string& name, const char *shaderCode, GLenum shaderType);
    void link_and_validate_shader_program(ShaderProgram shader, const std::string& name);

    void purge();
}
```

### Defining functions in the header

If a member function is to be defined in the header, it should be tagged virtual inline const. If your function cannot satisfy these requirements, define it in the corresponding .cpp file.

```cpp
class Direction {
private:
    bool m_is_north;
public:
    virtual inline const bool IsNorth() { return m_is_north; }
};
```

Bonus: **Don't abuse the mutable keyword on members just to define a function in the header!**