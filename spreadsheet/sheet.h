#pragma once

#include "cell.h"
#include "common.h"


#include <functional>

class Sheet : public SheetInterface {
    using CellPtr = std::unique_ptr<Cell>;    
    using Value = std::variant<std::string, double, FormulaError>;
public:
    ~Sheet() = default;
    
    void SetCell(Position pos, std::string text) override;
    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    void ClearCell(Position pos) override;
    Size GetPrintableSize() const override;
    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;
    
    const Cell* GetConcreteCell(Position pos) const;
    Cell* GetConcreteCell(Position pos);
    
private:	    
    std::vector<std::vector<CellPtr>> sheet_;  
    std::vector<int> row_to_num_of_cells_;
    std::vector<int> column_to_num_of_cells_;
    Size print_area_{-1, -1};
    
    std::ostream& PrintValue (std::ostream &os, const Value& value) const;
};

