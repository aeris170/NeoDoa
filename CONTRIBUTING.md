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

### Use uniform initialization if and only if calling "ctor" for aggregate type

```cpp
glm::vec3 *vector{ 1, 1, 1 };
```
instead of 
```cpp
glm::vec3 *vector = new glm::vec3(1, 1, 1);
//or
glm::vec3 *vector{ new glm::vec3(1, 1, 1) };
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

### Keep simple ifs one line

```cpp
if(condition) { then(); }
else { then2(); }
```

### Keep empty function/struct bodies one line

```cpp
struct A {}

void Foo() {}
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

### Use spaces between operators and put one space after commas

```cpp
int x = 16 + 32 / 4;

int x = RandomNumberBetween(0, 100);
```

### Prefer ++i over i++ when i is an iterator

```cpp
// Assume i is an iterator :P
for(int i = 0; i < 5; ++i) {
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
glm::vec3 *vector{ 1, 1, 1 };
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

### Adopt the "early-return" policy

Exit functions early if needed, however, make sure no logic takes place before the exits.

```cpp
// Don't do this
int IncrementIfNotZero(int x) {
    if(x != 0) { ++x; }
    return x;
};
// or this
void IncrementIfNotZero(int x) {
    x++; // logic before "early-return" is a NO-NO
    if(x - 1 == 0) { return 0; }
    return x;
};
```

```cpp
// Do this
void IncrementIfNotZero(int x) {
    if(x == 0) { return x; }
    return x + 1;
};
```

## Naming and Structure

Namespaces, classes (use struct keyword), private members (see below), and global variables are banned.  Global functions are preferred instead of Factory pattern.

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
	
    // If however, the member should be private (read-only, from outside the class), an '_' character is appended to its name.
    int _size;
};

// Member functions names are PascalCase
struct Foo {
    void Bar();
};

// Constant names are ALL_CAPITALS, separated by underscores.
constexpr int THIS_IS_A_CONSTANT_NUMBER = 5;
```

### Acronyms, if variable, should not be upper-case, otherwise they must be upper-case.

```cpp
int id = 1; // not "ID";

int GetID() { ... }; // not "GetId()"

CRUD *c = new CRUD(); // not "Crud";
```

### Cache the 'this' reference as '\_this' when implementing the singleton pattern.

```cpp
class Singleton {
	static Singleton* _this;
	
	Singleton() {
		if(_this) throw "This is a singleton!"
		
		_this = this;
	}
};
```

## Privacy

### DON'T make member variables private, mark them as "read-only"

If a member should be private and appropriate getters are to be provided, we say "fuck that" to that and make it public anyways,
but append an '\_' character in front of the variable's name. Variables that have an '\_' character in front of their names are to be
considered read-only from outside of the class.
```cpp
// DON'T
class Point2D { //now you see why class is also banned :)
private:
    int x;
    int y;
};
```
or
```cpp
// DO
struct Point2D {
    int x;
    int y;
};
```

## Namespaces and Classes

Again, namespaces and classes are banned. End of the story.  

### Don't define member functions in the header, unless you have to

If a member function is to be defined in the header, you better have a good reason to do so.

```cpp
struct Example {

    bool foo;

    void DoStuff(); // good

    void DoMoreStuff() { // go to the cpp file please...
        int x = 5; 
    }
	
    template<typename T>
    void DoEvenMoreStuff() { // good
        int xxx = 999;
    }
};
```
