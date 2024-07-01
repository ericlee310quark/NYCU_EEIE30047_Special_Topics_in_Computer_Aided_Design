#include "QM_solver.h"
#include "Petrick_solver.h"

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
#include <time.h>


using namespace std;

void init_qm(ifstream &f_in, QM_solver &qm_solver){
    vector <string> input_strings;
    string input_row;
    int num;

    while(getline(f_in, input_row)){
        stringstream ss;
        if((input_row.find(".i",0))!=-1){
            getline(f_in, input_row);
            ss << input_row;
            ss >> num;
            qm_solver.set_var_num(num);
        }
        else if((input_row.find(".m",0))!=-1){
            getline(f_in, input_row);
            ss << input_row;
            while(ss >> num){
                qm_solver.add_on(num);
            }
        }
        else if((input_row.find(".d",0))!=-1){
            getline(f_in, input_row);
            ss << input_row;
            int d_idx;
            while(ss >> num){
                qm_solver.add_dc(num);
            }
        }
        else{
            cerr << "Unsupport input format." << endl;
        }
    }


}



int main(int argc, char *argv[]){
    //double start_clock, end_clock;
    //start_clock = clock();
    if(argc!=3){
        cerr << "Wrong number of arguments. -> ./xxxxx.o <input_txt> <output_txt>" << endl;
        return 1;
    }

    ifstream input_txt(argv[1]);
    
    ofstream output_txt(argv[2], std::ios::binary);
    if (!output_txt) {
        std::cerr << "[Error] Failed to create output BMP file." << std::endl;
        return 1;
    }



    //***   Quine_McCluskey     ***

    QM_solver qm;
    init_qm(input_txt, qm);
    qm.solve();

    //cout<<"here"<<endl;

    vector <string> pri_im = qm.get_prime();
    //cout<< "qm over"<<endl;
    Petrick_Solver ps(qm.get_prime(),qm.get_on_set());


    vector <string> ans = ps.solve();




    //cout<< "-------------------------"<<endl;
    int pri_im_size = pri_im.size();
/*    
    cout<<".p "<<pri_im_size<<endl;
    for(int i=0;i<pri_im_size&&(i<15);i++){
//        int pos_idx = 0;
        for(int j=0;j<int(pri_im[i].size());j++){
            
            if(qm.get_prime()[i][j]!='-'){
                if(qm.get_prime()[i][j]=='0'||qm.get_prime()[i][j]=='1'){
                    if(j==0)        { cout<<"A"; }
                    else if(j==1)   { cout<<"B"; }
                    else if(j==2)   { cout<<"C"; }
                    else if(j==3)   { cout<<"D"; }
                    else if(j==4)   { cout<<"E"; }
                    else if(j==5)   { cout<<"F"; }
                    else if(j==6)   { cout<<"G"; }
                    else if(j==7)   { cout<<"H"; }
                    if(qm.get_prime()[i][j]=='0'){
                        cout<<"\'";
                    }
                }
   
            }
           //cout << pri_im[i][j];
        }
        cout<<endl;

    }
    cout<< endl;
*/
//----------------------------------------------------------------
    output_txt<<".p "<<pri_im_size<<endl;
    for(int i=0;i<pri_im_size&&(i<15);i++){
//        int pos_idx = 0;
        for(int j=0;j<int(pri_im[i].size());j++){
            
            if(qm.get_prime()[i][j]!='-'){
                if(qm.get_prime()[i][j]=='0'||qm.get_prime()[i][j]=='1'){
                    if(j==0)        { output_txt<<"A"; }
                    else if(j==1)   { output_txt<<"B"; }
                    else if(j==2)   { output_txt<<"C"; }
                    else if(j==3)   { output_txt<<"D"; }
                    else if(j==4)   { output_txt<<"E"; }
                    else if(j==5)   { output_txt<<"F"; }
                    else if(j==6)   { output_txt<<"G"; }
                    else if(j==7)   { output_txt<<"H"; }
                    if(qm.get_prime()[i][j]=='0'){
                        output_txt<<"\'";
                    }
                }
   
            }
           //cout << pri_im[i][j];
        }
        output_txt<<endl;

    }
    output_txt<< endl;




//----------------------------------------------------------------
    int literal_count = 0;

    int mc_size = ans.size();
    /*
    cout <<".mc "<<mc_size<<endl;
    for(int i=0;i<int(ans.size());i++){
        for(int j=0;j<int(ans[i].size());j++){
            
            if(ans[i][j]!='-'){
                if(ans[i][j]=='0'||ans[i][j]=='1'){

                    literal_count++;
                    if(j==0)        { cout<<"A"; }
                    else if(j==1)   { cout<<"B"; }
                    else if(j==2)   { cout<<"C"; }
                    else if(j==3)   { cout<<"D"; }
                    else if(j==4)   { cout<<"E"; }
                    else if(j==5)   { cout<<"F"; }
                    else if(j==6)   { cout<<"G"; }
                    else    { cout<<"H"; }
                    if(ans[i][j]=='0'){
                        cout<<'\'';
                    }
                }
            }            
           //cout<<ans[i][j];
        }
        cout<<endl;
    }
    cout<< "literal="<<literal_count<<endl;
*/
//--------------------------------------------------------------------------
    //literal_count = 0;
    output_txt <<".mc "<<mc_size<<endl;
    for(int i=0;i<int(ans.size());i++){
        for(int j=0;j<int(ans[i].size());j++){
            
            if(ans[i][j]!='-'){
                if(ans[i][j]=='0'||ans[i][j]=='1'){

                    literal_count++;
                    if(j==0)        { output_txt<<"A"; }
                    else if(j==1)   { output_txt<<"B"; }
                    else if(j==2)   { output_txt<<"C"; }
                    else if(j==3)   { output_txt<<"D"; }
                    else if(j==4)   { output_txt<<"E"; }
                    else if(j==5)   { output_txt<<"F"; }
                    else if(j==6)   { output_txt<<"G"; }
                    else    { output_txt<<"H"; }
                    if(ans[i][j]=='0'){
                        output_txt<<'\'';
                    }
                }
            }            
           //cout<<ans[i][j];
        }
        output_txt<<endl;
    }
    output_txt<< "literal="<<literal_count;


    //end_clock = clock();

    //cout<<"Time cost: "<<(end_clock - start_clock)/CLOCKS_PER_SEC <<" sec" << endl;








    return 0;
}