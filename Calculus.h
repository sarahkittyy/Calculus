/*////////////////////////////////////////////////

MIT License

Copyright (c) 2019 Sarah

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/////////////////////////////////////////////////

#include <functional>
#include <cmath>

/**
 * @brief Namespace where everything is defined.
 * 
 */
namespace calc
{
	/////////////////////////GLOBALS/////////////////////////////////////

	/**
	 * @brief A large integer to *simulate* infinity, because I'm lazy.
	 * 
	 * @remarks This is the only constant that should be changed in this code.
	 * 
	 */
	const double LARGE = 1000000.0;

	/**
	 * @brief A small value.
	 * 
	 * @see LARGE
	 * 
	 */
	const double SMALL = 1/LARGE;

	/**
	 * @brief Digits of accuracy to round to. Don't expect numbers more accurate.
	 * 
	 */
	const int ACCURACY = std::log10(LARGE)-1;

	////////////////TYPEDEFS///////////////

	/**
	 * @brief For simplicity.
	 * 
	 */
	typedef std::function<double(double)> Func;

	/////////////////////////METHODS/////////////////////////////////////

	/**
	 * @brief Rounding function with amount of digits to round to.
	 * 
	 * @param value The value to round.
	 * @param places The amount of places to round to.
	 * @return double The rounded value.
	 */
	constexpr double round(double value, unsigned places)
	{
		int prod = std::pow(10, places);
		return std::floor(value * prod + 0.5) / prod;
	}

	/**
	 * @brief Returns the derivative of a function as a callable function.
	 * 
	 * @param fx The function to take the derivative of.
	 * @return Func The derivative.
	 */
	Func derivative(Func fx)
	{
		return [&](double x)->double{
			return round(
				(fx(x+SMALL)-fx(x))*LARGE,
				ACCURACY
			);
		};
	}

	/**
	 * @brief Calculates the definite integral of a function.
	 * 
	 * @param fx The function to take the definite integral of.
	 * @param lower The lower bound.
	 * @param upper The upper bound.
	 * @return double The indefinite integral.
	 */
	double integral_definite(Func fx, double lower, double upper)
	{
		double ret = 0;
		for(double i = lower; i <= upper; i+=SMALL)
		{
			ret += fx(i)*SMALL;
		}
		return round(ret, ACCURACY);
	}

	/**
	 * @brief Returns the indefinite integral of a function as a callable lambda.
	 * 
	 * @param fx The lambda/function to integrate.
	 * @param valid_value A value within the function's domain.
	 * @return Func The integral of the function.
	 * 
	 * @remarks Changing valid_value will change the results of the integral by a constant.
	 * 		The indefinite integral is calculated as the integral from valid_value to X of the function.
	 */
	Func integral(Func fx, double valid_value = 0)
	{
		return [&, valid_value](double x)->double{
			return integral_definite(fx, valid_value, x);
		};
	}

	/**
	 * @brief Uses Newton's method to approximate the roots of a function.
	 * 
	 * @param fx The function to calculate the roots of.
	 * @param initial The initial value. The closer it is to the root, the less iterations required to reach it.
	 * @param iter The amount of iterations to do.
	 * @return double The approximation of the nearest root.
	 * 
	 * @remarks TODO: Check if the values repeat over iterations, and save processing time by returning.
	 */
	double roots(Func fx, double initial = 0, unsigned iter = 100)
	{
		if(iter == 0)
		{
			return initial;
		}
		else
		{
			return roots(
				fx,
				initial-(fx(initial)/derivative(fx)(initial)), 
				iter-1
			);
		}
	}

	/**
	 * @brief Lambert W function approximation, which is the inverse function of x*e^x
	 * 
	 * @param value Input.
	 * @return double Output.
	 */
	double lambertW(double value)
	{
		return roots(
			[=](double x)->double{
				return x*std::exp(x)-value;
			},
			value,
			150
		);
	}

	/**
	 * @brief Iterate a function over a value an amount of times.
	 * 
	 * @param fx The function to iterate. 
	 * @param times The amount of times to iterate.
	 * @param value The value to iterate over.
	 * @return double The result.
	 */
	double iterate(Func fx, double times, double value)
	{
		if(times <= 0)
		{
			return value;
		}
		else return fx(iterate(fx, times-1, value));
	}

	/**
	 * @brief Like iterate(), except generalized by returning a lambda for all values.
	 * 
	 * @param fx The function to iterate.
	 * @param times The amount of times to iterate.
	 * @return Func A lambda composed of the iterated function.
	 * 
	 * @see iterate()
	 */
	Func iterated(Func fx, double times)
	{
		return [&](double x)->double{
			return iterate(fx, times, x);
		};
	}
}