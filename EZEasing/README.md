![EZEasing](https://user-images.githubusercontent.com/25724155/124602780-e041a000-de71-11eb-82ab-5d1804a004f1.jpg)

# EZEasing
 
## Why?
	Why not?
	
## Example usage
```cpp
void update(float delta) {
	static float ratio = 0;
	float result = eze::easeOutCubic(90, 270, ratio);
	ratio += delta;
	
	if(ratio > 1) {
		ratio--;
	}
}

```


## Library design
 **EZEasing** is designed to be compiled into a `.lib` file. Compile and link with you own project.
 **EZEasing** has two types of functions in its namespace `eze`: standard functions and templated functions.
 
### Standard functions
All the standard functions in **EZEasing** are laid out as follows: 
 ```cpp
namespace eze {
	// ratio is in range [0-1]
	float easeLinear(float ratio);

	float easeInSine(float ratio);
	float easeOutSine(float ratio);
	float easeInOutSine(float ratio);

	float easeInQuad(float ratio);
	float easeOutQuad(float ratio);
	float easeInOutQuad(float ratio);

	float easeInCubic(float ratio);
	float easeOutCubic(float ratio);
	float easeInOutCubic(float ratio);

	float easeInQuart(float ratio);
	float easeOutQuart(float ratio);
	float easeInOutQuart(float ratio);

	float easeInQuint(float ratio);
	float easeOutQuint(float ratio);
	float easeInOutQuint(float ratio);

	float easeInExpo(float ratio);
	float easeOutExpo(float ratio);
	float easeInOutExpo(float ratio);

	float easeInCirc(float ratio);
	float easeOutCirc(float ratio);
	float easeInOutCirc(float ratio);

	float easeInBack(float ratio);
	float easeOutBack(float ratio);
	float easeInOutBack(float ratio);

	float easeInElastic(float ratio);
	float easeOutElastic(float ratio);
	float easeInOutElastic(float ratio);

	float easeInBounce(float ratio);
	float easeOutBounce(float ratio);
	float easeInOutBounce(float ratio);
}
 ```
 
### Templated functions
For each standard function, **EZEasing** provides a templated version that allows to define _start_ and _end_ points, like so: 
 ```cpp
float easeLinear(T start, T end, float ratio);

float easeInSine(T start, T end, float ratio);
float easeOutSine(T start, T end, float ratio);
float easeInOutSine(T start, T end, float ratio);
...
 ```
 
**EZEasing** can ease any type with a defined `T operator+(T)`, `T operator*(float)`. Any type defining those operators can be given as input to the templated functions. Primitives are always accepted.