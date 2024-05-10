#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;
using namespace std;


void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    int row_number = sheet_.size();
    int col_number = row_number == 0 ? 0 : sheet_.at(0).size();
    
    if (pos.row >= row_number) {
        sheet_.resize(pos.row + 1); 
        for (int i = pos.row; i >= row_number; --i) {
            sheet_[i].resize(col_number);
        }
    }
    if (pos.col >= col_number) {
        for (auto& row : sheet_) {
            row.resize(pos.col + 1);
        }        
    }

    if (sheet_[pos.row][pos.col] == nullptr)  {
        auto l = [this](Position p) {return GetConcreteCell(p);};
        sheet_[pos.row][pos.col] = std::make_unique<Cell>(this, l);
    }   
    
    sheet_[pos.row][pos.col]->Set(pos, text);    

    if ((int)row_to_num_of_cells_.size() <= pos.row) {
        row_to_num_of_cells_.resize(pos.row + 1);
    }
    
    if ((int)column_to_num_of_cells_.size() <= pos.col) {
        column_to_num_of_cells_.resize(pos.col + 1);
    }    
    
    row_to_num_of_cells_[pos.row] += 1;
    column_to_num_of_cells_[pos.col] += 1;

    print_area_.rows = std::max(print_area_.rows, pos.row);
    print_area_.cols = std::max(print_area_.cols, pos.col);     
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    if (pos.row > print_area_.rows || pos.col > print_area_.cols) return nullptr;
    if (sheet_[pos.row][pos.col] == nullptr) {
        return nullptr;
    } else {
        return sheet_[pos.row][pos.col].get();
    }    
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    if (pos.row > print_area_.rows || pos.col > print_area_.cols) return nullptr;
    if (sheet_[pos.row][pos.col] == nullptr) {
        return nullptr;
    } else {
       return sheet_[pos.row][pos.col].get();
    }    
}

const Cell* Sheet::GetConcreteCell(Position pos) const {        
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    if (pos.row > print_area_.rows || pos.col > print_area_.cols) return nullptr;
    if (sheet_[pos.row][pos.col] == nullptr) {
        return nullptr;
    } else {
        return sheet_[pos.row][pos.col].get();
    }   
}
    
Cell* Sheet::GetConcreteCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    if (pos.row > print_area_.rows || pos.col > print_area_.cols) return nullptr;
    if (sheet_[pos.row][pos.col] == nullptr) {
        return nullptr;
    } else {
        return sheet_[pos.row][pos.col].get();
    }   
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) throw InvalidPositionException("invalid position");
    if (pos.row > print_area_.rows || pos.col > print_area_.cols) return;
    if (sheet_[pos.row][pos.col]) {
        sheet_[pos.row][pos.col].reset();            

        row_to_num_of_cells_[pos.row] -= 1;
        column_to_num_of_cells_[pos.col] -= 1;

        for (int i = pos.row; i >= 0; --i) {
            if (row_to_num_of_cells_[i] == 0) {
                --print_area_.rows;  
            } else {
                break;
            }
        }

        for (int i = pos.col; i >= 0; --i) {
            if (column_to_num_of_cells_[i] == 0) {
                --print_area_.cols;  
            } else {
                break;
            }
        }             
    }          
}

Size Sheet::GetPrintableSize() const {  return {print_area_.rows + 1, print_area_.cols + 1}; }

std::ostream& Sheet::PrintValue (std::ostream &os, const Value& value) const {
    if (std::holds_alternative<std::string>(value)) {
        return os << std::get<std::string>(value);
    } else if (std::holds_alternative<double>(value)) {
        return os << std::get<double>(value);
    } else {
        return os << "#ARITHM!";
    }    
}

void Sheet::PrintValues(std::ostream& output) const {
    if (print_area_ == Size{-1, -1}) {        
        return;
    }
    for (int i = 0; i <= print_area_.rows; ++i ) {
         bool is_first = true;
        for (int j = 0; j <= print_area_.cols; ++j ) {
           
            if (is_first) {
                if (sheet_[i][j] == nullptr) {
                    output << ""s;                        
                } else {                    
                    PrintValue(output, sheet_[i][j]->GetValue());
                }
                is_first = false;
            } else {
                output << '\t';
                if (sheet_[i][j] == nullptr) {
                    output << ""s;                        
                } else {
                    PrintValue(output, sheet_[i][j]->GetValue());   
                }
                
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    for (int i = 0; i <= print_area_.rows; ++i ) {
        bool is_first = true;
        for (int j = 0; j <= print_area_.cols; ++j ) {
            
            if (is_first) {
                if (sheet_[i][j] == nullptr) {
                    output << ""s;                        
                } else {
                    output << sheet_[i][j]->GetText();   
                }
                is_first = false;
            } else {
                output << '\t';
                if (sheet_[i][j] == nullptr) {
                    output << ""s;                        
                } else {
                    output <<sheet_[i][j]->GetText();  
                }
                
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}