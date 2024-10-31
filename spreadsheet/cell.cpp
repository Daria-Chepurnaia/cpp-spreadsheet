#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <deque>

void Cell::Set(Position position, std::string text) {
    position_ = position;
    
    if (text.size() == 0) {
        impl_ = std::make_unique<EmptyImpl>();

    } else if (text.size() == 1) {
        impl_ = std::make_unique<TextImpl>();
        impl_->Set(position, text);

    } else if (text[0] == FORMULA_SIGN) {        
        auto formula = ParseFormula(text.substr(1));
        ThrowIfIncorrectFormula (formula);   
        impl_ = std::make_unique<FormulaImpl>(sheet_, formula.release());
        impl_->Set(position, text.substr(1)); 

    } else {        
        impl_ = std::make_unique<TextImpl>();
        impl_->Set(position, text);
    }
    UpdateDependencies(position);
    InvalidateCache();   
}

std::vector<Position> Cell::GetReferencedCells() const {      
    return {referenced_cells_.begin(), referenced_cells_.end()};
} 

void Cell::Clear() {  
    impl_->Set(position_, "");
    InvalidateCache();
    referenced_cells_.clear();
}

void Cell::InvalidateCache() {
    cached_value_.reset();
    std::deque<Position> cells_to_invalidate = {cells_dependent_on_this_cell_.begin(),
                                                cells_dependent_on_this_cell_.end()};
    std::set<Position> invalidated;
    
    while(cells_to_invalidate.size() != 0) {
        Position current = cells_to_invalidate.back();
        cells_to_invalidate.pop_back();
        cell_provider_(current)->cached_value_.reset();        
        for (Position pos : cell_provider_(current)->cells_dependent_on_this_cell_) {
            if (invalidated.count(pos) == 0) {
                cells_to_invalidate.push_front(pos);
            }
        }
        invalidated.insert(current);
    }    
}

Cell::Value Cell::GetValue() const { 
    if (cached_value_) {
        return cached_value_.value();
    } else {
        return impl_->GetValue();
    }    
}

std::string Cell::GetText() const {
    return impl_->GetText();
    
}
      
void Cell::Impl::Set(Position pos, std::string text) {
    text_ = text;
}

bool Cell::HasCircDependenciesFromCell(Position p) const {
    if (p == position_) return true;
    for (auto position : cells_dependent_on_this_cell_) {
        if (p == position ||
            cell_provider_(position)->HasCircDependenciesFromCell(p)) {
            return true;
        } 
    }
    return false;
}

bool Cell::HasCircDependencies(std::vector<Position> cells) const {    
    for (auto pos : cells) {
        if (pos == position_ || HasCircDependenciesFromCell(pos)) return true;
    }
    return false;
}

void Cell::ThrowIfIncorrectFormula (std::unique_ptr<FormulaInterface>& formula) const {
    //If there are dependencies on other cells and they are cyclic, throw an exception
    if (formula->GetReferencedCells().size() != 0) {        
        if (HasCircDependencies(formula->GetReferencedCells())) {
            throw CircularDependencyException("incorrect formula. Causes circular dependencies");        
        }
        //check the validity of the positions in the formula
        for (auto position : formula->GetReferencedCells()) {
            if (!position.IsValid()) throw FormulaException("incorrect formula");
        }
    }
}

void Cell::UpdateDependencies(Position position) {
    for (auto pos : referenced_cells_) {
        cell_provider_(pos)->cells_dependent_on_this_cell_.erase(position);
    }
   
    auto referenced_cells = impl_->GetReferencedCells();
    referenced_cells_ = {referenced_cells.begin(), referenced_cells.end()};
    
    for (auto pos : referenced_cells_) {
        if (cell_provider_(pos) == nullptr) {
            sheet_->SetCell(pos, "");
        }
        cell_provider_(pos)->cells_dependent_on_this_cell_.insert(position);
    }
}

CellInterface::Value Cell::Impl::GetValue() const {
    return text_;
}

std::string Cell::Impl::GetText() const {
    return text_;
}  

CellInterface::Value Cell::TextImpl::GetValue() const {            
    if (text_[0] == ESCAPE_SIGN) {
        return text_.substr(1);
    } else {
        return text_;
    }
}     

void Cell::FormulaImpl::Set(Position pos, std::string text) {
    formula_ = ParseFormula(text);     
    text_ = FORMULA_SIGN + formula_->GetExpression();
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() {
    return formula_->GetReferencedCells();
}

CellInterface::Value Cell::FormulaImpl::GetValue() const {
    auto result = formula_->Evaluate(*table_);
    if (std::holds_alternative<double>(result)) {
        return std::get<double>(result);
    } else {
        return std::get<FormulaError>(result);
    }            
}
  