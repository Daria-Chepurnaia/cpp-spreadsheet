#pragma once

#include "common.h"

#include <memory>
#include <vector>

// Formula that allows for calculating and updating an arithmetic expression.
// Supported features:
// * Simple binary operations and numbers, parentheses: 1+2*3, 2.5*(2+3.5/7)
// * Cell values as variables: A1+B2*C3
// Cells specified in the formula can be either formulas or text. If it's 
// text that represents a number, it is treated as a number. An empty 
// cell or a cell with empty text is treated as the number zero.
class FormulaInterface {
public:
    using Value = std::variant<double, FormulaError>;

    virtual ~FormulaInterface() = default;

    // Returns the computed value of the formula for the given sheet or an error.
    // If the computation of any of the cells specified in the formula results in 
    // an error, that error is returned. If there are multiple such errors, 
    // any one of them is returned.
    virtual Value Evaluate(const SheetInterface& sheet) const = 0;

    // Returns the expression representing the formula.
    // It does not contain spaces or unnecessary parentheses.
    virtual std::string GetExpression() const = 0;

    // Returns a list of cells that are directly involved in the calculation
    // of the formula. The list is sorted in ascending order and does not
    // contain duplicate cells.
    virtual std::vector<Position> GetReferencedCells() const = 0;
};

// Parses the given expression and returns a formula object.
// Throws FormulaException if the formula is syntactically incorrect.
std::unique_ptr<FormulaInterface> ParseFormula(std::string expression);
