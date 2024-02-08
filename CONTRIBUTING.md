# CONTRIBUTING

# HOW TO CONTRIBUTE

1. Read the coding standards below.
2. Keep PR simple and focused - one PR per feature.
3. Make a Pull Request.
4. Check back if any revisions are requested.

#### Tips

Feel free to contribute bug fixes or documentation fixes as pull request.

If you are looking for ideas what to work on, head to [Issues](https://github.com/aeris170/NeoDoa/issues) and checkout out open tickets or start a conversation. It is best to start conversation if you are going to make major changes to the engine or add significant features to get advice on how to approach it.

Try to keep PR focused on a single feature, small PR's are easier to review and will get merged faster. If your PR is too large, we'll want it get broken down to chunks of small features.

# CODING STANDARDS

### Keep it simple, stupid

Simple code is always better. Write for people not for the machine. The compiler will take care of the tiny optimizations for you.

### Use American English spelling

For example, use "Initialize" instead of "Initialise", and "color" instead of "colour".

### Use uniform initialization if and only if calling "ctor" for aggregate type

```cpp
glm::vec3 vector{ 1, 1, 1 };
```
instead of 
```cpp
glm::vec3 vector(1, 1, 1);
//or
glm::vec3 vector = glm::vec3(1, 1, 1);
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
if (true) {
    // do something
} else {
    // do something else
}
```

Always use braces, this applies to where braces can be omitted:

```cpp
// Notice the braces
if (true) {
    then();
}
```

### Keep simple ifs one line

```cpp
if (condition) { then(); }
else { then2(); }
```

### Keep empty function/struct bodies one line

```cpp
struct A {} // demonstration only, we shouldn't have empty structs

void Foo() {}
```

### Remove all trailing white spaces and ending line

```cpp
// Assume dots are spaces, WRONG
if (x == 0) {........
    then();.....
}.....
```

```cpp
// Assume dots are spaces, CORRECT
if (x == 0) {
    then();
}
```

On save, set your text editor to remove trailing white spaces.

### Use spaces between operators and put one space after commas

```cpp
int x = 16 + 32 / 4;

int x = RandomNumberBetween(0, 100);
```

### Prefer ++i over i++ when i is an iterator

```cpp
for(auto itr = someIterable.begin(); itr != someIterable.end(); ++itr) {
    // do something
}
```

### No spaces between (), [] and {} brackets, unless using uniform initialization

```cpp
foo();
bar(1, 2);
while(true);
for (;;)
if (false) {}
int b[5] { 8, 20, 25, 9, 14 };
int a[10];
glm::vec3 vector{ 1, 1, 1 };
```

### Define variables where they are first needed in a function

Variable declarations should only appear in a function when the variable itself is needed.

Variables should be declared one per line. (This one is permissive, there are times where you can break this rule, left it up to you.) 

```cpp
void foo() {
    std::string x = "I'm a string";
    cout << x << "\n";
    
    int y = 3;
    cout << y << "\n";
}
```

### Adopt the "early-return" policy

When checking for nullness, or parameter correctness, exit functions early. Be a never-nester.

```cpp
// Don't do this
void SomeFunc(int* x) {
    if (x != nullptr) { 
        /*
        *  Logic
        */
    }
};
```

```cpp
// Do this
void SomeFunc(int* x) {
    if (x == nullptr) { return; }
    
    /*
    *  Logic
    */
};
```

## Naming and Structure

Namespaces, global variables are banned. Global functions are preferred over where appropriate.

### Capitalization

```cpp
// Global functions names are PascalCase
void UselessFunction() {}

// Struct names are PascalCase
struct Sample {
    ...
};

// Member variable names are camelCase.
struct IntArray {

    int* arrayOfInts;
};

// Member functions names are PascalCase
struct Foo {
    void Bar();
};

// Constant names are PascalCase, separated by underscores.
constexpr int ThisIsAConstant = 5;
```

### Acronyms, if variable, should not be upper-case, otherwise they must be upper-case.

```cpp
int id = 1; // not "ID";

int GetID() { ... }; // not "GetId()"

CRUD *c = new CRUD(); // not "Crud";
```

## Privacy

### If there are no good reasons for encapsulation, then there shouldn't be any encapsulation.

If a member should be private and **trivial** getters and setters are to be provided; we don't do that here. "private" variables with setters are oxymoron and cause confusion, and are to be used with caution.

```cpp
// DON'T
class Point2D { // this is pointless to do and verbose with no good reason
    int GetX() const;
    void SetX(int x);
    int GetY() const;
    void SetY(int y);
private:
    int x;
    int y;
};
```

```cpp
// DO
struct Point2D {
    int x; 
    int y;
};
```

## Namespaces and Classes/Structs

Users of NeoDoa should not have to browse functionality namespace by namespace. Therefore, namespaces are banned. Any and all functionality of NeoDoa should be declared and defined in the global namespace. Usage of `detail` namespaces to hide implementation details are allowed, but should be kept to a minimum.

For the `class` keyword, developers should favor the `struct` keyword and put the public API to the top of the `struct`.

```cpp
// DON'T
class Something { // class - implicitly private members. usage of public: is mandatory
public:
    void DoWork(); 
    void DoWork2();
private:
    int data;
    int data2;
};
```

```cpp
// DO
struct Something { // struct - implicitly public members. usage of public: is redundant
    void DoWork(); 
    void DoWork2();
private:
    int data;
    int data2;
};
```

### Don't define member functions in the header, unless you have to

If a member function is to be defined in the header, there must be a good reason to do so.

Template functions are an example of a good reason.

Functions marked `inline` are an example of a bad reason. They have no positive benefits. They increase compile times.

```cpp
// hpp file
struct Example {
    void DoStuff(); // good

    inline void DoMoreStuff() { // go to the cpp file please...
        int x = 5; 
    }
	
    template<typename T>
    void DoEvenMoreStuff() { // valid reason to define in hpp
        int xxx = 999;
    }
};
```
