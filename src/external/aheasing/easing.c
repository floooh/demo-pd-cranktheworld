//
// This is free and unencumbered software released into the public domain.
//
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
//
// For more information, please refer to <https://unlicense.org>
//

#include <math.h>
#include "easing.h"

#define M_PIf 3.14159265358979323846f
#define M_PI_2f 1.57079632679489661923f

// Modeled after the line y = x
AHFloat LinearInterpolation(AHFloat p)
{
	return p;
}

// Modeled after the parabola y = x^2
AHFloat QuadraticEaseIn(AHFloat p)
{
	return p * p;
}

// Modeled after the parabola y = -x^2 + 2x
AHFloat QuadraticEaseOut(AHFloat p)
{
	return -(p * (p - 2));
}

// Modeled after the piecewise quadratic
// y = (1/2)((2x)^2)             ; [0, 0.5)
// y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
AHFloat QuadraticEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		return 2 * p * p;
	}
	else
	{
		return (-2 * p * p) + (4 * p) - 1;
	}
}

// Modeled after the cubic y = x^3
AHFloat CubicEaseIn(AHFloat p)
{
	return p * p * p;
}

// Modeled after the cubic y = (x - 1)^3 + 1
AHFloat CubicEaseOut(AHFloat p)
{
	AHFloat f = (p - 1);
	return f * f * f + 1;
}

// Modeled after the piecewise cubic
// y = (1/2)((2x)^3)       ; [0, 0.5)
// y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
AHFloat CubicEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		return 4 * p * p * p;
	}
	else
	{
		AHFloat f = ((2 * p) - 2);
		return 0.5f * f * f * f + 1.0f;
	}
}

// Modeled after the quartic x^4
AHFloat QuarticEaseIn(AHFloat p)
{
	return p * p * p * p;
}

// Modeled after the quartic y = 1 - (x - 1)^4
AHFloat QuarticEaseOut(AHFloat p)
{
	AHFloat f = (p - 1);
	return f * f * f * (1 - p) + 1;
}

// Modeled after the piecewise quartic
// y = (1/2)((2x)^4)        ; [0, 0.5)
// y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
AHFloat QuarticEaseInOut(AHFloat p) 
{
	if(p < 0.5f)
	{
		return 8 * p * p * p * p;
	}
	else
	{
		AHFloat f = (p - 1);
		return -8 * f * f * f * f + 1;
	}
}

// Modeled after the quintic y = x^5
AHFloat QuinticEaseIn(AHFloat p) 
{
	return p * p * p * p * p;
}

// Modeled after the quintic y = (x - 1)^5 + 1
AHFloat QuinticEaseOut(AHFloat p) 
{
	AHFloat f = (p - 1);
	return f * f * f * f * f + 1;
}

// Modeled after the piecewise quintic
// y = (1/2)((2x)^5)       ; [0, 0.5)
// y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
AHFloat QuinticEaseInOut(AHFloat p) 
{
	if(p < 0.5f)
	{
		return 16 * p * p * p * p * p;
	}
	else
	{
		AHFloat f = ((2 * p) - 2);
		return  0.5f * f * f * f * f * f + 1;
	}
}

// Modeled after quarter-cycle of sine wave
AHFloat SineEaseIn(AHFloat p)
{
	return sinf((p - 1) * M_PI_2f) + 1;
}

// Modeled after quarter-cycle of sine wave (different phase)
AHFloat SineEaseOut(AHFloat p)
{
	return sinf(p * M_PI_2f);
}

// Modeled after half sine wave
AHFloat SineEaseInOut(AHFloat p)
{
	return 0.5f * (1 - cosf(p * M_PIf));
}

// Modeled after shifted quadrant IV of unit circle
AHFloat CircularEaseIn(AHFloat p)
{
	return 1 - sqrtf(1 - (p * p));
}

// Modeled after shifted quadrant II of unit circle
AHFloat CircularEaseOut(AHFloat p)
{
	return sqrtf((2 - p) * p);
}

// Modeled after the piecewise circular function
// y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
// y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
AHFloat CircularEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		return 0.5f * (1 - sqrtf(1 - 4 * (p * p)));
	}
	else
	{
		return 0.5f * (sqrtf(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	}
}

// Modeled after the exponential function y = 2^(10(x - 1))
AHFloat ExponentialEaseIn(AHFloat p)
{
	return (p == 0.0f) ? p : powf(2, 10 * (p - 1));
}

// Modeled after the exponential function y = -2^(-10x) + 1
AHFloat ExponentialEaseOut(AHFloat p)
{
	return (p == 1.0f) ? p : 1 - powf(2, -10 * p);
}

// Modeled after the piecewise exponential
// y = (1/2)2^(10(2x - 1))         ; [0,0.5)
// y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
AHFloat ExponentialEaseInOut(AHFloat p)
{
	if(p == 0.0f || p == 1.0f) return p;
	
	if(p < 0.5f)
	{
		return 0.5f * powf(2, (20 * p) - 10);
	}
	else
	{
		return -0.5f * powf(2, (-20 * p) + 10) + 1;
	}
}

// Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
AHFloat ElasticEaseIn(AHFloat p)
{
	return sinf(13 * M_PI_2f * p) * powf(2, 10 * (p - 1));
}

// Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
AHFloat ElasticEaseOut(AHFloat p)
{
	return sinf(-13 * M_PI_2f * (p + 1)) * powf(2, -10 * p) + 1;
}

// Modeled after the piecewise exponentially-damped sine wave:
// y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
// y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
AHFloat ElasticEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		return 0.5f * sinf(13 * M_PI_2f * (2 * p)) * powf(2, 10 * ((2 * p) - 1));
	}
	else
	{
		return 0.5f * (sinf(-13 * M_PI_2f * ((2 * p - 1) + 1)) * powf(2, -10 * (2 * p - 1)) + 2);
	}
}

// Modeled after the overshooting cubic y = x^3-x*sin(x*pi)
AHFloat BackEaseIn(AHFloat p)
{
	return p * p * p - p * sinf(p * M_PIf);
}

// Modeled after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
AHFloat BackEaseOut(AHFloat p)
{
	AHFloat f = (1 - p);
	return 1 - (f * f * f - f * sinf(f * M_PIf));
}

// Modeled after the piecewise overshooting cubic function:
// y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
// y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
AHFloat BackEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		AHFloat f = 2 * p;
		return 0.5f * (f * f * f - f * sinf(f * M_PIf));
	}
	else
	{
		AHFloat f = (1 - (2*p - 1));
		return 0.5f * (1 - (f * f * f - f * sinf(f * M_PIf))) + 0.5f;
	}
}

AHFloat BounceEaseIn(AHFloat p)
{
	return 1 - BounceEaseOut(1 - p);
}

AHFloat BounceEaseOut(AHFloat p)
{
	if(p < 4/11.0f)
	{
		return (121 * p * p)/16.0f;
	}
	else if(p < 8/11.0f)
	{
		return (363/40.0f * p * p) - (99/10.0f * p) + 17/5.0f;
	}
	else if(p < 9/10.0f)
	{
		return (4356/361.0f * p * p) - (35442/1805.0f * p) + 16061/1805.0f;
	}
	else if (p < 1.0f)
	{
		return (54/5.0f * p * p) - (513/25.0f * p) + 268/25.0f;
	}
	else
	{
		return 1.0f;
	}
}

AHFloat BounceEaseInOut(AHFloat p)
{
	if(p < 0.5f)
	{
		return 0.5f * BounceEaseIn(p*2);
	}
	else
	{
		return 0.5f * BounceEaseOut(p * 2 - 1) + 0.5f;
	}
}
