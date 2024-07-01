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

// Sort alphabetically, Descneding, for ouput format
bool sort_by_alpha(const string a, const string b);

class QM_solver{
private:
    int Var_Num;            //Store the variable number from input txt
    vector <int> ON_set;    //Store the ON set from input txt
    vector <int> DC_set;    //Store the DC set from input txt
    vector <string> ON_s_set; //Store the ON set in string type
    vector <string> prime_list; //Store the Final Prime implicants for output
    vector <string> im;         //Store implicants from input txt and temporary implicants
public:
    QM_solver();
    ~QM_solver();

    void set_var_num(int num);  //Set Var_Num
    void add_on(int num);       //add a ON set to ON_set vector 
    void add_dc(int num);       //add a DC set to ON_set vector
    
    //Compare 2 strings with GrayCode pattern and output the index of differenet place
    //If they are not graycode neighbors, return -1
    int GrayCode_Diff_indx(string a, string b);
    //Merge two implicants by replacing with '-'
    string Replace(string a, int dif_idx);
    //transform decimal int data to a binary string data
    string Dec2Bin(int num);
    //To output the on set in string type (vector <string> ON_s_set)
    vector<string> & get_on_set();
    //To output the Final Prime implicants (vector <string> prime_list)
    vector<string> & get_prime(); 
    //To process the Quine-McCluskey Algorithm
    void solve();
};