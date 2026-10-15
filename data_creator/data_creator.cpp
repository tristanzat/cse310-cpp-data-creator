#include <iostream>
#include <random>
#include <list>
#include <iomanip>

using std::list;

std::random_device rd;
std::mt19937 gen(rd());


// Constants for looping
double ERROR_LIMIT = 0.000001;
int MAX_ITER = 1000;
int SAME_DIFF_ITER_MAX = 10;

static list<double> init_values(const double& min, const double& max,
    const int& count);
static list<double> tweak_values(const list<double>& init_list,
    const double& curr_mean, const double& target_mean,
    const double& curr_std_dev, const double& target_std_dev);
static void print_list(const list<double>& list);

namespace DataCalculator {
    /// <summary>
    /// Calculates the average of a list
    /// </summary>
    /// <param name="data">List of doubles to get average of</param>
    /// <returns>The average of the values in the list</returns>
    static double calculate_average(const list<double>& data)
    {
        // sum of all values / count

        double sum = 0.0;

        for (double x : data)
        {
            sum += x;
        }

        return sum / data.size();
    }

    /// <summary>
    /// Calculates the standard deviation of a list
    /// </summary>
    /// <param name="data">List of doubles to get standard deviation of</param>
    /// <returns>The standard deviation of that list</returns>
    static double calculate_std_dev(const list<double>& data)
    {
        // std dev = sqrt((sum from i=1 to N(x_i - avg)^2)/N)

        double sum = 0.0;
        double avg = calculate_average(data);

        for (double x : data)
        {
            sum += std::pow((x - avg), 2);
        }

        return std::sqrt(sum / data.size());
    }
}

int main()
{
    list<double> generated_data;
    double target_mean = 80.0;
    double target_std_dev = 15;
    double min = 10.0;
    double max = 100.0;
    int count = 10;

    // Initialize list with values
    generated_data = init_values(min, max, count);

    // Display initial values
    print_list(generated_data);

    // Loop until standard deviation and mean are near target, we run into max iterations,
    // or we hit max iterations with same mean and std dev (impossible targets)
    double curr_mean = 1.0;
    double curr_std_dev = 1.0;
    int iter = 0;
    int same_count = 0;
    bool impossible_targets = false;
    while ((std::abs(curr_mean - target_mean) > ERROR_LIMIT || std::abs(curr_std_dev - target_std_dev) > ERROR_LIMIT)
        && iter < MAX_ITER && !impossible_targets)
    {
        // Get current mean and standard deviation
        double new_mean = DataCalculator::calculate_average(generated_data);
        double new_std_dev = DataCalculator::calculate_std_dev(generated_data);

        // Compare to previous mean and standard deviation to see if we have set impossible targets
        if (std::abs(new_mean - curr_mean) < ERROR_LIMIT * 0.1 && std::abs(new_std_dev - curr_std_dev) < ERROR_LIMIT * 0.1)
        {
            same_count++;
            if (same_count > SAME_DIFF_ITER_MAX)
            {
                impossible_targets = true;
            }
        }
        else
        {
            same_count = 0;
        }

        curr_mean = new_mean;
        curr_std_dev = new_std_dev;
        
        // Tweak values
        generated_data = tweak_values(generated_data, curr_mean, target_mean, curr_std_dev, target_std_dev);

        // Display info and increment iteration counter
        std::cout << "Loop " << std::setw(5) << ++iter
            << " | Current mean: " << std::fixed << std::setprecision(6) << std::setw(10) << curr_mean
            << " | Target mean" << std::fixed << std::setprecision(6) << std::setw(10) << target_mean
            << " | Current std dev" << std::fixed << std::setprecision(6) << std::setw(10) << curr_std_dev
            << " | Target std dev" << std::fixed << std::setprecision(6) << std::setw(10) << target_std_dev
            << std::endl;
    }

    // Display message if user set impossible targets
    if (impossible_targets)
    {
        std::cout << "Targets determined to be impossible to reach. Showing closest data:" << std::endl;
    }

    // Display final values
    print_list(generated_data);

    return 0;
}

/// <summary>
/// Generates a random list of numbers given these parameters.
/// </summary>
/// <param name="min">Minimum value of dataset</param>
/// <param name="max">Maximum value of dataset</param>
/// <param name="count">Number of items in dataset</param>
/// <returns>A list with `count` items between `min` and `max`, inclusive</returns>
static list<double> init_values(const double& min, const double& max, const int& count)
{
    // Initialize return list
    list<double> return_list;

    // Put in min and max
    return_list.push_back(min);
    return_list.push_back(max);

    // Set up random number generation
    std::uniform_real_distribution<double> distr(min, max);

    // Fill with random values
    for (int i = 0; i < count - 2; i++)
    {
        return_list.push_back(distr(gen));
    }

    // Return the list
    return return_list;

}

/// <summary>
/// Modifies values of a given list by applying a linear rescaling transformation.
/// </summary>
/// <param name="init_list">List to transform</param>
/// <param name="curr_mean">Current mean</param>
/// <param name="target_mean">Target mean</param>
/// <param name="curr_std_dev">Current standard deviation</param>
/// <param name="target_std_dev">Target standard deviation</param>
/// <returns>The transformed list</returns>
static list<double> tweak_values(const list<double>& init_list, const double& curr_mean,
    const double& target_mean, const double& curr_std_dev, const double& target_std_dev)
{
    list<double> return_list = init_list;
    const double min = *return_list.begin();
    const double max = *std::next(return_list.begin(), 1);

    // new value = target_mean + (old value - current_mean) * (target_std_dev / curr_std_dev)
    auto list_start = std::next(return_list.begin(), 2);
    std::for_each(list_start, return_list.end(), [&](auto& num) {
        // Perform transformation
        num = target_mean + (num - curr_mean) * (target_std_dev / curr_std_dev);
        // Clamp if necessary
        num = (num < min) ? min : num;
        num = (num > max) ? max : num;
        });

    return return_list;
}

static void print_list(const list<double>& list)
{
    int i = 0;
    int count = list.size();
    std::cout << "[";

    for (const auto& item : list)
    {
        std::cout << std::fixed << std::setprecision(2) << std::setw(4) << item;
        i++;
        // Add comma if it's not the last item in the list
        if (i < count)
        {
            std::cout << ", ";
        }
    }

    std::cout << "]" << std::endl;
}
