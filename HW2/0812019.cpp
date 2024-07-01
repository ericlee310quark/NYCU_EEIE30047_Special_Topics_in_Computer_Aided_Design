#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>

#include <bitset>
#include <cstring>
#include <math.h>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <map>
#include <queue>
#include <regex>
#include <time.h>
using namespace std;

int CELL_RISE = 0;
int CELL_FALL = 1;
int RISE_TRANS = 2;
int FALL_TRANS = 3;

struct Input_Pin{
    long double capacitance;
};

struct Output_Pin{
    string pin_idx;
    long double capacitance;
    //*  Power Ana  
    vector < vector <long double > > rise_power_lut;
    vector < vector <long double > > fall_power_lut;
    //* Time Ana
    vector < vector <long double> > cell_rise_lut;
    vector < vector <long double> > cell_fall_lut;
    vector < vector <long double> > rise_trans;
    vector < vector <long double> > fall_trans;
};

struct Cell_INFO{
    map <string, Input_Pin>  input_pin_list;
    Output_Pin output_pin_list; 
};

class Lib_Viewer{

private:
    string time_unit;
    string voltage_unit;
    string current_unit;
    string capacitve_load_unit;
    vector <long double> lut_x_axis;
    vector <long double> lut_y_axis;
    int lut_x_len;
    int lut_y_len;
    map <string, Cell_INFO> cell_lib;
public:
    Lib_Viewer(ifstream &f_in);
    ~Lib_Viewer();
    void set_time_unit(string s_time_unit);
    void set_voltage_unit(string s_volt_unit);
    void set_current_unit(string s_curr_unit);
    void set_cap_load_unit(string s_cap_unit);
    void set_lut_x_axis(long double x_value);
    void set_lut_y_axis(long double y_value);

    long double lut_inp(string gate, int table_idx, long double x_value, long double y_value);
    long double lut_cap(string gate, string port_name);
};
void Lib_Viewer::set_time_unit(string s_time_unit){     time_unit = s_time_unit;        }
void Lib_Viewer::set_voltage_unit(string s_volt_unit){ voltage_unit = s_volt_unit;      }
void Lib_Viewer::set_current_unit(string s_curr_unit){ current_unit = s_curr_unit;      }
void Lib_Viewer::set_cap_load_unit(string s_cap_unit){ capacitve_load_unit = s_cap_unit;}

void Lib_Viewer::set_lut_x_axis(long double x_value){
    lut_x_axis.push_back(x_value);
    lut_x_len++;
}
void Lib_Viewer::set_lut_y_axis(long double y_value){
    lut_y_axis.push_back(y_value);
    lut_y_len++;
}
long double Lib_Viewer::lut_inp(string gate, int table_idx, long double x_value, long double y_value){
    map <string, Cell_INFO>::iterator  iter = cell_lib.find(gate);
    if(iter == cell_lib.end()){
        cerr<<"No Find This Gate INFO in the lib."<<endl;
    }
    
    vector < vector <long double> >  temp_table;
    int x_0 = 0, x_1 = 1;
    int y_0 = 0, y_1 = 1;
    long double temp_y_0 = 0, temp_y_1 = 0;

    long double result;
    //find x indx
    for(int i=1;i<lut_x_len-1;i++){
        if(x_value>lut_x_axis[i]){
            x_0++;
            x_1++;
        }
    }
    //find y indx
    for(int i=1;i<lut_y_len-1;i++){
        if(y_value>lut_y_axis[i]){
            y_0++;
            y_1++;
        }
    }

    if(table_idx==CELL_RISE){
        temp_table = iter->second.output_pin_list.cell_rise_lut;
    }
    else if(table_idx==CELL_FALL){
        temp_table = iter->second.output_pin_list.cell_fall_lut;
    }
    else if(table_idx==RISE_TRANS){
        temp_table = iter->second.output_pin_list.rise_trans;
    }
    else if(table_idx==FALL_TRANS){
        temp_table = iter->second.output_pin_list.fall_trans;
    }
    temp_y_0 = ((temp_table[y_0][x_1]-temp_table[y_0][x_0])*(x_value-lut_x_axis[x_0])/(lut_x_axis[x_1]-lut_x_axis[x_0]))+temp_table[y_0][x_0];
    temp_y_1 = ((temp_table[y_1][x_1]-temp_table[y_1][x_0])*(x_value-lut_x_axis[x_0])/(lut_x_axis[x_1]-lut_x_axis[x_0]))+temp_table[y_1][x_0];
    result = ((temp_y_1-temp_y_0)*(y_value-lut_y_axis[y_0])/(lut_y_axis[y_1]-lut_y_axis[y_0]))+ temp_y_0;

    return result;
}
long double Lib_Viewer::lut_cap(string gate, string port_name){
    map <string, Cell_INFO>::iterator  iter = cell_lib.find(gate);
    if(iter == cell_lib.end()){
        cerr<<"No Find This Gate INFO in the lib."<<endl;
        return -1;
    }
    //* port_idx = 0: I, A1
    if(iter->second.input_pin_list.find(port_name)!=iter->second.input_pin_list.end()){
        return iter->second.input_pin_list[port_name].capacitance;
    }
    else{
       cerr<<"No Find This Port INFO in the lib."<<endl;
       return -1;
    }
}


