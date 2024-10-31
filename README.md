# Project Name: Simplified Spreadsheet

## Description
This project is a core logic module for a spreadsheet application like Microsoft Excel or Google Sheets. The spreadsheet allows text and formula entries in its cells. Formulas can include cell references, and they are parsed and evaluated using ANTLR, a powerful parser generator.

## Features
- Text and formula entries in cells
- Formula parsing and evaluation using ANTLR
- Efficient memory usage for sparse tables
- Fast cell access by index
- Expandable structure for future functionality

## Installation
To get started with this project, you need to install ANTLR and set up your development environment.

### Prerequisites
- **JDK**: Install JDK or OpenJDK in your system.
- **ANTLR**: Download and set up ANTLR from [antlr.org](https://www.antlr.org).

### Steps
1. **Set up ANTLR**:
   - Follow the installation instructions on the ANTLR website.
   - Ensure the `antlr*.jar` file is included in the `CLASSPATH` environment variable.

2. **Generate ANTLR files**:
   ```bash
   antlr4 Formula.g4
   javac *.java

3. **Generate C++ files with ANTLR:**
   ```bash
   antlr4 -Dlanguage=Cpp Formula.g4

4. **Integrate ANTLR with CMake:**   
  
   - Place CMakeLists.txt and FindANTLR.cmake in your project directory.
   - Add the following line to your CMakeLists.txt:
    ```cmake
    include(${CMAKE_CURRENT_SOURCE_DIR}/FindANTLR.cmake)

5. **Build the project:**
   - Navigate to the build directory and run the CMake build commands. 
  
## Usage

`Sheet::SetCell(Position pos, std::string text)` - Set the content of a cell by specifying its position and value (text or formula).

`Sheet::GetCell(Position position)` - Retrieve the content of a cell by its position.

`Sheet::ClearCell(Position position)` - Clear the content of a cell by specifying its position.

`Sheet::PrintValues(std::ostream& output)` - Print the minimal printable area of the spreadsheet to the standard output.

## Future Enhancements
The project is designed with extensibility in mind. Future enhancements may include:

- Inserting and deleting rows/columns
- Handling cell references in formulas
- Improving error handling and reporting
  
## License
This project is licensed under the terms of the [Mit License](https://github.com/Daria-Chepurnaia/cpp-spreadsheet/blob/main/LICENSE.txt) 
