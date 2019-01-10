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
#include <vector>
#include <cmath>
#include <utility>
#include <string>
#include <sstream>

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
	const double LARGE = 10000.0;

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
		for(double i = lower; i <= std::abs(upper); i+=SMALL)
		{
			ret += fx(i)*SMALL;
		}
		int sign = (upper < 0)?(-1):(1);
		return round(ret*sign, ACCURACY);
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
		return [=, &fx](double x)->double{
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
		return [=](double x)->double{
			return iterate(fx, times, x);
		};
	}

	//////////////////////////UTILS/////////////////////////////
	
	/**
	 * @brief A namespace of more utility & beauty functions than actual mathematical ones.
	 * 
	 */
	namespace util
	{
		/**
		 * @brief Class to beautifully plot functions directly in terminal.
		 * 
		 * @remarks The grossest code you'll ever read. I'm sorry. It's late. I haven't had coffee.
		 * 
		 */
		class Grapher
		{
		public:
			/**
			 * @brief Default Constructor.
			 * 
			 */
			Grapher()
			{
				//Default values.
				mTermWidth = 80;
				mTermHeight = 24;
				mXrange[0] = -10;
				mXrange[1] = 10;
				mYrange[0] = -10;
				mYrange[1] = 10;
			}

			/**
			 * @brief Set the dimensions, in characters, of the output graph.
			 * 
			 * @param width The width of the printed output.
			 * @param height The height of the printed output.
			 */
			void setOutputDimensions(int width, int height)
			{
				mTermWidth = width;
				mTermHeight = height;
			}
			
			/**
			 * @brief Set the domain of the function to render.
			 * 
			 * @param from The left end-point.
			 * @param to The right end-point.
			 */
			void setDomain(double from, double to) 
			{
				mXrange[0] = from;
				mXrange[1] = to;
			}

			/**
			 * @brief Set the range of the function to render.
			 * 
			 * @param from The bottom-most point.
			 * @param to The top-most point.
			 */
			void setRange(double from, double to)
			{
				mYrange[0] = from;
				mYrange[1] = to;
			}

			/**
			 * @brief Pushes another function to be rendered on display().
			 * 
			 * @param fx The function to render.
			 * @param funcChar The character to be used to draw the function.
			 */
			void addFunction(Func fx, char funcChar = '#')
			{
				mFunctions.push_back(std::make_pair(fx, funcChar));
			}

			/**
			 * @brief Resets all functions that were queued.
			 * 
			 */
			void clearFunctions()
			{
				mFunctions.clear();
			}

			/**
			 * @brief The main display call. Prints the final graph to the screen.
			 * 
			 * @remarks This is horribly programmed. It's a hacky, inconsistent, memory-draining cesspool. I'm sorry.
			 * 
			 * @see Hell
			 * 
			 */
			void display()
			{
				//Declare the screen, to be updated before the final print statement.
				char screen[mTermHeight][mTermWidth];

				//Clear the screen with ' ' characters.
				for(int i = 0; i < mTermHeight; ++i)
				{
					for(int j = 0; j < mTermWidth; ++j)
					{
						screen[i][j] = ' ';
					}
				}

				/*
				These are lambdas that convert between the coordinate systems of the terminal & function.
				*/
				auto pixelToX = [&](int val)->double{
					return mMap(val, 0, mTermWidth-1, mXrange[0], mXrange[1]);
				};
				//Unused, here for completion.
				/*auto pixelToY = [&](int val)->double{
					return mMap(val, 0, mTermHeight-1, mYrange[0], mYrange[1]);
				};*/
				auto xToPixel = [&](double val)->int{
					return int(mMap(val, mXrange[0], mXrange[1], 0, mTermWidth));
				};
				auto yToPixel = [&](double val)->int{
					return int(mMap(val, mYrange[0], mYrange[1], mTermHeight, 0));
				};
				/*
				------------
				*/

				//Get the positions of the axis, and make sure they're in-bounds (or don't render them)
				int xaxis = yToPixel(0);
				int yaxis = xToPixel(0);
				/*
				xaxis is the y-position of the xaxis, and vice-versa, which is why these comparisons seem off.
				Here we are just filling in the axis's with their respective character.
				*/
				if(yaxis >= 0 && yaxis < mTermWidth)
				{
					for(int i = 0; i < mTermHeight; ++i)
					{
						screen[i][yaxis] = '|';
					}
				}
				if(xaxis >= 0 && xaxis < mTermHeight)
				{
					for(int i = 0; i < mTermWidth; ++i)
					{
						screen[xaxis][i] = '-';
					}
				}
				/*
				--------------
				*/
				
				//Update the screen w/ the functions.
				for(auto &pair : mFunctions)
				{
					/*
					For every function we're going to:
					     * Go through each X value (Term coords).
						 	 * Get the respective function-coordinate x position
							 * Pass it through the function.
							 * Assert that the y-value is not out of range.
							 * Convert back to term coords.
							 * If the pixel is too far from the previous pixel (and it's not the first execute),
							 	 * Iterate through from the last pixel to the current pixel's y-coordinate.
								 * Place a character at all points passed.
								 This generates smooth lines.
							 * Update the last pixel.
							 * Set the pixel at the actual point. 
					*/

					//Set to max to avoid the first point being compared with whatever other initial value this may take on.
					//(Try setting this to 0 and rendering x cubed.)
					int last_pixel_y = INT32_MAX;

					//Iterate through all x-values (term coords).
					for(int i = 0; i < mTermWidth; ++i)
					{
						//Get the corresponding x-value in function-coordinates.
						double val = pixelToX(i);
						//Pass it through the function to get the output (y-value).
						double result = pair.first(val);

						//If the result is out of range, ignore and move on.
						if(result < mYrange[0] || result > mYrange[1])
						{
							continue;
						}
						
						//Otherwise, we're going to convert the point back to "pixel" coordinates.
						int pixel_y = yToPixel(result);

						//Checks if the last pixel is too far from the new pixel,
							//AND that it's not the first iteration that something was drawn (The INT32_MAX check).
						if((last_pixel_y > pixel_y || last_pixel_y < pixel_y) && last_pixel_y != INT32_MAX)
						{
							//For every pixel in between..
							for(int j = last_pixel_y; 
									j != pixel_y; 
									//Inc or dec based on which will terminate the loop >w<.
									j += (last_pixel_y > pixel_y) ? (-1) : (1))
							{
								//Set that pixel.
								screen[j][i] = pair.second;
							}
						}
						//Set the las drawn pixel.
						last_pixel_y = pixel_y;
						
						//And finally, actually, draw the real, actual pixel that was supposed to be drawn.
						screen[pixel_y][i] = pair.second;
					}
				}

				//Render everything in one std::cout call.
				std::stringstream ss;
				for(int i = 0; i < mTermHeight; ++i)
				{
					for(int j = 0; j < mTermWidth; ++j)
					{
						//Push every character into a string stream...
						ss << screen[i][j];
					}
					//..append newlines as needed...
					ss << '\n';
				}
				//..and print!
				std::cout << ss.str();
			}
		private:
			std::vector<std::pair<Func, char>> mFunctions;
			int mTermWidth;
			int mTermHeight;
			double mXrange[2];
			double mYrange[2];

			long mMap(long value, long lower, long upper, long newlower, long newupper)
			{
				return (value - lower) * (newupper - newlower) / (upper - lower) + newlower;
			}
		};
	}
}