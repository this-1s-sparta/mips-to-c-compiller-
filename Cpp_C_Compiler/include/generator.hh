#ifndef GENERATOR_HH
#define GENERATOR_HH

#include <iostream>
#include <unordered_map>
#include "ast.hh"

extern int current_region;
extern int region_counter;

extern std::unordered_map<int, bool> break_flags;
extern std::unordered_map<int, std::string> break_targets;

void generate_code(ASTNode* node);
void emit_code(const std::string& code);
std::string new_label(const std::string& base);

#endif // GENERATOR_HH
