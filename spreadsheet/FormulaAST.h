#pragma once

#include "FormulaLexer.h"
#include "common.h"
#include "cell.h"


#include <forward_list>
#include <functional>
#include <stdexcept>
#include <functional>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr,
                        std::forward_list<Position> cells);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    double Execute(const SheetInterface* sheet) const;
    void PrintCells(std::ostream& out) const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

    std::vector<Position>& GetCells() {
        if (unique_sorted_cells_.empty()) {
            std::set<Position> result(cells_.begin(), cells_.end());        
            unique_sorted_cells_ = {result.begin(), result.end()};
        }
        
        return unique_sorted_cells_;
    }

    const std::vector<Position>& GetCells() const {
        if (unique_sorted_cells_.empty()) {
            std::set<Position> result(cells_.begin(), cells_.end());        
            unique_sorted_cells_ = {result.begin(), result.end()};
        }        
        return unique_sorted_cells_;
    }

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;

    // physically stores cells so that they can be
    // efficiently traversed without going through
    // the whole AST
    std::forward_list<Position> cells_;
    mutable std::vector<Position> unique_sorted_cells_;
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