Lib_Viewer::Lib_Viewer(ifstream &f_in){
    regex word_regex("\\w+");
    regex float_regex("(0\\.[\\d]+,?)+");
    regex replace_regex("(\\/\\*.*\\*\\/)|(\\/\\/.*)|(\\s+)");
    regex space_regex("(\\s+)");
    regex find_regex("(cell\\(.*\\))|(pin\\(.*\\))|(capacitance:0.[\\d]+)|(0.[\\d]+)");
    smatch s;
    string unit_string;
    string line;
    string cell;
    string pin;
    
    vector<string> cellName = { "NOR2X1", "INVX1", "NANDX1" };
    vector<string> tableName = { "rise_power", "fall_power", "cell_rise", "cell_fall", "rise_transition", "fall_transition" };

    bool time_unit_done = 0, voltage_unit_done = 0, current_unit_done = 0, capacitve_load_unit_done = 0;
    bool x_axis_done = 0, y_axis_done = 0;
    
    lut_x_len = 0;
    lut_y_len = 0;

    while(getline(f_in, line)) {
        line = regex_replace(line, replace_regex, "");
        if(!time_unit_done){
            regex time_unit_regex("time_unit:\"(\\w+)\"");
            if(regex_search(line, s, time_unit_regex)){
                unit_string = s.str(1);
                set_time_unit(unit_string);
                time_unit_done = 1;
            }
        }
        else if(!voltage_unit_done){
            regex voltage_unit_regex("voltage_unit:\"(\\w+)\"");
            if(regex_search(line, s, voltage_unit_regex)){
                unit_string = s.str(1);
                set_voltage_unit(unit_string);
                voltage_unit_done = 1;
            }
        }
        else if(!current_unit_done){
            regex current_unit_regex("current_unit:\"(\\w+)\"");
            if(regex_search(line, s, current_unit_regex)){
                unit_string = s.str(1);
                set_current_unit(unit_string);
                current_unit_done = 1;
            }
        }
        else if(!capacitve_load_unit_done){
            regex capacitive_unit_regex("capacitive_load_unit\\((\\d+\\.\\d+\\,\\w+)\\)");
            if(regex_search(line, s, capacitive_unit_regex)){
                unit_string = s.str(1);
                set_cap_load_unit(unit_string);
                capacitve_load_unit_done = 1;
            }
        }
        else if(!x_axis_done){
            if(regex_search(line, s, float_regex)){
                unit_string = s.str(0);
                string float_num;
                while(unit_string.find(",")!=-1){
                    int indx = unit_string.find(",");
                    float_num = unit_string.substr(0,indx);
                    unit_string.replace(0,indx+1,"");
                    set_lut_x_axis(stold(float_num));
                }            
                set_lut_x_axis(stold(unit_string));
                x_axis_done = 1;
            }
        }
        else if(!y_axis_done){
            if(regex_search(line, s, float_regex)){
                unit_string = s.str(0);
                string float_num;
                while(unit_string.find(",")!=-1){
                    int indx = unit_string.find(",");
                    float_num = unit_string.substr(0,indx);
                    unit_string.replace(0,indx+1,"");
                    set_lut_y_axis(stold(float_num));
                }            
                set_lut_y_axis(stold(unit_string));
                y_axis_done = 1;
            }
        }

        //TODO cell detect
        else {
            regex cell_regex("cell\\(([\\w]+)\\)[\\{]+");
            string cell_name;
            if(regex_search(line, s, cell_regex)){
                cell_name = s.str(1);
                //cout<<unit_string<<endl;
                //TODO: set up cell
                Cell_INFO* temp_cell = new Cell_INFO;
                regex pin_regex("pin\\(([\\w]+)\\)[\\{]+");
                string pin_name_s, dir_s;
                int left_count = 1;
                while(left_count!=0){
                    getline(f_in, line);
                    line = regex_replace(line, replace_regex, "");
                    left_count +=int(count(line.begin(), line.end(), '{'));
                    left_count -=int(count(line.begin(), line.end(), '}'));
                    long double cap=0;

                    if(regex_search(line, s, pin_regex)){
                        pin_name_s = s.str(1);
                        //cout<<"pin: "<<pin_name_s<<endl;
                        getline(f_in, line);
                        line = regex_replace(line, replace_regex, "");
                        regex dir_regex("direction:([\\w]+)[;]+");
                        regex cap_regex("capacitance:([\\d]+\\.[\\d]+)[;]+");
                        
                        if(regex_search(line, s, dir_regex)){
                            dir_s = s.str(1);
                            //cout<<"dir: "<<dir_s<<endl;
                            getline(f_in, line);
                            line = regex_replace(line, replace_regex, "");
                            if(regex_search(line, s, cap_regex)){
                                cap = stold(s.str(1));
                                //cout<<"cap: "<<cap<<endl;
                            }

                            // ! OUTPUT pin
                            if(dir_s.find("output")==0){

                                getline(f_in, line);
                                getline(f_in, line);
                                Output_Pin* temp_out_pin = new Output_Pin;
                                temp_out_pin->pin_idx = pin_name_s;
                                temp_out_pin->capacitance = cap;
                                //Rise Power
                                vector < vector <long double> > temp_table;
                                vector <long double> temp_row;
                                string value_temp;
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->rise_power_lut.push_back(temp_row);
                                        temp_row.clear();
                                }
                                temp_row.clear();
                                getline(f_in, line);
                                getline(f_in, line);
                                
                                // fall_power
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->fall_power_lut.push_back(temp_row);
                                        temp_row.clear();
                                }
                                temp_row.clear();
                                getline(f_in, line);
                                getline(f_in, line);
                                getline(f_in, line);
                                getline(f_in, line);
                                // cell rise
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->cell_rise_lut.push_back(temp_row);
                                        temp_row.clear();
                                }
                                getline(f_in, line);
                                getline(f_in, line);
                                //cell fall
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->cell_fall_lut.push_back(temp_row);
                                        temp_row.clear();
                                }
                                getline(f_in, line);
                                getline(f_in, line);
                                //rise transition
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->rise_trans.push_back(temp_row);
                                        temp_row.clear();
                                }
                                getline(f_in, line);
                                getline(f_in, line);
                                //fall transition
                                for(int y_len=0;y_len<lut_y_len;y_len++){
                                    if(y_len!=(lut_y_len)){
                                        getline(f_in, line);
                                        line = regex_replace(line, replace_regex, "");
                                    }
                                    if(regex_search(line, s, float_regex)){
                                        value_temp = s.str(0);
                                        //cout<<value_temp<<endl;
                                        string float_num;
                                        while(value_temp.find(",")!=-1){
                                            int indx = value_temp.find(",");
                                            float_num = value_temp.substr(0,indx);
                                            value_temp.replace(0,indx+1,"");
                                            temp_row.push_back(stold(float_num));
                                        }            
                                        temp_row.push_back(stold(value_temp));
                                        
                                    }
                                        temp_out_pin->fall_trans.push_back(temp_row);
                                        temp_row.clear();
                                }
                                
                                temp_cell->output_pin_list = *temp_out_pin;//
                                delete temp_out_pin;
                            }
                            else if(dir_s.find("input")==0){
                                Input_Pin* temp_input = new Input_Pin;
                                temp_input->capacitance = cap;
                                temp_cell->input_pin_list.insert(std::pair<string, Input_Pin>(pin_name_s, *temp_input));
                                delete temp_input;
                            }
               
                        }         
                    }      
                }
                cell_lib.insert(std::pair < string, Cell_INFO >(cell_name,*temp_cell));
                delete temp_cell;
            }
        }    
    }
    //cout<<"========="<<endl;
    //cout<<"lib size: "<<cell_lib.size()<<endl;
    //for(const auto& s : cell_lib){
    //    cout<<"cell name: "<<s.first<<endl;
    //    cout<<s.second.output_pin_list.pin_idx<<endl;
    //}
}
Lib_Viewer::~Lib_Viewer(){}

