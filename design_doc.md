# Data Creator Design Overview

This program will create a data set that fulfills certain conditions. The conditions that will be passed to it will be 
the mean, minimum, maximum, standard deviation, and count (number of elements).

## Provided value breakdown

- $\sigma = \sqrt{\frac{\sum_{i=1}^{N} (x_i - \mu)^2}{N}}$
	- σ: Population standard deviation (provided)
	- μ: Population mean (provided) 
	- $x_{i}$: Each individual data point (need to generate [besides $x_{min}$ and $x_{max}$])
	- N: Total number of data points in the population (provided)
	- Σ: Sum of all values

## Potential solution

A potential solution could be an algorithmic approach that is similar to training a neural network. The program would
start with an array of size N containing $x_{min}$, $x_{max}$, and random values in between.

There will be a loss function; the goal of the program is to minimize loss. The loss equation would be
$\(\mu_{current} - \mu_{target}\)^2 + \(\sigma_{current} - \sigma_{target}\)^2$

The program would then loop and apply small, random adjustments to the elements (excluding min and max). If a change
decreases loss, keep it; if it increases loss, discard it. The loop would end when Loss is near 0 (some small value like 0.00001)

## Function overview

- `DataCalculator` - namespace for static calculation functions (average and standard deviation)
- `init_values` - initializes the list by inserting the min, max, and random values in between
- `tweak_values` - modifies the list to try to minimize loss
- `calculate_loss` - calculates loss using the above equation
