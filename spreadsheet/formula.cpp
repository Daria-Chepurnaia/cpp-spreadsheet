#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <functional>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#ARITHM!";
}

namespace {
class Formula : public FormulaInterface {
public:
     explicit Formula(std::string expression) try
        : ast_(ParseFormulaAST(expression)) {
    } catch (const FormulaException& exc) {
        throw exc;
    }
    
    Value Evaluate(const SheetInterface& sheet) const override {
        Value value;
        try {
           value = ast_.Execute(&sheet);
        } catch (const FormulaError& exc) {
           value = exc;
        }
        return value;
    }
    std::string GetExpression() const override  {
        std::ostringstream expression;
        ast_.PrintFormula(expression);
        return expression.str();
    }
    
    std::vector<Position> GetReferencedCells() const override {
        return ast_.GetCells();
    };

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try { 
        return std::make_unique<Formula>(std::move(expression));        
    }
    catch(...) {
        throw FormulaException("formula exception");
    }    
}