struct wire;

struct gate{
    string gate_type;
    string gate_name;
    long double output_cap;
    long double delay_sum;
    //!***
    long double prop_rise_delay;
    long double prop_fall_delay;
    long double trans_time;
    bool longest_when_rise;
    
    string in_0;
    string in_1;
    string out_wire;    
    vector <string> longest_path;
    int visit_time;
    //bool is_input_node;
    bool is_output_gate;
};


struct wire{
    string wire_name;
    bool is_global_start;
    bool is_global_output;
    string in_gate;
    map <string, string> out_map;

    long double wire_delay;
    long double delay_sum;
    long double trans_time;
};


class Net{
private:
    string module_name;

    set <string> global_input_set;
    set <string> global_output_set;

    map <string, wire> wire_map;
    map <string, gate> gate_map;

    set <string> global_input_gate;
    set <string> global_output_gate;

    queue <gate*> gate_queue; 
    vector <string> output_step2;
    
    int wire_max_size;
    int gate_max_size;
public:
    Net(ifstream &f_in);
    ~Net();
    void Step1(Lib_Viewer &lib_data);
    void Step2_Step3(Lib_Viewer &lib_data);
    
    friend bool sort_by_cap(gate gate_a, gate gate_b);
    friend bool sort_by_prop(gate* gate_a_s, gate* gate_b_s);
    friend bool sort_by_path(gate* gate_a, gate* gate_b);
};
Net::Net(ifstream &f_in){
    string line;
    regex replace_regex("(\\/\\/.*)|(\\/\\*((?!\\*\\/).)*\\*\\/)|(^\\s+)");
    regex module_regex(".*module\\s*(\\w*)\\s*\\(*[\\w\\d,]*\\)*;*");
    regex openComment_regex("\\/\\*.*");
    regex closeComment_regex(".*\\*\\/");
    regex space_regex("(\\s+)");
    regex word_regex("\\w+");
    regex cell_regex("(NOR2\\w*)|(NAND\\w*)|(INV\\w*)");
    regex wire_regex("(output)|(input)|(wire)");
    smatch firstWord;
    bool module = true;
    bool comment = false;
    bool declareMode = false;
    bool cellMode = false;
    bool pinMode = false;

    //* Parameters: Wire Type Index
    int Input_Wire = 0;
    int Output_Wire = 1;
    int Inter_Wire = 2;
    //* Parameters: Gate Type Index
    int INVX1_Cell = 0;
    int NOR2X1_Cell = 1;
    int NANDX1_Cell = 2;

    int wire_type_idx = -1;
    int gate_type_idx = -1;
    wire_max_size = 0;
    gate_max_size = 0;

    gate* gate_temp;
    while(getline(f_in, line)) {
        
        line = regex_replace(line, replace_regex, "");
        line = regex_replace(line, space_regex, " ");

        if (line.size()) {
            //* Filter Comment
            if (comment) {
                if (regex_search(line, firstWord, closeComment_regex)) {
                    line = regex_replace(line, closeComment_regex, "");
                    comment = false;
                }
                else
                    continue;
            }
            if (regex_search(line, firstWord, openComment_regex)) {
                line = regex_replace(line, openComment_regex, "");
                comment = true;
            }
            if(module&&regex_search(line, firstWord, module_regex)){
                module = false;
                module_name = firstWord.str(1);
            }


            //* Declare Wire
            if (regex_search(line, firstWord, wire_regex)) {
                declareMode = true;

                if (firstWord.str() == "input")
                    wire_type_idx = Input_Wire;
                else if (firstWord.str() == "output")
                    wire_type_idx = Output_Wire;
                else if (firstWord.str() == "wire")
                    wire_type_idx = Inter_Wire;

            }
            //* Declare Gate
            else if (regex_search(line, firstWord, cell_regex)) {
                if (pinMode){
                    gate_map.insert(std::pair<string, gate>(gate_temp->gate_name, *gate_temp));
                    gate_max_size++;
                    if(gate_temp->visit_time==0){
                        global_input_gate.insert(gate_temp->gate_name);
                        gate_queue.push(&(gate_map[gate_temp->gate_name]));
                    }
                    if(global_output_set.find(gate_temp->out_wire)!=global_output_set.end()){
                        global_output_gate.insert(gate_temp->gate_name);
                    }

                }

                declareMode = false;
                cellMode = true;
                pinMode = false;

                if (line.substr(0, 3) == "NAN")
                    gate_type_idx = NANDX1_Cell;
                else if (line.substr(0, 3) == "NOR")
                    gate_type_idx = NOR2X1_Cell;
                else if (line.substr(0, 3) == "INV")
                    gate_type_idx = INVX1_Cell;
            }
            //* Wire Element
            auto words_begin = sregex_iterator(line.begin() + firstWord.str().length(), line.end(), word_regex);
            if (declareMode) {
                for (sregex_iterator i = words_begin; i != sregex_iterator(); ++i) {
                    wire* wire_temp = new wire();
                    wire_temp->wire_name = (*i).str();
                    if(wire_type_idx==Input_Wire){
                        wire_temp->is_global_start = 1;
                        wire_temp->is_global_output = 0;
                        wire_temp->wire_delay = 0;
                        global_input_set.insert(wire_temp->wire_name);
                    }
                    else if(wire_type_idx==Output_Wire){
                        wire_temp->is_global_start = 0;
                        wire_temp->is_global_output = 1;
                        wire_temp->wire_delay = 0;
                        global_output_set.insert(wire_temp->wire_name);
                    }
                    else if(wire_type_idx==Inter_Wire){
                        wire_temp->is_global_start = 0;
                        wire_temp->is_global_output = 0;
                        wire_temp->wire_delay = 0.005;
                    }
                    wire_map.insert(std::pair<string, wire>(wire_temp->wire_name,*wire_temp));
                    wire_max_size++;
                }

            }
            else if (cellMode) {
                if (words_begin != sregex_iterator()) {
                    gate_temp = new gate();
                    gate_temp->gate_name = (*words_begin).str();
                    gate_temp->is_output_gate = 0;
                    //gate_temp->is_input_node = true;
                    
                    if(gate_type_idx==NANDX1_Cell){
                        gate_temp->gate_type = string("NANDX1");
                        gate_temp->visit_time = 2;
                    }
                    else if(gate_type_idx==NOR2X1_Cell){
                        gate_temp->gate_type = string("NOR2X1");
                        gate_temp->visit_time = 2;
                    }
                    else if(gate_type_idx==INVX1_Cell){
                        gate_temp->gate_type = string("INVX1");
                        gate_temp->visit_time = 1;
                    }
                    cellMode = false;
                    pinMode = true;
                } 
            }

            if (pinMode) {
                auto words_begin = sregex_iterator(line.begin(), line.end(), word_regex);
                for (sregex_iterator i = words_begin; i != sregex_iterator(); ++i) {
                    string pin = (*i).str();
                    string net = (*++i).str();
                    if (pin == "ZN") {
                        gate_temp->out_wire = net;
                        wire_map[net].in_gate = (gate_temp->gate_name);
                        if(global_output_set.find(net)!=global_output_set.end()){
                            gate_temp->is_output_gate = 1;
                            gate_temp->output_cap = 0.03;
                        }

           }
                    else if ((pin == "I")||(pin == "A1")) {
                        gate_temp->in_0 = net;
                        wire_map[net].out_map.insert(std::pair<string,string>(gate_temp->gate_name+"_1",pin));
                        if(global_input_set.find(net)!=global_input_set.end()){
                            gate_temp->visit_time--;
                        }
                        
                    }
                    else if(pin == "A2"){
                        gate_temp->in_1 = net;
                        wire_map[net].out_map.insert(std::pair<string,string>(gate_temp->gate_name+"_2",pin));
                        if(global_input_set.find(net)!=global_input_set.end()){
                            gate_temp->visit_time--;
                        }
                    }
                }
            }
        }
    }
    if (pinMode){
        gate_map.insert(std::pair<string, gate>(gate_temp->gate_name, *gate_temp));
        gate_max_size++;
        if(gate_temp->visit_time==0){
            global_input_gate.insert(gate_temp->gate_name);
            gate_queue.push(&(gate_map[gate_temp->gate_name]));
        }
        if(global_output_set.find(gate_temp->out_wire)!=global_output_set.end()){
            global_output_gate.insert(gate_temp->gate_name);
        }
        pinMode = false;
    }
}
Net::~Net(){}
bool sort_by_cap(gate gate_a, gate gate_b){
    
    if(gate_a.output_cap!=gate_b.output_cap){
        return gate_a.output_cap > gate_b.output_cap;
    }
    else{
        string gate_num1_s, gate_num2_s;
        int gate_num1, gate_num2;
        gate_num1_s.assign(gate_a.gate_name, 1, gate_a.gate_name.size()-1);
        gate_num2_s.assign(gate_b.gate_name, 1, gate_b.gate_name.size()-1);
        gate_num1 = stoi(gate_num1_s);
        gate_num2 = stoi(gate_num2_s);
        return gate_num1 < gate_num2;

    }
}
bool sort_by_prop(gate* gate_a_s, gate* gate_b_s){
    long double prop_a, prop_b;
    if(gate_a_s->longest_when_rise){
        prop_a = gate_a_s->prop_rise_delay;
    }
    else{
        prop_a = gate_a_s->prop_fall_delay;
    }
    if(gate_b_s->longest_when_rise){
        prop_b =gate_b_s->prop_rise_delay;
    }
    else{
        prop_b = gate_b_s->prop_fall_delay;
    }
    if(prop_a!=prop_b){
       return prop_a > prop_b;
    }
    else{
        string gate_num1_s, gate_num2_s;
        int gate_num1, gate_num2;
        gate_num1_s.assign(gate_a_s->gate_name, 1, gate_a_s->gate_name.size()-1);
        gate_num2_s.assign(gate_b_s->gate_name, 1, gate_b_s->gate_name.size()-1);
        gate_num1 = stoi(gate_num1_s);
        gate_num2 = stoi(gate_num2_s);
        return gate_num1 < gate_num2;
    }
}

