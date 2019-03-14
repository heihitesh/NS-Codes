#include<iostream>
#include<bits/stdc++.h>

using namespace std;

vector<vector<int> > matrix; //2D Vector

void initialiseMatrix(int node){
    for(int i=0; i<=node;i++)
    {
        vector<int> temp;
        for(int j=0; j<=node; j++)
        {
            if(j==i){
                //Do Nothing
                temp.push_back(0);
            }else{
                int n = rand() % 2; //will get either 0 or 1
                temp.push_back(n);
            }
        }
        //Now Push the row in the Vector
        matrix.push_back(temp);
    }
}

void viewMatrix(vector<vector<int> > m , bool flag){
    float total_path = 0;
    map<int,int> mymap; // to store the result hop count of each node
    for(int i=0; i<m.size();i++) // matrix size = no of rows (subjects)
    {
        float node_path = 0;
        for(int j=0;j<m[i].size();j++)
        {
            int val = m[i][j];
            cout<<val<<" ";
            node_path += val;
            ++mymap[val];
        }
        total_path += node_path/(m.size()-1);
        if(flag) cout <<"\t"<<"Avg : "<< node_path/(m.size()-1);
        cout << endl;
    }

    if(flag) {
        cout << "Total Avg Path Length of All Nodes : " << total_path / m[0].size() << endl;
        for (auto x:mymap) //for(pair<int,int> x: mymap)
            cout << x.first << " : " << x.second << endl;

    }
}



// Solves the all-pairs shortest path problem using Floyd Warshall algorithm
void floyd_Warshall (vector<vector<int> > matrix)
{
    int  i, j, k;
    vector<vector<int> > dist(matrix); //create a copy of matrix
//    for (i = 0; i < matrix[0].size(); i++)
//        for (j = 0; j < matrix[0].size(); j++)
//            dist[i][j] = matrix[i][j]; // copy the matrix


    for (k = 0; k < matrix.size(); k++)
    {
        for (i = 0; i < matrix.size(); i++)
        {
            for (j = 0; j < matrix.size(); j++)
            {
                if(i==j) continue;
                if(dist[i][j] == 0) // 0 means there is no path btw them Therefor add a path
                    dist[i][j] = dist[i][k] + dist[k][j];
                else if(dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    // Print the shortest distance matrix
    viewMatrix(dist,true);
}



void insertWormHole(){
    vector<vector<int> > wm(matrix);
    int row, col;
    cout<<"\nRows(Node) (Source) :";
    cin>>row;

    for(int i=0; i<matrix.size();i++){
        cout <<"\t"<<matrix[row][i]<<" ";
    }
    cout<<"\nColumn(Node) (Dest) :";
    cin>>col;
    if(row != col){
        wm[row][col] = 1;
    }

    viewMatrix(wm,false);
    cout<<"\n\n<----------------------------------------------->\n\n";
    floyd_Warshall(wm);

}





int main()
{
    int count;
    cout<<"Enter the No of Nodes : ";
    cin>>count;
    initialiseMatrix(count);
    viewMatrix(matrix,false);

    cout<<"\n\n<----------------------------------------------->\n\n";
    floyd_Warshall(matrix);
    insertWormHole();


    return 0;
}
