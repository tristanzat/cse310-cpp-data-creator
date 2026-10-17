# Overview

This software uses C++ to generate a data set that meets certain requirements. More detailed info can be found
in the [design document](design_doc.md).

This demonstrates the processing power that C++ has when working with datasets and numbers. This also helped me
get a better understanding of C++ syntax.

[Demonstration Video](https://youtu.be/GP9WTECP_Q8)

# Running the code

The program takes in command line arguments when being run. With a terminal open in the directory with the compiled executable, run
`.\data_creator.exe [input_file_path] [output_file_path | verbosity] [verbosity]`

- If the second argument is not a valid file (does not end in .txt), the code assumes that it is verbosity.
- `verbosity` is either `verbose` (prints case, beginning list, and end list) or `extra-verbose` (prints mean and standard deviation each loop compared to target as well)

# Development Environment

I used the Visual Studio IDE (VS Community Edition 2026) with the `Desktop development with C++` workload to develop and test my code.

# Useful Websites

- [W3Schools](https://www.w3schools.com/cpp/)
- [cppreference.com](https://cppreference.com/)

# Future Work

- Update the algorithm to detect anomalies in the dataset
- Make the algorithm accept data in multiple formats
- Tweak algorithm to have dynamic scaling ratio depending on input parameters
 