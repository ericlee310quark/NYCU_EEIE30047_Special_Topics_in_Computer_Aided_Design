
#include "Petrick_solver.h"
#include "QM_solver.h"

bool is_fewer_term(set<string> a, set<string> b){
    if(a.size()<b.size()){
        return 1;
    }
    else if(a.size()>b.size()){
        return 0;
    }
    else{
        int literal_a = 0, literal_b = 0;
        int weighted_a = 0, weighted_b=0;
        int weighted_sum_a = 0, weighted_sum_b = 0;
        vector < string > v_a(a.begin(),a.end());
        vector < string > v_b(b.begin(),b.end());
        
        for(int i=0;i<int(v_a.size());i++){
            weighted_a = 0;
            weighted_b = 0;
            for(int j=0;j<int(v_a[i].size());j++){
                if(v_a[i][j]!='-'){ literal_a++;}
                else              { weighted_a++;}
                if(v_b[i][j]!='-'){ literal_b++;}
                else              { weighted_b++;}
                weighted_a*=2;
                weighted_b*=2;
            }
            weighted_sum_a += weighted_a;
            weighted_sum_b += weighted_b;
        }
        if(literal_a!=literal_b){ return literal_a < literal_b; }
        else                    { return weighted_sum_a<=weighted_sum_b; }
    }  
}



bool Petrick_Solver::is_include_in_prime(string ON_term, string prime_term){
    for(int i=0; i < int(ON_term.size()); i++){
        if(prime_term[i]!='-' && prime_term[i]!=ON_term[i])
            return 0;
    }
    return 1;
}
Petrick_Solver::Petrick_Solver(vector<string> &prime_list, vector<string> &ON_s_set){
    
    set <string> ON_reduce_set;
    set <string> ON_kill_set;
    for(int i=0; i < int(ON_s_set.size()); i++){
        vector<string> term;
        int count = 0;
        if(ON_kill_set.find(ON_s_set[i])==ON_kill_set.end()){
            for(int j=0; j<int(prime_list.size()); j++){
                if(count>1){
                    break;
                }
                if(is_include_in_prime(ON_s_set[i],prime_list[j])){
                    term.push_back(prime_list[j]);
                    count++;
                }
            }
        }
        else{
            continue;
        }

        if((count>1)&&(ON_kill_set.find(ON_s_set[i])==ON_kill_set.end())){
            ON_reduce_set.insert(ON_s_set[i]);
        }else{
            for(int m=0;m<int(ON_s_set.size());m++){
                if(is_include_in_prime(ON_s_set[m],term[0])){
                    ON_reduce_set.erase(ON_s_set[m]);
                    ON_kill_set.insert(ON_s_set[m]);
                }
            }
            must_p_terms.push_back(term[0]);
        }
    }

    vector <string> ON_reduce_vec(ON_reduce_set.begin(),ON_reduce_set.end());

    for(int i=0; i < int(ON_reduce_vec.size()); i++){
        vector<string> POS_term;
        for(int j=0; j<int(prime_list.size()); j++){
            if(is_include_in_prime(ON_reduce_vec[i],prime_list[j])){
                POS_term.push_back(prime_list[j]);
            }
        }
        POS.push_back(POS_term);   
    }
}

string set2string(set<string> &temp) {
	stringstream ss;
    set<string>::iterator iter;
	for( iter=temp.begin() ; iter!=temp.end() ; iter++ )
		ss<< *iter ;
	string s;
	ss>> s ;
	return s ;
}

Petrick_Solver::~Petrick_Solver(){}

void Petrick_Solver::P2S(set<string> &p_term, int idx, int end_idx){
    if(p_term.size()>min_cost){
        return;
    }
    if(idx==end_idx){
        string terms_a = set2string(p_term);
        if(p_term.size()<=min_cost){
            if(SOP_s_list.find(terms_a)==SOP_s_list.end()){
                SOP_s_list.insert(terms_a);
                if(p_term.size()<min_cost){
                    SOP.clear();
                    SOP_s_list.clear();
                }
                SOP.push_back(p_term);
                min_cost = p_term.size();
            }
        }
    }
    
    if(idx<end_idx){
        for(int i=0;i<int(POS[idx].size());i++){
                if(p_term.find(POS[idx][i])==p_term.end()){
                    p_term.insert(POS[idx][i]);
                    P2S(p_term, idx+1, end_idx);
                    p_term.erase(POS[idx][i]);
                }
                else{
                    P2S(p_term, idx+1, end_idx);
                }
        }
    }

}


vector<string> Petrick_Solver::solve(){

    set<string> p_terms;
    for(int i=0;i<must_p_terms.size();i++){
        p_terms.insert(must_p_terms[i]);
    }
    min_cost = 999999;
    P2S(p_terms,0,POS.size());
    sort(SOP.begin(),SOP.end(),is_fewer_term);
    vector<string> ans(SOP[0].begin(), SOP[0].end());
    sort(ans.begin(),ans.end(),sort_by_alpha);
    
    return ans ;
}
