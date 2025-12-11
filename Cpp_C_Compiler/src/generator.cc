#include "generator.hh"
#include "map.hh"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <set>
#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <map>
#include <vector>
std::ofstream out;

int region_counter = 0;
int old_region = 0;
SymbolMap current_symbols; //to handle the initializations when the file opens 

std::unordered_map<int, bool> break_flags;
std::unordered_map<int, std::string> break_targets; //to handle break

int return_count=0; //to handle return (inside function)
int call_count; //to handle where it returns to

using namespace std;
static std::set<std::string> generated_functions;
int label_counter = 0;

std::string new_label(const std::string& base) {
    return base + std::to_string(label_counter++);
}

void emit_code(const std::string& code) {
    static bool file_checked = false;
    static std::string last_label_line;
    if (!file_checked) {
        file_checked = true;
        std::remove("out.mixal");
        std::remove("out.mix");
        out.open("out.mixal");
        if (!out.is_open()) {
            std::cerr << "Error opening file out.mixal" << std::endl;
            return;
        }
        out << "        ORIG   1000\n";
        out << "TMP     CON     0\n";//tmp for storage
        out << "TEMP1   CON     0\n";//tmp1 for storage
        out << "ONE     CON     1\n";
        out << "ZERO    CON     0\n";

        for (const auto& [region_id, region_map] : regions_map) {
            if (region_id != 0) {
                for (const auto& [name, sym_ptr] : region_map) {
                    if (sym_ptr->getType() == 0) {
                        std::string name_upper = sym_ptr->getName();
                        std::transform(name_upper.begin(), name_upper.end(), name_upper.begin(), ::toupper);
                        out << name_upper << "\tCON\t0\n";
                    }
                }
            }
	    else { // region_id == 0
		// how many functions in region 0
		for (const auto& [name, sym_ptr] : region_map) {
		    if (sym_ptr->getKind() == 1) {  // 1 = function
		        return_count++;
		        call_count=return_count;
		    }
		}
	    }
        }
    }
    std::string line = code;
    //line.erase(0, line.find_first_not_of(" \t")); // Trim leading whitespace
    if (line.empty()) {
        if (!last_label_line.empty()) {
            out << last_label_line << "\n";
            last_label_line.clear();
        }
        out << "\n";
        return;
    }
    size_t space_pos = line.find(' ');
    std::string first_word = (space_pos == std::string::npos) ? line : line.substr(0, space_pos);
    static const std::set<std::string> opcodes = {
        "LDA", "STA", "ADD", "SUB", "MUL", "DIV", "MOD", "JMP", "JSJ", "NOP", "HLT", "CON", "END", "ORG",
        "EQU", "LDC", "LD1", "LD2", "LD3", "LD4", "LD5", "LD6", "LD7", "LD8", "LD9", "ST1", "ST2", "ST3",
        "ST4", "ST5", "ST6", "ST7", "ST8", "ST9", "CMPA", "JE", "JZ", "JG", "JL", "JNE","JXZ"
    };
    if (opcodes.count(first_word)) {
        if (!last_label_line.empty()) {
            out << last_label_line << "\t" << line << "\n";
            last_label_line.clear();
        } else {
            out << "\t" << line << "\n";
        }

    } else {
        // Assume it's a label (shouldn't happen)
        if (!last_label_line.empty()) {
            out << last_label_line << "\n";
        }
        last_label_line = line;
    }
}

