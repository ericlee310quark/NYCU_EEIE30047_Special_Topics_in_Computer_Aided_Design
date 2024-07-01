#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <bitset>
#include <math.h>
#include <algorithm>

using namespace std;

class Petrick_Solver{

private:
    //Store POS form
    vector < vector < string > > POS;
    //Store SOP form (output)
    vector < set < string > > SOP;
    //To store the path that have ever been to
    set < string > SOP_s_list;
    //Store the essential term
    vector < string > must_p_terms;
    //Store the current min cost term
    //cost: the number of terms
    int min_cost;
public:
    //Initial Petrick Solver
    //Read the On sets and Prime Implicants from QM_solver
    //It will transformthe Prime Implicants to POSS form
    //Also it will extract the essential term from the Prime Implicants
    //Single term in the original POS form will store in vector < string > must_p_terms
    //Other will keep in vector < vector < string > > POS
    //Then that will ease the following steps by considering fewer terms
    Petrick_Solver(vector<string> &prime_list, vector<string> &ON_s_set);
    ~Petrick_Solver();
    //Process the Petrick's Method
    //And reorder the SOP to prepare for output
    vector<string> solve();
    //Check if prime_term is inuclded in the ON_set
    bool is_include_in_prime(string ON_term, string prime_term);
    //Expand the rest of all POS
    //It applys Branch and Bound Algorithm to avoid memeory crash
    //It only allows to store the possible SOP anser with the current minimal number of terms
    void P2S(set<string> &p_term, int idx, int end_idx);
};

