#pragma once

#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

struct Position {
    int row = 0;
    int col = 0;

    bool operator==(Position rhs) const;
    bool operator<(Position rhs) const;

    bool IsValid() const;
    std::string ToString() const;
    void ThrowIfInvalid() const;
    static Position FromString(std::string_view str);

    static const int MAX_ROWS = 16384;
    static const int MAX_COLS = 16384;
    static const Position NONE;
};

struct Size {
    int rows = 0;
    int cols = 0;

    bool operator==(Size rhs) const;
};

// Describes errors that may occur when calculating the formula.
class FormulaError {
public:
    enum class Category {
        Ref,    // reference to a cell with an incorrect position
        Value,  // The cell cannot be interpreted as the number
        Arithmetic,  // division by zero 
    };

    FormulaError(Category category) : category_(category) {}

    Category GetCategory() const;

    bool operator==(FormulaError rhs) const {
        return category_ == rhs.category_;
    }

    std::string_view ToString() const;

private:
    Category category_;
};

std::ostream& operator<<(std::ostream& output, FormulaError fe);

// An exception is thrown if an incorrect position is passed
class InvalidPositionException : public std::out_of_range {
public:
    using std::out_of_range::out_of_range;
};

// An exception is thrown the formula is incorrect
class FormulaException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// An exception thrown when trying to set a formula that leads to a cyclic dependency between cells
class CircularDependencyException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class CellInterface {
public:
    // either the text of the cell, or the value of the formula, or the message about the error from
    using Value = std::variant<std::string, double, FormulaError>;

    virtual ~CellInterface() = default;

    // Returns the visible value of the cell.
    // For a text cell, this is its text (without escape characters). 
    // In the case of a formula - the numerical value of the formula or an error message.
    virtual Value GetValue() const = 0;

    // Returns the internal text of the cell, as if we were
    // editing it. For a text cell, this is its text (possibly
    // containing escape characters). In the case of a formula - its expression.
    virtual std::string GetText() const = 0;

    // Returns a list of cells that are directly involved in this
    // formula. The list is sorted in ascending order and does not
    // contain duplicate cells. In the case of a text cell, the list is empty.
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

inline constexpr char FORMULA_SIGN = '=';
inline constexpr char ESCAPE_SIGN = '\'';

// Интерфейс таблицы
class SheetInterface {
public:
    virtual ~SheetInterface() = default;

    // Sets the content of the cell. If the text starts with "=", it 
    // is interpreted as a formula. If an invalid formula is set, 
    // a FormulaException is thrown and the cell's value remains 
    // unchanged. If a formula leads to a circular dependency (for 
    // example, if the formula references the current cell), a 
    // CircularDependencyException is thrown and the cell's value 
    // remains unchanged.
    // 
    // Clarifications regarding formula notation:
    // * If the text contains only the "=" symbol and nothing else, 
    //   it is not considered a formula.
    // * If the text starts with the "'" (apostrophe) symbol, it 
    //   will be omitted when outputting the cell's value using 
    //   the GetValue() method. This can be used to start the 
    //   text with "=", but prevent it from being interpreted 
    //   as a formula.
    virtual void SetCell(Position pos, std::string text) = 0;

    virtual const CellInterface* GetCell(Position pos) const = 0;
    virtual CellInterface* GetCell(Position pos) = 0;

    // Clears the cell. 
    // A subsequent call to GetCell() for this cell will return either 
    // nullptr or an object with empty text.
    virtual void ClearCell(Position pos) = 0;

    // Calculates the size of the area involved in printing. 
    // Defined as the bounding rectangle of all cells with non-empty text. 
    virtual Size GetPrintableSize() const = 0;

    // Outputs the entire table to the given stream. Columns are 
    // separated by a tab character. After each row, a newline 
    // character is printed. The methods GetValue() or GetText() 
    // are used to convert cells to strings. An empty cell is represented 
    // as an empty string
    virtual void PrintValues(std::ostream& output) const = 0;
    virtual void PrintTexts(std::ostream& output) const = 0;
};

// Creates a ready-to-use empty table.
std::unique_ptr<SheetInterface> CreateSheet();