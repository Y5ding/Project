#include "Graph.h"
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <utility>  // load pair class
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <vector>
#include <algorithm>
#include<float.h> 
#include <tuple>
using namespace std;

unordered_map<string,bool> isValid;
Graph::Graph(const char* const & edgelist_csv_fn) {
    E=0;
    V=0;
    // TODO
    //parse edgelist_csv_fn
    //adj_list O(V+E)
     ifstream my_file(edgelist_csv_fn);     
     string line;                  
  while(getline(my_file, line)) {  // read one line from the file
    istringstream ss(line);      // create istringstream of current line
    string first, second, third; // helper vars
    getline(ss, first, ',');     // store first column in "first"
    getline(ss, second, ',');    // store second column in "second"
    getline(ss, third, '\n');    // store third column column in "third"
    double weight = stod(third);
    edgelist.push_back(make_tuple(first,second,weight));
    pair<string, double> pair1=make_pair(first,weight);
    pair<string, double> pair2=make_pair(second,weight);
    if(adj.count(first)==0){
        adj.insert(pair<string,vector<pair<string, double>>>(first, vector<pair<string, double>>()));
        V++;
        node.push_back(first);
    }
    if(adj.count(second)==0){
        adj.insert(pair<string,vector<pair<string, double>>>(second, vector<pair<string, double>>()));
        node.push_back(second);
        V++;
    }
    adj[first].push_back(pair2);
    adj[second].push_back(pair1);
    E++;
}
my_file.close();                 // close file when done
    //set represent node or vector<string>
}

unsigned int Graph::num_nodes() {
    // TODO
    // set size 
    return V;
}

vector<string> Graph::nodes() {
    // TODO
    vector<string> nodeRever=node;
    reverse(nodeRever.begin(),nodeRever.end());
    return nodeRever;
    
}

unsigned int Graph::num_edges() {
    // TODO
    return E;
}

unsigned int Graph::num_neighbors(string const & node_label) {
    // TODO
    unsigned int size;
    if(adj.count(node_label)>0){
        size=adj[node_label].size();
    }
    return size;
}

double Graph::edge_weight(string const & u_label, string const & v_label) {
    // TODO
    double weight= -1.0;
    if(adj.count(u_label)>0){
        vector<pair<string, double>> temp=adj[u_label];
        for(auto i:temp){
            if(i.first==v_label){
                weight=i.second;
            }
        }
    }
    return weight;
}

vector<string> Graph::neighbors(string const & node_label) {
    // TODO
    vector<string> answer;
    if(adj.count(node_label)>0){
        vector<pair<string, double>> temp=adj[node_label];
        for(auto j:temp){
            answer.push_back(j.first);
            }
         return answer;
        }
     return answer;
}
//./GraphTest small.csv graph_properties
vector<string> Graph::shortest_path_unweighted(string const & start_label, string const & end_label) {
    // TODO
    queue<string> stack;
    vector<string> ans;
    unordered_map<string,string> parents;
    for(auto n:node){parents[n]="-1";}
    if(start_label==end_label){
        ans.push_back(start_label);
        return ans;
    }
    stack.push(start_label);
    while(!stack.empty()){
        string key=stack.front();
        stack.pop();
        if(key==end_label){break;}
        vector<string> temp=neighbors(key);
        for(auto p:temp){
            //bool valid
            if(parents[p]=="-1"){
                stack.push(p);
                parents[p]=key;
                }
            }
    } 

    string parent=parents[end_label];
    if(parent=="-1"){return ans;}
    ans.push_back(end_label);
    while(parent!=start_label ){
        ans.push_back(parent);
        parent=parents[parent];
    }
    ans.push_back(start_label);
     vector<string> ansRever=ans;
     reverse(ansRever.begin(),ansRever.end());
     return ansRever;
}

