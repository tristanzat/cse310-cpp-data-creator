#include <iostream>
#include <random>
#include <vector>
#include <tuple>
#include <iomanip>
#include <string>
#include <fstream>
#include <generator>
#include <sstream>

using std::vector;

std::random_device rd;
std::mt19937 gen(rd());


// Constants
double ERROR_LIMIT = 0.000001;
int MAX_ITER = 1000;
int SAME_DIFF_ITER_MAX = 10;

std::tuple<vector<double>, bool> modification_loop(vector<double>& data, const double& target_mean,
    const double& target_std_dev, const bool& do_print);
static bool valid_data(vector<double>& data, const double& min, const double& max,
    const double& target_mean, const double& target_std_dev);
static void display_loop_info(const int& iter, const double& curr_mean, const double& target_mean,
    const double& curr_std_dev, const double& target_std_dev);
static vector<double> init_values(const double& min, const double& max, const int& count);
static vector<double> tweak_values(const vector<double>& init_list, const double& curr_mean,
    const double& target_mean, const double& curr_std_dev, const double& target_std_dev);
static void print_list(const vector<double>& list);
static std::generator<vector<double>> get_data_from_file(const std::string& filename);
static void write_data_to_file(const std::string& filename, const vector<double>& test_case,
    const vector<double>& data, const bool& impossible);

