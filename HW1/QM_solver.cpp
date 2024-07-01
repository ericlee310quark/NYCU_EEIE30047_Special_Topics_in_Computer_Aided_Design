#include "QM_solver.h"
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

vector<string> & QM_solver::get_on_set() {	return ON_s_set ;	}
vector<string> & QM_solver::get_prime() {	return  prime_list;	}


bool sort_by_alpha(const string a, const string b) {
	int cnt_a = 0 , cnt_b = 0 , size = a.length() ;
    bool flag=0;
	for( int i=0 ; i<size ; i++ ) {
		if( a[i]!='-' ){
            if(a[i]=='1'){ cnt_a+=2;}
            else         { cnt_a++;}
        }
		if( b[i]!='-' ){
            if(b[i]=='1'){ cnt_b+=2; }
            else         { cnt_b++;  }
        }
        cnt_a*=3;
        cnt_b*=3;
	}
	return ( cnt_a > cnt_b ) ;
}

QM_solver::QM_solver(){};
QM_solver::~QM_solver(){};
void QM_solver::set_var_num(int num){   
    Var_Num = num; 
    //cout<<"Store Var num: "<<num<<endl;
}
void QM_solver::add_on(int num){    
    ON_set.push_back(num); 
    //cout<<"ON_set: push "<<num<<endl; 
}
void QM_solver::add_dc(int num){    
    DC_set.push_back(num); 
    //cout<<"DC_set: push "<<num<<endl;
}

int QM_solver::GrayCode_Diff_indx(string a, string b){
    int dif_num = 0;
    int dif_idx = 0;
    int itr_idx = 0;
    if(a.size()!=b.size()){
        cerr << "[FATAL] QM_solver::isGrayCode(string a, string b) -> a.size() should be the same as b.size()" << endl;
    }
    while(itr_idx < int(a.size())){
        if(a[itr_idx] != b[itr_idx]){
            dif_num++;
            dif_idx = itr_idx;
        }            
        if(dif_num > 1)
            return -1;
        itr_idx++;
    }
    if(dif_num!=1)
        return -1;
    else
        return dif_idx;
}
string QM_solver::Replace(string a, int dif_idx){
    a[dif_idx] = '-';
    return a;
}


string QM_solver::Dec2Bin(int num){
    int n = Var_Num-1;
    string bin_str;
    string bin_buffer;
    bin_str = "";
    bin_buffer = "";
    int pad_count = 0;
    if(num==0){
        for(int i=0;i<Var_Num;i++){
            bin_str+="0";
        }
        return bin_str;
    }
    while((pow (2, n))>num){
        bin_str+="0";      
        n--;
    }
    while (num > 0) {
        bin_buffer = std::to_string(num % 2) + bin_buffer;
        num /= 2;
    }    
    bin_str+=bin_buffer;
    return bin_str;
}

void QM_solver::solve(){
    //Set up On sets
    for(int i=0; i < int(ON_set.size()); i++){
        string abc = Dec2Bin(ON_set[i]);    //Transform to Binary String
        im.push_back(abc);                  //Place into implicants vector
        ON_s_set.push_back(abc);            //Place into Onset String vector
    }
    //Set up DC sets
    for(int i=0; i < int(DC_set.size()); i++){
        string abc = Dec2Bin(DC_set[i]);
        im.push_back(abc);
    }
    int dif_idx = 0;

    while(im.size()!=0){
        vector<string> next_im_list;
        vector<bool> is_imp(int(im.size()), 1);
        for(int i=0; i < int(im.size()); i++){
            for(int j=i+1; j < int(im.size()); j++){
                dif_idx = GrayCode_Diff_indx(im[i],im[j]);      //Check if they are GrayCode-like
                if(dif_idx != -1){
                    string new_term = Replace(im[i], dif_idx);    //Replace and form new Implicant
                    if(find(next_im_list.begin(),next_im_list.end(), new_term)==next_im_list.end()){
                        next_im_list.push_back(new_term);         //Store the new Implicant
                    }
                    is_imp[i] = 0;      //Refresh the is_prime_implicant flag
                    is_imp[j] = 0;      //Refresh the is_prime_implicant flag
                }
            }
            if(is_imp[i]==1){
                prime_list.push_back(im[i]);    //Store prime implicant
            }

        }
        im.clear();
        for(int i=0; i < int(next_im_list.size()); i++){
            im.push_back(next_im_list[i]);       //Prepare for next-level QM algorithm
        }
        next_im_list.clear();                   //Clear the temp vector
    }
    sort( prime_list.begin() , prime_list.end() , sort_by_alpha );  //Sort alphabatically
}