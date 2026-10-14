#include <iostream>
#include <random>
#include <list>
#include <iomanip>

using std::list;

std::random_device rd;
std::mt19937 gen(rd());


// Constants for looping
double LOSS_LIMIT = 0.000001;
double MAX_ITER = 500;

// Function prototypes
list<double> init_values(const double& min, const double& max,
    const int& count);
list<double> tweak_values(const list<double>& init_list);
double calculate_loss(const double& curr_mean, const double& target_mean,
    const double& curr_std_dev, const double& target_std_dev);

namespace DataCalculator {
    static double calculate_average(const list<double>& data);
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
}

int main()
{
    list<double> generated_data;
    double target_mean = 80;
    double target_std_dev = 8.5;
    double min = 68;
    double max = 92;
    int count = 8;

    // Initialize list with values
    generated_data = init_values(min, max, count);

    // Loop until loss goes down or we run into max iterations
    double loss = 1.0;
    int iter = 0;
    while (loss > LOSS_LIMIT && iter < MAX_ITER)
    {
        // Calculate current loss
        loss = calculate_loss(DataCalculator::calculate_average(generated_data),
            target_mean, DataCalculator::calculate_std_dev(generated_data), target_std_dev);
        std::cout << "Loop " << std::setw(5) << iter + 1
            << " | Initial loss: " << std::fixed << std::setprecision(6) << std::setw(10) << loss;
        
        // Need to tweak values then calculate loss
        list<double> new_data = tweak_values(generated_data);

        // Calculate final loss
        double new_loss = calculate_loss(DataCalculator::calculate_average(new_data),
            target_mean, DataCalculator::calculate_std_dev(new_data), target_std_dev);

        // Display final loss of this loop
        std::cout << " | Final loss: " << std::fixed << std::setprecision(6) << std::setw(10) << new_loss << std::endl;

        // "Save" list changes if it decreased loss
        if (new_loss < loss)
        {
            generated_data = new_data;
        }

        iter++;
    }

    // Display final values
    std::cout << "[";
    for (int x : generated_data)
    {
        std::cout << x << ", ";
    }
    std::cout << "]" << std::endl;

    return 0;
}

/// <summary>
/// Generates a random list of numbers given these parameters.
/// </summary>
/// <param name="min">Minimum value of dataset</param>
/// <param name="max">Maximum value of dataset</param>
/// <param name="count">Number of items in dataset</param>
list<double> init_values(const double& min, const double& max, const int& count)
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
/// Modifies values of a given list and returns a new list with those tweaked values.
/// </summary>
/// <param name="init_list">The list to copy and modify</param>
/// <returns>The passed-in list with modified values</returns>
list<double> tweak_values(const list<double>& init_list)
{
    list<double> return_list = init_list;

    // Set up random number generation
    std::uniform_real_distribution<double> distr(*return_list.begin(),
        *std::next(return_list.begin(), 1));

    // Loop through the list starting at begin + 2 so we don't modify min or max
    auto list_start = std::next(return_list.begin(), 2);
    std::for_each(list_start, return_list.end(), [&distr](auto& num) {
        num = distr(gen);
        });

    return return_list;
}

double calculate_loss(const double& curr_mean, const double& target_mean,
    const double& curr_std_dev, const double& target_std_dev)
{
    return std::pow((curr_mean - target_mean), 2) + std::pow((curr_std_dev - target_std_dev), 2);
}