namespace DataCalculator {
    /// <summary>
    /// Calculates the average of a list
    /// </summary>
    /// <param name="data">List of doubles to get average of</param>
    /// <returns>The average of the values in the list</returns>
    static double calculate_average(const vector<double>& data)
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
    static double calculate_std_dev(const vector<double>& data)
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

int main(int argc, char* argv[])
{
    // Set up variables for later
    double min;
    double max;
    double target_mean;
    double target_std_dev;
    int count;
    vector<double> data;
    bool impossible;
    // Parse arguments from the command line for filename so no user input is required inside the program
    // Have this base case in case no file is passed in
    if (argc < 2)
    {
        min = 50.0;
        max = 100.0;
        target_mean = 80.0;
        target_std_dev = 15.0;
        count = 20;

        // Run the program as normal
        data = init_values(min, max, count);

        print_list(data);

        if (valid_data(data, min, max, target_mean, target_std_dev))
        {
            const auto [new_data, is_impossible] = modification_loop(data, target_mean, target_std_dev, true);

            data = new_data;
            impossible = is_impossible;
        }
        else
        {
            impossible = true;
        }

        // Display message if user set impossible targets
        if (impossible)
        {
            std::cout << "Targets determined to be impossible to reach. Showing closest data:" << std::endl;
        }

        print_list(data);
    }
    // File was provided
    else
    {
        bool output_to_file = false;
        bool verbose = false;
        bool extra_verbose = false;
        int verbose_index = 2;
        // Pull args into strings
        std::string in_file = argv[1];
        std::string out_file;
        // output file
        try
        {
            if (argc > 2)
            {
                // If this goes through, then we have a valid file to write to
                out_file = argv[2];
                if (out_file.length() > 4 && out_file.ends_with(".txt"))
                {
                    output_to_file = true;
                    verbose_index++;
                }
            }
        }
        // Move on if output file parsing fails
        catch (...) {}

        // verbose and extra-verbose options
        try
        {
            if (argc >= verbose_index + 1)
            {
                std::string verbosity = argv[verbose_index];
                if (verbosity == "verbose")
                {
                    verbose = true;
                }
                if (verbosity == "extra-verbose")
                {
                    verbose = true;
                    extra_verbose = true;
                }
            }
        }
        // Move on if verbosity parsing fails
        catch (...) {}

        for (vector<double> args : get_data_from_file(in_file))
        {
            // Each returned vector is expected to be the format
            // min, max, target_mean, target_std_dev, count
            min = args[0];
            max = args[1];
            target_mean = args[2];
            target_std_dev = args[3];
            count = args[4];

            // Run the program using this data
            data = init_values(min, max, count);

            if (verbose)
            {
                std::cout << "Min: " << min << " | Max: " << max << " | Target mean: " << target_mean
                    << " | Target std dev: " << target_std_dev << " | Number of items: " << count << std::endl;
                print_list(data);
                std::cout << std::endl;
            }

            if (valid_data(data, min, max, target_mean, target_std_dev))
            {
                const auto [new_data, is_impossible] = modification_loop(data, target_mean, target_std_dev, extra_verbose);

                data = new_data;
                impossible = is_impossible;
            }
            else
            {
                impossible = true;
            }

            // Display message if user set impossible targets
            if (impossible && verbose)
            {
                std::cout << "Targets determined to be impossible to reach. Showing closest data:" << std::endl;
            }

            if (verbose)
            {
                print_list(data);
                std::cout << std::endl << std::endl;
            }

            if (output_to_file)
            {
                write_data_to_file(out_file, args, data, impossible);
            }
        }

    }

    return 0;
}

std::tuple<vector<double>, bool> modification_loop(vector<double>& data, const double& target_mean,
    const double& target_std_dev, const bool& do_print)
{
    // Set up variables used in loop
    double curr_mean = DataCalculator::calculate_average(data);
    double curr_std_dev = DataCalculator::calculate_std_dev(data);
    int iter = 0;
    int same_count = 0;
    // Sanity check to see if we even need to enter the loop.
    // If the data is 1 or 2 elements and the calculated average or standard deviation
    // are off the targets, we already know targets are impossible to hit.
    bool impossible_targets = (data.size() <= 2
        && (std::abs(curr_mean - target_mean) > ERROR_LIMIT
        || std::abs(curr_std_dev - target_std_dev) > ERROR_LIMIT));

    // Loop until standard deviation and mean are near target, we run into max iterations,
    // or we hit max iterations with same mean and std dev (impossible targets).
    while ((std::abs(curr_mean - target_mean) > ERROR_LIMIT
        || std::abs(curr_std_dev - target_std_dev) > ERROR_LIMIT)
        && iter < MAX_ITER && !impossible_targets)
    {
        // Increment iteration counter
        iter++;
        // Display info if we want it to
        if (do_print)
        {
            display_loop_info(iter, curr_mean, target_mean, curr_std_dev, target_std_dev);
        }

        // Get current mean and standard deviation
        double new_mean = DataCalculator::calculate_average(data);
        double new_std_dev = DataCalculator::calculate_std_dev(data);

        // Compare to previous mean and standard deviation to see if we have set impossible targets.
        // This is done so there is not an unnecessary amount of looping when the data won't change.
        // The error limit is made an order of magnitude smaller to make sure this doesn't trip before
        // the limit comparison for the targets; the loop won't end early for no reason.
        // This is a guaranteed trigger on the first pass because modification hasn't happened yet so
        // there is no difference.
        if (std::abs(new_mean - curr_mean) < ERROR_LIMIT * 0.1
            && std::abs(new_std_dev - curr_std_dev) < ERROR_LIMIT * 0.1)
        {
            same_count++;
            if (same_count > SAME_DIFF_ITER_MAX)
            {
                impossible_targets = true;
            }
        }
        // Reset the counter when we get a difference in case the data can be pulled closer.
        else
        {
            same_count = 0;
        }

        curr_mean = new_mean;
        curr_std_dev = new_std_dev;
        
        // Tweak values
        data = tweak_values(data, curr_mean, target_mean, curr_std_dev, target_std_dev);
    }

    // Assume targets were impossible if we hit max iterations
    if (iter >= MAX_ITER)
    {
        impossible_targets = true;
    }

    return { data, impossible_targets };
}

static bool valid_data(vector<double>& data, const double& min, const double& max,
    const double& target_mean, const double& target_std_dev)
{
    // This function handles lots of logical checks to see if we need to modify the target data
    // or if it's fundamentally impossible.

    // Mean needs to be between min and max
    if (min > target_mean || target_mean > max)
        return false;
    
    // Standard deviation is not negative
    if (target_std_dev < 0)
        return false;

    // If min = max, std dev needs to be 0
    if (min == max)
    {
        if (target_std_dev != 0)
            return false;
    }

    // Passed these checks
    return true;

}

static void display_loop_info(const int& iter, const double& curr_mean,const double& target_mean,
    const double& curr_std_dev,const double& target_std_dev)
{
    // This function exists so that functions are more cohesive; modification_loop doesn't directly print data anymore
    std::cout << "Loop " << std::setw(5) << iter
        << " | Current mean: " << std::fixed << std::setprecision(6) << std::setw(10) << curr_mean
        << " | Target mean" << std::fixed << std::setprecision(6) << std::setw(10) << target_mean
        << " | Current std dev" << std::fixed << std::setprecision(6) << std::setw(10) << curr_std_dev
        << " | Target std dev" << std::fixed << std::setprecision(6) << std::setw(10) << target_std_dev
        << std::endl;
}

/// <summary>
/// Generates a random list of numbers given these parameters.
/// </summary>
/// <param name="min">Minimum value of dataset</param>
/// <param name="max">Maximum value of dataset</param>
/// <param name="count">Number of items in dataset</param>
/// <returns>A list with `count` items between `min` and `max`, inclusive</returns>
static vector<double> init_values(const double& min, const double& max, const int& count)
{
    // Display errors if passed-in data won't work
    if (min > max)
    {
        std::cerr << "Minimum is less than maximum.";
    }
    if (count <= 0)
    {
        std::cerr << "Count must be greater than zero.";
    }
    if (count == 1 && min != max)
    {
        std::cerr << "List has one element but min and max are not the same.";
    }

    // Initialize return list
    vector<double> return_list;

    // Put in min and max
    return_list.push_back(min);
    if (count > 1)
    {
        return_list.push_back(max);
    }

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
static vector<double> tweak_values(const vector<double>& init_list, const double& curr_mean,
    const double& target_mean, const double& curr_std_dev, const double& target_std_dev)
{
    // This function applies a linear rescaling transformation to the data set. This is an equation
    // that nudges every element closer to the target mean and standard deviation.
    vector<double> return_list = init_list;
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

static void print_list(const vector<double>& list)
{
    // This function exists to pretty-print a list
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

static std::generator<vector<double>> get_data_from_file(const std::string& filename)
{
    // Similar to yield return in C#. I want this so that my file can have multiple sets
    // of parameters for main to pass in. Expects values separated by spaces in format
    // min max target_mean target_std_dev count

    std::ifstream file(filename);

    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file " + filename);
    }

    std::string line;
    vector<double> args;
    
    // Go through each line
    while (std::getline(file, line))
    {
        // Remove any previous data
        args.clear();        

        // Set up string stream
        std::stringstream ss(line);
        double value;

        // Try parsing into double
        try
        {
            while (ss >> value)
            {
                args.push_back(value);
            }
        }
        catch (...)
        {
            std::cerr << "Error parsing into double.";
            continue;
        }

        co_yield args;
    }

    file.close();
}

static void write_data_to_file(const std::string& filename, const vector<double>& test_case,
    const vector<double>& data, const bool& impossible)
{
    // This function writes the test case and the generated data to a file. If the case was impossible, it writes
    // "Impossible to generate valid data."

    std::ofstream file(filename, std::ios::app);

    if (!file.is_open())
    {
        throw std::runtime_error("Error opening file " + filename);
    }

    // Write test case
    file << "Min: " << test_case[0] << " | Max: " << test_case[1] << " | Target mean: " << test_case[2]
        << " | Target std dev: " << test_case[3] << " | Number of items: " << test_case[4] << std::endl;

    // Write data or impossible
    if (!impossible)
    {
        // Borrow code from print_list
        int i = 0;
        int count = data.size();
        file << "[";

        for (const auto& item : data)
        {
            file << item;
            i++;
            // Add comma if it's not the last item in the list
            if (i < count)
            {
                file << ", ";
            }
        }

        file << "]" << std::endl;
    }
    else
    {
        file << "Impossible to generate valid data." << std::endl;
    }

    file << std::endl;

    file.close();
}