void generate_code(ASTNode* node) {
    //std::cerr << "Node type: " << typeid(*node).name() << std::endl;
    if (!node) return;

    else if (auto stmts = dynamic_cast<StmtsNode*>(node)) {
        generate_code(stmts->left);
        generate_code(stmts->right);
    }

    //example: k=2;
    else if (auto assignNode = dynamic_cast<AssignNode*>(node)) {
    // Πρώτα φτιάξε τον κώδικα της έκφρασης (δεξιά πλευρά)
        generate_code(assignNode->right);

    // Μετά κάνε ST στην μεταβλητή (αριστερή πλευρά)
       if (auto idNode = dynamic_cast<IdNode*>(assignNode->left)) {
           std::string s = idNode->name;
           std::transform(s.begin(), s.end(), s.begin(), ::toupper);
           emit_code("STA " + s);  // Store στη μεταβλητή
        } else {
            std::cerr << "Assignment to non-id not supported\n";
        }
    }
	
//BASIC OPERATIONS (+ - * /)	
    //ADD
    else if (auto add = dynamic_cast<AddNode*>(node)) {
        generate_code(add->left);
        emit_code("STA TEMP1"); //STORE 
        generate_code(add->right);
        emit_code("ADD TEMP1");
    }
    //SUB
    else if (auto sub = dynamic_cast<SubNode*>(node)) {
        generate_code(sub->left);
        emit_code("STA TEMP1"); //STORE
        generate_code(sub->right);
        emit_code("SUB TEMP1");
    }
    //MUL
    else if (auto mul = dynamic_cast<MulNode*>(node)) {
        generate_code(mul->left);
        emit_code("STA TEMP1"); //STORE
        generate_code(mul->right);
        emit_code("MUL TEMP1");
    }
    //DIV
    else if (auto div = dynamic_cast<DivNode*>(node)) {
        generate_code(div->left);
        emit_code("STA TEMP1"); //STORE
        generate_code(div->right);
        emit_code("DIV TEMP1");
    }
    // for future development 
    //else if (auto mod = dynamic_cast<ModNode*>(node)) {
    //    generate_code(mod->left);
    //    emit_code("STA TEMP1"); //STORE
    //    generate_code(mod->right);
    //    emit_code("MOD TEMP1");
    //}

//COMPARISSON 
    //==
    else if (auto eq = dynamic_cast<EqNode*>(node)) {
        
    	generate_code(eq->left);
    	 std::cerr << "hello";
    	emit_code("STA TMP");       // save left on TMP
    	generate_code(eq->right);   // save right 
    	emit_code("CMPA TMP");      // compare
    	std::string label_true = new_label("EQTRUE");
    	std::string label_end  = new_label("EQEND");
    	emit_code("JE " + label_true);
    	emit_code("LDA ZERO");         // false
    	emit_code("JMP " + label_end);
    	emit_code(label_true);
    	emit_code("LDA ONE");         // true
    	emit_code(label_end);
    }
    
    //!=
    else if (auto ne = dynamic_cast<NeNode*>(node)) {
	generate_code(ne->left);
	emit_code("STA TMP");
	generate_code(ne->right);
	emit_code("CMPA TMP");
	std::string label_true = new_label("NETRUE");
	std::string label_end  = new_label("NEEND");
	emit_code("JNE " + label_true);
	emit_code("LDA ZERO");
	emit_code("JMP " + label_end);
	emit_code(label_true);
	emit_code("LDA ONE");
	emit_code(label_end);
    }
    
    //>
    else if (auto gt = dynamic_cast<GtNode*>(node)) {
        generate_code(gt->left);
        emit_code("STA TMP");
        generate_code(gt->right);
        emit_code("CMPA TMP");
        std::string label_true = new_label("GTTRUE");
        std::string label_end  = new_label("GTEND");
        emit_code("JG " + label_true);
        emit_code("LDA ZERO");
        emit_code("JMP " + label_end);
        emit_code(label_true);
        emit_code("LDA ONE");
        emit_code(label_end);
    }

    //>=
    else if (auto ge = dynamic_cast<GeNode*>(node)) {
        generate_code(ge->left);
        emit_code("STA TMP");
        generate_code(ge->right);
        emit_code("CMPA TMP");
        std::string label_true = new_label("GETRUE");
        std::string label_end  = new_label("GEEND");
        emit_code("JGE " + label_true);
        emit_code("LDA ZERO");
        emit_code("JMP " + label_end);
        emit_code(label_true);
        emit_code("LDA ONE");
        emit_code(label_end);
    }
    
    //<
    else if (auto lt = dynamic_cast<LtNode*>(node)) {
        generate_code(lt->left);
        emit_code("STA TMP");
        generate_code(lt->right);
        emit_code("CMPA TMP");
        std::string label_true = new_label("LTTRUE");
        std::string label_end  = new_label("LTEND");
        emit_code("JL " + label_true);
        emit_code("LDA ZERO");
        emit_code("JMP " + label_end);
        emit_code(label_true);
        emit_code("LDA ONE");
        emit_code(label_end);
    }
    
    //<=
    else if (auto le = dynamic_cast<LeNode*>(node)) {
        generate_code(le->left);
        emit_code("STA TMP");
        generate_code(le->right);
        emit_code("CMPA TMP");
        std::string label_true = new_label("LETRUE");
        std::string label_end  = new_label("LEEND");
        emit_code("JLE " + label_true);
        emit_code("LDA ZERO");
        emit_code("JMP " + label_end);
        emit_code(label_true);
        emit_code("LDA ONE");
        emit_code(label_end);
    }

//CONDITION 
// CONDITION (if / else if / else)
else if (auto ifelse = dynamic_cast<IfElseNode*>(node)) {
    std::vector<std::pair<ASTNode*, ASTNode*>> cases; // ζευγάρια (condition, then-block)
    ASTNode* else_block = nullptr;
    ASTNode* current = ifelse;
    // Βρίσκουμε όλα τα if / else if και το else μπλοκ
    while (true) {
        auto curr_if = dynamic_cast<IfElseNode*>(current);
        if (!curr_if) break;
        ASTNode* cond = curr_if->left;
        ASTNode* right = curr_if->right;
        auto branches = dynamic_cast<BranchesNode*>(right);
        if (!branches) break;
        cases.emplace_back(cond, branches->left);  // if (cond) then-block
        if (auto nested_if = dynamic_cast<IfElseNode*>(branches->right)) {
            current = nested_if; // else if
        } else {
            else_block = branches->right; // else
            break;
        }
    }
    std::string end_lbl = new_label("ENDIF");
    std::vector<std::string> else_lbls(cases.size());
    for (size_t i = 0; i < else_lbls.size(); ++i)
        else_lbls[i] = new_label("ELSEIF");
    std::string else_lbl;
    if (else_block) {
        else_lbl = new_label("ELSE");
    }
    for (size_t i = 0; i < cases.size(); ++i) {
        generate_code(cases[i].first); // condition (επιστρέφει 1 ή 0 στο A)
        std::string next_lbl;
        if (i + 1 < else_lbls.size()) {
            next_lbl = else_lbls[i + 1];  // επόμενο else if
        } else {
            // Αν υπάρχει else μπλοκ πηγαίνουμε εκεί, αλλιώς στο τέλος
            next_lbl = else_block ? else_lbl : end_lbl;
        }
        emit_code("\tJXZ " + next_lbl + "\n");  // αν false πήγαινε στο επόμενο else if / else
        generate_code(cases[i].second); // μπλοκ του if / else if
        emit_code("\tJMP " + end_lbl + "\n"); // μετά το μπλοκ πήγαινε στο τέλος
        emit_code(else_lbls[i] + " NOP\n"); // ετικέτα για else if / else
    }

    if (else_block) {
        emit_code(else_lbl + " NOP\n"); // ετικέτα else
        generate_code(else_block);      // κώδικας else
    }
    emit_code(end_lbl + " NOP\n"); // τέλος if-else
}




//WHILE - BREAK - RETURN (LOOP MANAGEMENT) 
    else if (auto whileNode = dynamic_cast<WhileNode*>(node)) {
        int new_region = ++region_counter;
        int old_region = current_region;
        current_region = new_region;
    	std::string label_start = "WHS" + std::to_string(new_region);
    	std::string label_end   = "WHE" + std::to_string(new_region);
    	// Initialize break handling for this region
    	break_flags[current_region] = false;
    	break_targets[current_region] = label_end;
    	// Emit start label
    	emit_code(label_start);
    	// Generate code for loop condition
    	generate_code(whileNode->left);  // Assumes left is condition
    	// Emit conditional jump (exit if false)
    	emit_code("CMPA ONE");            // Assumes condition sets CMPA
    	emit_code("JE " + label_end);    // Jump to end if condition is false
    	// Generate code for loop body
    	generate_code(whileNode->right); // Assumes right is body
    	// Loop back to start
    	emit_code("JMP " + label_start);
    	// End label
    	emit_code(label_end);
    	// Optional comment if a break was used
    	if (break_flags[current_region]) {
            emit_code("; break triggered in while region " + std::to_string(current_region));
    	}
    	// Restore previous region
    	current_region = old_region;
    }
    else if (auto breakNode = dynamic_cast<BreakNode*>(node)) {
    // Ensure current_region is valid
        if (break_targets.count(current_region)) {
            break_flags[current_region] = true;
            emit_code("JMP " + break_targets[current_region]);
        } else {
            emit_code("; Error: 'break' used outside of valid region");
        }
    }

    else if (auto returnNode = dynamic_cast<ReturnNode*>(node)) {
        generate_code(returnNode->child);
        // Inline function get_region_function_name
        if (return_count == 1) {
            emit_code("JMP RET");  // return count is global and takes its value on the emit function
        } 
        else {
            emit_code("JMP J" + std::to_string(return_count)); //use return_count so that multiple jumps wont get mixed up
            //std::cerr << "THE RETURN COUNT IS " <<return_count << "\n"; //for testing 
	    return_count--;
        }
    }

//LOADING OPERATORS 
    else if (auto idNode = dynamic_cast<IdNode*>(node)) {
        std::string s = idNode->name;
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        emit_code("LDA " + s); // LOAD the value 
    }

    else if (auto intNode = dynamic_cast<IntNode*>(node)) {
    	int value = intNode->value;
    	emit_code("LDA " + std::to_string(value));  // LOAD constant value directly
    }
     
//STORE     
    //example: int k=2; (initialization is handled from the emit function here only tha value is changed )
    else if (auto declAssign = dynamic_cast<DeclAssignNode*>(node)) {
        generate_code(declAssign->right);
        auto id = dynamic_cast<IdNode*>(declAssign->left);
        std::string s = id->name;
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        emit_code("STA " + s); //STORE
    }

    else if (auto declList = dynamic_cast<DeclListNode*>(node)) {
        if (declList->left) generate_code(declList->left);
        if (declList->right) generate_code(declList->right);
    }

    else if (auto decls = dynamic_cast<DeclsNode*>(node)) {
    	if (decls->left) generate_code(decls->left);
    	if (decls->right) generate_code(decls->right);
    }

    //example:int k; (this is handled from the emit function during the first initialization)
    else if (auto decl = dynamic_cast<DeclNode*>(node)) {
    	if (decl->left) {
    	    generate_code(decl->left); 
    	}
    	if (decl->right) {
    	    generate_code(decl->right);
    	}
    }
    

    //TODO for future development
    //else if (auto notNode = dynamic_cast<NotNode*>(node)) {
    //    generate_code(notNode->child);
    //    //emit_code("NOT");
    //}

    //TODO for future development 
    //else if (auto uminus = dynamic_cast<UMinusNode*>(node)) {
    //    generate_code(uminus->child);
    //    //emit_code("NEG");
    //}

    else if (auto uplus = dynamic_cast<UPlusNode*>(node)) {
        generate_code(uplus->child); // No operation needed
    }

    else if (auto body = dynamic_cast<BodyNode*>(node)) {
        if (body->left) generate_code(body->left);
        if (body->right) generate_code(body->right);
    }

    else if (auto program = dynamic_cast<ProgramNode*>(node)) {
        if (program->left) generate_code(program->left);
        if (program->right) generate_code(program->right);
    }

    else if (auto constNode = dynamic_cast<ConstNode*>(node)) {
        generate_code(constNode->child);
    }

    else if (auto functionList = dynamic_cast<FunctionListNode*>(node)) {
    	if (functionList->left) generate_code(functionList->left);
    	if (functionList->right) generate_code(functionList->right);
    }

    else if (auto paramsBody = dynamic_cast<ParamsBodyNode*>(node)) {
    	if (paramsBody->left) generate_code(paramsBody->left);
    	if (paramsBody->right) generate_code(paramsBody->right);
    }

//FUNCTION 
    else if (auto function = dynamic_cast<FunctionNode*>(node)) {
        if (auto idNode = dynamic_cast<IdNode*>(function->left)) {
            std::string func_name = idNode->name;
        // check if the function has already been declared
        if (generated_functions.count(func_name)) {
            // std::cerr << "[SKIP] Function already generated: " << func_name << std::endl; //for testing usage only
            return;
        }
        std::string s = func_name;
	std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        generated_functions.insert(s); // mark as already declared
        emit_code(s);        
        if (function->right) generate_code(function->right);
        } else {
            if (function->left) generate_code(function->left);
            if (function->right) generate_code(function->right);
        }
    }

    //function calls 
    else if (auto call = dynamic_cast<CallNode*>(node)) {
	    if (call->right) {
	        generate_code(call->right);  // πχ generate_code για λίστα ορισμάτων
	    }
        if (call->left) {
            if (auto idNode = dynamic_cast<IdNode*>(call->left)) {
        	std::string s = idNode->name;
		std::transform(s.begin(), s.end(), s.begin(), ::toupper);
            	emit_code("JSJ " + s +"\nJ" + std::to_string(call_count)+"	NOP"); //CALL function and add the RETURN 
            	call_count--;
            	//(nop does nothing) call_count is reduced after each call (not on main)
            	//so the function knows where to return after the call  
            } else {
                generate_code(call->left);
            }
    	}
    }
    
    
    else if (auto initTail = dynamic_cast<InitTailNode*>(node)) {
    if (initTail->left) generate_code(initTail->left);
    if (initTail->right) generate_code(initTail->right);
}

    

    else {
    	std::cerr << "Unknown ASTNode type in generate_code: " << typeid(*node).name() << " at " << node << std::endl;
	}
}
