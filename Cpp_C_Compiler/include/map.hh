#ifndef MAP_H
#define MAP_H

#include <string>
#include <map>
#include <memory>

class Symbol {
private:
    int type;
    int kind;      
    int id;
    std::string name;

public:
    Symbol(int type, int kind, int id, const std::string& name);
    void print() const;
    //added
    int getType() const;
    int getKind() const;
    int getId() const;
    std::string getName() const;
    
};

using SymbolPtr = std::shared_ptr<Symbol>;
using SymbolMap = std::map<std::string, SymbolPtr>;

// map: each region has its own symbol table
extern std::map<int, SymbolMap> regions_map;

bool add_symbol(int region_id, const Symbol& sym);
Symbol* lookup_symbol(int region_id, const std::string& name);
Symbol* lookup_in_scope(int region_id, const std::string& name);
void print_region(int region_id);


#endif // MAP_H
