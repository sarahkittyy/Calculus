# Calculus
A one-header libary with an overwhelmingly creative name for lots of simple Calculus I & II concepts.

## Building
It's a single header file. If you really need build instructions, here are some simple steps on linux:

* `cd Calculus/`
* `cat Calculus.h | cat > Calculus.h`

Now just `#include "Calculus.h"` and you're set.

## Usage
See `Calculus.h` for all methods. 

See `docs/` for HTML doxygen documentation.

## Example
```c++
#include <iostream>
#include "Calculus.h"

int main()
{
	//f(x) = x^2
	calc::Func x_squared = [](double x)->double{
		return x*x;
	};

	//F(x) = x^3/3
	calc::Func x_squared_integral
		 = calc::integral(x_squared);

	for(int i = 0; i < 10; ++i)
	{
		std::cout << "F(" << i << ") = ";
		std::cout << x_squared_integral(i);
		std::cout << std::endl;
	}
	return 0;
}
```
#### Output

```
F(0) = 0
F(1) = 0.33333
F(2) = 2.66667
F(3) = 9
F(4) = 21.3333
F(5) = 41.6666
F(6) = 72
F(7) = 114.333
F(8) = 170.667
F(9) = 243
```

## Notes
Accuraccy could be improved but it's great for general-purpose.