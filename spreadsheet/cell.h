#pragma once

#include "common.h"
#include "formula.h"

#include <functional>
#include <set>
#include <optional>
#include <memory>

class Cell : public CellInterface {
public:
    Cell(SheetInterface* sheet, std::function<Cell*(Position)> cell_provider) : sheet_(sheet), cell_provider_(cell_provider) {}
    ~Cell() = default;

    void Set(Position pos, std::string text);    
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;   
    std::vector<Position> GetReferencedCells() const override;  
    bool HasCircDependenciesFromCell(Position p) const;
    bool HasCircDependencies(std::vector<Position> cells) const;    
    void InvalidateCache();
    void ThrowIfIncorrectFormula (std::unique_ptr<FormulaInterface>& formula) const;    
    
private:
    void UpdateDependencies(Position position);
    class Impl {
    public:
        Impl() = default;
        virtual ~Impl() = default;
        
        virtual void Set(Position pos, std::string text);
        virtual Value GetValue() const;
        virtual std::string GetText() const;
        virtual std::vector<Position> GetReferencedCells() { return {}; }
        
    protected:
        std::string text_ = ""; 
    };
    
    class EmptyImpl : public Impl {    
    };
    
    class TextImpl : public Impl {
    public:        
        Value GetValue() const override;           
    };
    
    class FormulaImpl : public Impl {
    public:
        FormulaImpl(SheetInterface* sheet, FormulaInterface* formula) : table_(sheet), formula_(formula) {}
        void Set(Position pos, std::string text) override;
        Value GetValue() const override; 
        std::vector<Position> GetReferencedCells() override;
    private:
        SheetInterface* table_;
        std::unique_ptr<FormulaInterface> formula_;        
    };
    
    std::unique_ptr<Impl> impl_;
    Position position_ {-1, -1};
    std::set<Position> cells_dependent_on_this_cell_;
    std::set<Position> referenced_cells_;
    mutable std::optional<double> cached_value_;
    SheetInterface* sheet_;
    std::function<Cell*(Position)> cell_provider_;
};
