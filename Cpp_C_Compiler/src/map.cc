#include <iostream>
#include <unordered_map>
#include <string>
#include "map.hh"

// constructor
Symbol :: Symbol(int t, int k, int i, const std::string& n)
     : type(t), kind(k), id(i), name(n) {}

 int Symbol::getType() const { return type; }
 int Symbol::getKind() const { return kind; }
 int Symbol::getId() const { return id; }
 std::string Symbol::getName() const { return name; }

//print
void Symbol::print() const {
    std::cout << "  name: " << name
              << ", type: " << type
              << ", kind: " << kind
              << ", ID: " << id << "\n";
}

std::map<int, SymbolMap> regions_map;

// add a symbol
bool add_symbol(int region_id, const Symbol& sym) {
    auto& sym_map = regions_map[region_id];
    if (sym_map.find(sym.getName()) != sym_map.end()) {
        std::cerr << "error the symbol '" << sym.getName()
                  << "' has already been declared in the region " << region_id << "\n";
                  exit(1);
        return true;
    }
    
    if (sym.getType()==0 && region_id!=0)
    {
    sym_map[sym.getName()] = std::make_shared<Symbol>(sym);
    }
    
    if (region_id==0 && sym.getKind()==1)
    {
    sym_map[sym.getName()] = std::make_shared<Symbol>(sym);
    }
    return true;
}

// search in a certain region
Symbol* lookup_symbol(int region_id, const std::string& name) {
    auto it = regions_map.find(region_id);
    if (it != regions_map.end()) {
        auto sym_it = it->second.find(name);
        if (sym_it != it->second.end()) {
            return sym_it->second.get();
        }
    }
    return nullptr;
}

// search in a scope (0 = global)
Symbol* lookup_in_scope(int region_id, const std::string& name) {
    for (int i = region_id; i >= 0; --i) {
        Symbol* sym = lookup_symbol(i, name);
        if (sym) return sym;
    }
    return nullptr;
}

// print the symbols of a region
void print_region(int region_id) {
    std::cout << "symbols in the region " << region_id << ":\n";
    for (const auto& [name, sym_ptr] : regions_map[region_id]) {
        sym_ptr->print();
    }
}