bool sort_by_path(gate* gate_a, gate* gate_b){
    return gate_a->delay_sum >= gate_b->delay_sum;
}

void Net::Step1(Lib_Viewer& lib_data){
    map <string, gate>::iterator  iter;
    map <string, string>::iterator it_output_map;
    vector <gate> gate_cap_order;
    gate* temp_gate_0;
    gate temp_gate_1;
    wire temp_wire;
    long double cap_sum;

    for(iter=gate_map.begin();iter != gate_map.end(); iter++){
        temp_gate_0 = &((*iter).second);
        //cout<<"gate name: "<<temp_gate_0->gate_name<<endl;
        cap_sum = 0;
        if(global_output_gate.find(temp_gate_0->gate_name)!=global_output_gate.end()){
            //cout<<"This end of net"<<endl;
            cap_sum = 0.03;
        }

        if(wire_map.find(temp_gate_0->out_wire)!=wire_map.end()){
            temp_wire = wire_map[temp_gate_0->out_wire];
            for(it_output_map=temp_wire.out_map.begin();it_output_map!=temp_wire.out_map.end();it_output_map++){
                string gate_name;
                gate_name.assign((*it_output_map).first,0,(*it_output_map).first.size()-2);
                temp_gate_1 = gate_map[gate_name];
                cap_sum+=lib_data.lut_cap(temp_gate_1.gate_type,(*it_output_map).second);
                //cout<<"cap_sum: "<<cap_sum<<endl;
            }
        }
        temp_gate_0->output_cap = cap_sum;
        gate_cap_order.push_back(*temp_gate_0);
    }
    //cout<<module_name<<endl;
    sort(gate_cap_order.begin(),gate_cap_order.end(),sort_by_cap);
    string output1_name = "0812019_" +module_name+"_load.txt";
    ofstream file_out;
    file_out.open(output1_name);
    if(!file_out.is_open()){
        cerr<<output1_name<<" cannot be opened!"<<endl;
    }
    for(int i=0;i<gate_cap_order.size();i++){
        //file_out << fixed  << setprecision(6) << gate_cap_order[i].gate_name <<" "<<gate_cap_order[i].output_cap;
        file_out << fixed  << setprecision(6) << gate_cap_order[i].gate_name <<" "<< (round(gate_cap_order[i].output_cap*1000000.0)/1000000.0);
        if(i!=gate_cap_order.size()-1){
            file_out<<endl;
        }
    }
    file_out.close();
    gate_cap_order.clear();
}
void Net::Step2_Step3(Lib_Viewer& lib_data){
    //Get Depth of all gate

    //set <string> finish_wire;

    set <string>::iterator iter;
    map <string,string>::iterator iter_map;
    //map <string,string>::iterator iter_map_ini_add;
    map <string, gate>::iterator iter_gate;
    //set <string> ever_put_in;
    //set <string> finish_gate;
    //set <string> un_do_gate;
    //queue <string> wire_queue;
    vector <gate*> output_gate_v;

    //!
    //for(iter_gate=gate_map.begin();iter_gate!=gate_map.end();iter_gate++){
    //    un_do_gate.insert((*iter_gate).first);
    //}



    long double input_time;
    long double input_trans;
    long double wire_delay;
    long double cell_rise;
    long double cell_fall;
    long double rise_trans;
    long double fall_trans;



    while(!gate_queue.empty()){
        gate* curr_gate = gate_queue.front();
        /*
        if(finish_wire.find(curr_gate->gate_name)!=finish_wire.end()){
            gate_queue.pop();
            un_do_gate.erase(curr_gate->gate_name);
            continue;
        }

        cout<<"current: "<<curr_gate->gate_name<<endl;
        */
        if(curr_gate->gate_type=="INVX1"){
            input_time = wire_map[curr_gate->in_0].delay_sum;
            input_trans = 0;
            wire_delay = 0;
            if(global_input_set.find(curr_gate->in_0)==global_input_set.end()){
                input_trans = wire_map[curr_gate->in_0].trans_time;
                wire_delay = 0.005;

            }
            if(global_input_gate.find(curr_gate->gate_name)==global_input_gate.end()){
                gate* last_gate = &(gate_map[wire_map[curr_gate->in_0].in_gate]);
                for(int i=0;i<last_gate->longest_path.size();i++){
                    curr_gate->longest_path.push_back(last_gate->longest_path[i]);
                }
            }
            curr_gate->longest_path.push_back(curr_gate->in_0);
            cell_rise =  lib_data.lut_inp(curr_gate->gate_type, CELL_RISE, curr_gate->output_cap, input_trans);
            cell_fall =  lib_data.lut_inp(curr_gate->gate_type, CELL_FALL, curr_gate->output_cap, input_trans);
            rise_trans = lib_data.lut_inp(curr_gate->gate_type, RISE_TRANS, curr_gate->output_cap, input_trans);
            fall_trans = lib_data.lut_inp(curr_gate->gate_type, FALL_TRANS, curr_gate->output_cap, input_trans);

            curr_gate->prop_rise_delay = cell_rise;
            curr_gate->prop_fall_delay = cell_fall;

            if(cell_rise>=cell_fall){
                curr_gate->longest_when_rise = true;
                curr_gate->delay_sum = input_time + wire_delay + cell_rise;
                curr_gate->trans_time = rise_trans;
                wire_map[curr_gate->out_wire].delay_sum = curr_gate->delay_sum;
                wire_map[curr_gate->out_wire].trans_time = rise_trans;
            }
            else{
                curr_gate->longest_when_rise = false;
                curr_gate->delay_sum = input_time + wire_delay + cell_fall;
                curr_gate->trans_time = fall_trans;
                wire_map[curr_gate->out_wire].delay_sum = curr_gate->delay_sum;
                wire_map[curr_gate->out_wire].trans_time = fall_trans;
            }
            
            //finish_wire.insert(curr_gate->out_wire);
            //un_do_gate.erase(curr_gate->gate_name);
            /*
            if(wire_map[curr_gate->out_wire].out_map.size()==0){
                cout<<"gate: "<<curr_gate->gate_name<<endl;
                cout<<"warming"<<endl;
            }
            */
            
            
            for(iter_map = wire_map[curr_gate->out_wire].out_map.begin();
                iter_map!= wire_map[curr_gate->out_wire].out_map.end();iter_map++){
                string gate_name;
                gate_name.assign((*iter_map).first,0,(*iter_map).first.size()-2);
                gate_map[gate_name].visit_time-=1;
                if(gate_map[gate_name].visit_time<=0){
                    gate_queue.push(&(gate_map[gate_name]));
                }
            }


        }
        else if((curr_gate->gate_type=="NOR2X1" )||(curr_gate->gate_type=="NANDX1" )){
            long double input_time_0 = wire_map[curr_gate->in_0].delay_sum;
            long double input_time_1 = wire_map[curr_gate->in_1].delay_sum;
            //cout<<"gate_name: "<<curr_gate->gate_name<<endl;
            long double input_trans_0 =0, input_trans_1 = 0;
            long double wire_delay_0 = 0, wire_delay_1 = 0;
            if(global_input_set.find(curr_gate->in_0)==global_input_set.end()){
                input_trans_0 = wire_map[curr_gate->in_0].trans_time;
                //cout<<"input_trans_0: "<<input_trans_0<<endl;
                wire_delay_0 = 0.005;
            }
            if(global_input_set.find(curr_gate->in_1)==global_input_set.end()){
                input_trans_1 = wire_map[curr_gate->in_1].trans_time;
                //cout<<"input_trans_1: "<<input_trans_1<<endl;
                wire_delay_1 = 0.005;
            }
            
            long double delay_sum_0 = input_time_0 + wire_delay_0; //+ input_trans_0;
            long double delay_sum_1 = input_time_1 + wire_delay_1;// + input_trans_1;

            //cout<<"delay_sum_0: "<<delay_sum_0<<endl;
            //cout<<"delay_sum_1: "<<delay_sum_1<<endl;
            if(delay_sum_0>=delay_sum_1){
                input_time = input_time_0;
                input_trans = input_trans_0;
                wire_delay = wire_delay_0;

                if(global_input_gate.find(curr_gate->gate_name)==global_input_gate.end()){
                    gate* last_gate = &(gate_map[wire_map[curr_gate->in_0].in_gate]);
                    for(int i=0;i<last_gate->longest_path.size();i++){
                        curr_gate->longest_path.push_back(last_gate->longest_path[i]);
                    }
                }
                curr_gate->longest_path.push_back(curr_gate->in_0);
            }
            else{
                input_time = input_time_1;
                input_trans = input_trans_1;
                wire_delay = wire_delay_1;       

                if(global_input_gate.find(curr_gate->gate_name)==global_input_gate.end()){
                    gate* last_gate = &(gate_map[wire_map[curr_gate->in_1].in_gate]);
                    //cout<<"test long path: "<<last_gate->longest_path.size()<<endl;
                    for(int i=0;i<last_gate->longest_path.size();i++){
                        curr_gate->longest_path.push_back(last_gate->longest_path[i]);
                    }
                }
                curr_gate->longest_path.push_back(curr_gate->in_1);
            }


            long double cell_rise =  lib_data.lut_inp(curr_gate->gate_type, CELL_RISE, curr_gate->output_cap, input_trans);
            long double cell_fall =  lib_data.lut_inp(curr_gate->gate_type, CELL_FALL, curr_gate->output_cap, input_trans);
            long double rise_trans = lib_data.lut_inp(curr_gate->gate_type, RISE_TRANS, curr_gate->output_cap, input_trans);
            long double fall_trans = lib_data.lut_inp(curr_gate->gate_type, FALL_TRANS, curr_gate->output_cap, input_trans);
            
            curr_gate->prop_rise_delay = cell_rise;
            curr_gate->prop_fall_delay = cell_fall;
            //cout<<"intrans: "<<input_trans<<endl;
            //cout<<"cap: "<<curr_gate->output_cap<<endl;
            //cout<<"prop_rise_delay: "<<curr_gate->prop_rise_delay<<endl;
            //cout<<"prop_fall_delay: "<<curr_gate->prop_fall_delay<<endl;
            if(cell_rise>=cell_fall){
                curr_gate->longest_when_rise = true;
                curr_gate->delay_sum = input_time + wire_delay + cell_rise;
                curr_gate->trans_time = rise_trans;
                wire_map[curr_gate->out_wire].delay_sum = curr_gate->delay_sum;
                wire_map[curr_gate->out_wire].trans_time = rise_trans;
            }
            else{
                curr_gate->longest_when_rise = false;
                curr_gate->delay_sum = input_time + wire_delay + cell_fall;
                curr_gate->trans_time = fall_trans;
                wire_map[curr_gate->out_wire].delay_sum = curr_gate->delay_sum;
                wire_map[curr_gate->out_wire].trans_time = fall_trans;
            }
            
            //finish_wire.insert(curr_gate->out_wire);
            //un_do_gate.erase(curr_gate->gate_name);
            /*
            if(wire_map[curr_gate->out_wire].out_map.size()==0){
                cout<<"gate: "<<curr_gate->gate_name<<endl;
                cout<<"warming"<<endl;
            }
            cout<<"N24: "<<wire_map["N24"].out_map.size()<<endl;
            */
            for(iter_map = wire_map[curr_gate->out_wire].out_map.begin();
                iter_map!= wire_map[curr_gate->out_wire].out_map.end();iter_map++){
                string gate_name;
                gate_name.assign((*iter_map).first,0,(*iter_map).first.size()-2);
                gate_map[gate_name].visit_time-=1;
                if(gate_map[gate_name].visit_time<=0){
                    gate_queue.push(&(gate_map[gate_name]));
                }
            }
        }
        output_gate_v.push_back(curr_gate);
        gate_queue.pop();
        /*
        if(gate_queue.empty()&& un_do_gate.size()>0){
            
            for(iter=un_do_gate.begin();iter!=un_do_gate.end();iter++){
                if(gate_map[*iter].visit_time<=0){
                    gate_queue.push(&(gate_map[(*iter)]));
                }
            }
        }
        */
    }
    //cout<<un_do_gate.size()<<endl;
    //cout<<int(output_gate_v.size())<<endl;
    sort(output_gate_v.begin(),output_gate_v.end(), sort_by_prop);
    string output1_name = "0812019_" +module_name+"_delay.txt";
    ofstream file_out;
    file_out.open(output1_name);
    if(!file_out.is_open()){
        cerr<<output1_name<<" cannot be opened!"<<endl;
    }
    //cout<<int(output_gate_v.size())<<endl;
    for(int i=0;i<output_gate_v.size();i++){
        if(output_gate_v[i]->longest_when_rise){
            //file_out << fixed  << setprecision(6) << output_gate_v[i]->gate_name<<" 1 " << output_gate_v[i]->prop_rise_delay << " " << output_gate_v[i]->trans_time;
            file_out  << fixed  << setprecision(6) << output_gate_v[i]->gate_name<<" 1 " << (round(output_gate_v[i]->prop_rise_delay*1000000.0)/1000000.0)  << " " <<  (round(output_gate_v[i]->trans_time*1000000.0)/1000000.0);
    
        }
        else{

            //file_out << fixed  << setprecision(6) << output_gate_v[i]->gate_name<<" 0 " << output_gate_v[i]->prop_fall_delay << " " << output_gate_v[i]->trans_time;
            file_out  << fixed  << setprecision(6) << output_gate_v[i]->gate_name<<" 0 " << (round(output_gate_v[i]->prop_fall_delay*1000000.0)/1000000.0)  << " " <<  (round(output_gate_v[i]->trans_time*1000000.0)/1000000.0);
    
        }
        if(i!=output_gate_v.size()-1){
            file_out<<endl;
        }
    }
    file_out.close();
    set <string>::iterator iter_set_s;
    gate* output_gate_t;
    //cout<<"output gate"<<endl;
    vector <gate*> global_output_gate_v;
    for(iter_set_s=global_output_gate.begin();iter_set_s!=global_output_gate.end();iter_set_s++){
        global_output_gate_v.push_back(&gate_map[*(iter_set_s)]);
    }



    sort(global_output_gate_v.begin(),global_output_gate_v.end(),sort_by_path);
    //cout<<"--------------------------------------------"<<endl;
    string output_path_name = "0812019_" +module_name+"_path.txt";
    file_out.open(output_path_name);
    if(!file_out.is_open()){
        cerr<<output_path_name<<" cannot be opened!"<<endl;
    }
    //cout<<"global_output_gate_v[0]->delay_sum: "<< global_output_gate_v[0]->delay_sum<<endl;
    //file_out << fixed  << setprecision(6) << "Longest delay = "<< global_output_gate_v[0]->delay_sum <<", the path is: ";
    file_out << fixed  << setprecision(6) << "Longest delay = "<< (round(global_output_gate_v[0]->delay_sum*1000000.0)/1000000.0) <<", the path is: ";
    
    
    for(int i = 0;i<global_output_gate_v[0]->longest_path.size();i++){
            file_out << global_output_gate_v[0]->longest_path[i];
            if(i<global_output_gate_v[0]->longest_path.size()-1){
                file_out << " -> ";
            }
            else{
                file_out << " -> "<< global_output_gate_v[0]->out_wire << endl;
            }
    }

    //file_out<< fixed  << setprecision(6) << "Shortest delay = "<< global_output_gate_v[global_output_gate_v.size()-1]->delay_sum <<", the path is: ";
    file_out<< fixed  << setprecision(6) << "Shortest delay = "<< (round(global_output_gate_v[global_output_gate_v.size()-1]->delay_sum*1000000.0)/1000000.0) <<", the path is: ";
    for(int i = 0;i<global_output_gate_v[global_output_gate_v.size()-1]->longest_path.size();i++){
            file_out << global_output_gate_v[global_output_gate_v.size()-1]->longest_path[i];
            if(i<global_output_gate_v[global_output_gate_v.size()-1]->longest_path.size()-1){
                file_out << " -> ";
            }
            else{
                file_out << " -> "<< global_output_gate_v[global_output_gate_v.size()-1]->out_wire << endl;
            }
    }
    file_out.close();
  
}



