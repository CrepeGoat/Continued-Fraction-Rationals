# Continued-Fraction-Rationals

## Description
This is a small C++ library that provides a space-efficient encoding of continued-fraction integer sequences.

## Anatomy
There are a few sections to this project.
- The encoding's **space-efficiency** is compared to theoretical limits and other previous works in the *Proofs & Demo.ipynb* file.
- A **formal mathematical paper**, hosted on OverLeaf.com, is linked in *LaTeX_report.url*.
- An **implementation** of the encoding in C++ is located in the *Implementation* folder.
- Other relevant works are listed in the *Reading* folder.

## Implementation Installation
All C++-implemented code is stored in the *Implementation* folder. The C++ implementation itself has no dependencies.

This module uses [CMake](https://cmake.org/) to generate makefiles for compilation & linkage. The *CMakeLists.txt* in the *Implementation* folder acts as the project head file. This may be adjusted to incorporate into another CMake-organized project.

## Usage
The full functionality is implemented in the class `WholeNumSequence`, which is declared in the header *WholeNumSequence.hpp*.

## License
This project is made available under the MIT License.