vector<tuple<string,string,double>> Graph::shortest_path_weighted(string const & start_label, string const & end_label) {
    // ./GraphTest example/small.csv shortest_weighted
    priority_queue<pair<string, double>,vector<pair<string, double>>,PQComp> stack;
    vector<tuple<string,string,double>> ans;
    unordered_map<string,string> parents;
    unordered_map<string,double> distant;
    for(auto l:node){isValid[l]=false;}
    for(auto n:node){parents[n]="-1";}
    for(auto m:node){distant[m]=DBL_MAX;}
    if(start_label==end_label){
        ans.push_back(make_tuple(start_label,end_label,-1.0));
        return ans;
    }
    distant[start_label]=0.0;
    stack.push(make_pair(start_label,0.0));
    while(!stack.empty()){
        pair<string, double> key=stack.top();
        stack.pop();
        if(isValid[key.first]==true){break;}
        else{
        isValid[key.first]=true;
        vector<pair<string, double>> temp=adj[key.first];
        for(auto p:temp){
            //bool valid
            if(distant[p.first]>distant[key.first]+p.second){
                distant[p.first]=distant[key.first]+p.second;
                parents[p.first]=key.first;
                stack.push(p);
                }
            else if(distant[p.first]==distant[key.first]+p.second){
                double edgenum=edge_weight(p.first,parents[p.first]);
                if(edgenum>p.second){
                    parents[p.first]=key.first;
                    stack.push(p);
                } 
            }
            }
    } 
    }

    string parent=parents[end_label];
    if(parent=="-1"){return ans;}
    ans.push_back(make_tuple(parent,end_label,edge_weight(parent,end_label)));
    while(parent!=start_label){
        ans.push_back(make_tuple(parents[parent],parent,edge_weight(parents[parent],parent)));
        parent=parents[parent];
    }
     vector<tuple<string,string,double>> ansRever=ans;
     reverse(ansRever.begin(),ansRever.end());
     return ansRever;
}
vector<vector<string>> result;
vector<string> path;
//unsigned int dfsCount;
void Graph::dfs(string start, double const & threshold){
    isValid[start]=true;
    //dfsCount++;
    path.push_back(start);
    //if(dfsCount==V){return;}
     vector<pair<string, double>> info=adj[start];
     for(auto q:info){
         if(q.second<=threshold && isValid[q.first]==false){
             dfs(q.first,threshold);
         }
     }
}
//./GraphTest example/small.csv connected_components
vector<vector<string>> Graph::connected_components(double const & threshold) {
    // TODO dfs search
    for(auto n:node){isValid[n]=false;}
    for(auto j:node){
       if(isValid[j]==true){continue;}
       dfs(j,threshold);
       result.push_back(path);
       path.clear();
    }
    return result;
}

string Graph::find(unordered_map<string,string>& par, string const & i)
{
    if (par[i] == "-1"){return i;}
    return find(par, par[i]);
}
  
// Naive implementation of union()
void Graph::Union(unordered_map<string,string>& par, string const & x, string const & y)
{
    string xset = find(par, x);
    string yset = find(par, y);
    if(xset!=yset){par[xset] = yset;}
}
bool sortByweight(tuple<string,string,double> & tub1, tuple<string,string,double> & tub2){
        return get<2>(tub1) < get<2>(tub2);
    }
double Graph::smallest_connecting_threshold(string const & start_label, string const & end_label) {
    // ./GraphTest example/small.csv smallest_threshold
    if(start_label==end_label){return 0.0;}
     unordered_map<string,string> parents;
     for(auto n:node){parents[n]="-1";}
     sort(edgelist.begin(), edgelist.end(), sortByweight);
     for(auto edge:edgelist){
         //cout<<get<0>(edge)<<" "<<get<1>(edge)<<" "<<get<2>(edge)<<endl;
         Union(parents, get<0>(edge),get<1>(edge));
         if(find(parents, start_label)==find(parents, end_label)){return get<2>(edge);}
     }
     return -1.0;   
}
bool PQComp::operator()(pair<string, double>& a, pair<string, double>& b) const{
    return a.second>b.second;
}