/*
    1. Read design
    2. travel every node and get [output load]:    Front->End
    3. travel every node to calculate delay
        * For each
            - Table input
                - input trans: From Procedding output trans
                - output load
            - do interpol 
            - To current Node total delay = Wire delay + Current Node Delay(cell rise/cell fall) + (Output delay?)   

        * Sort ans
    4. Shortest Path / Longest Path 
        - travel all possible path
            - Store current longeset
            - Store current shorteset
*/



int main(int argc, char *argv[]){
    //double start_clock, end_clock_1, end_clock_2;
    //start_clock = clock();
    string argv_flag = argv[2];
    if((argc!=4)||(argv_flag.find("-l")==-1)){
        cerr << "Wrong format of arguments. -> ./xxxxx {design}.v -l {test_lib}.lib" << endl;
        return 1;
    }

    
    ifstream input_v(argv[1]);
    ifstream input_lib(argv[3]);
    //int table_idx, long double x_value, long double y_value

    Lib_Viewer lib_data(input_lib);
    Net net(input_v);
    net.Step1(lib_data);
    //end_clock_1 = clock();
    net.Step2_Step3(lib_data);
    //cout<<lib_data.lut_inp(string("NANDX1"),CELL_RISE,0.040501,0.055050)<<endl;
    //end_clock_2 = clock();
    //cout<<"Step 1 Time cost: "<<(end_clock_1 - start_clock)/CLOCKS_PER_SEC <<" sec" << endl;
    //cout<<"Step 2,3 Time cost: "<<(end_clock_2 - end_clock_1)/CLOCKS_PER_SEC <<" sec" << endl;
    //cout<<"Total Time cost: "<<(end_clock_2 - start_clock)/CLOCKS_PER_SEC <<" sec" << endl;



    return 0;
}