# Data Creator Design Overview

This program will create a data set that fulfills certain conditions. The conditions that will be passed to it will be 
the mean, minimum, maximum, standard deviation, and count (number of elements).

## Useful equations

- Linear rescaling transformation: $y_{i} = \mu_{target} + \(x_{i} - \mu_{current}\) * \(\frac{\sigma_{target}}{\sigma_{current}}\)$
- Standard deviation: $\sigma = \sqrt{\frac{\sum_{i=1}^{N} (x_i - \mu)^2}{N}}$

## [Initial attempted solution](https://github.com/tristanzat/cse310-cpp-data-creator/blob/5a1957d64d24b8fb5dc4e0ef6ce4a79be4c674e3/data_creator/data_creator.cpp)

A potential solution could be an algorithmic approach that is similar to training a neural network. The program would
start with an array of size N containing $x_{min}$, $x_{max}$, and random values in between.

There will be a loss function; the goal of the program is to minimize loss. The loss equation would be
$\(\mu_{current} - \mu_{target}\)^2 + \(\sigma_{current} - \sigma_{target}\)^2$

The program would then loop and apply small, random adjustments to the elements (excluding min and max). If a change
decreases loss, keep it; if it increases loss, discard it. The loop would end when loss is near 0 (some small value like 0.00001).

## Reworked solution

A better solution to this would be applying a linear rescaling transformation. This would occur every iteration until it is
near 0 (some small value like 0.000001).

## Function overview

- `DataCalculator` - namespace for static calculation functions (average and standard deviation)
- `init_values` - initializes the list by inserting the min, max, and random values in between
- `tweak_values` - modifies the list by applying linear rescaling transformation
