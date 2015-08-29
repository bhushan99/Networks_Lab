#include <iostream>
#include <igraph.h>
#include <fstream>
#include <vector>
#include <cstdio>
#include <set>
#include <algorithm>

using namespace std;


void write_in_file(vector<int > v) 
{
	int l=v.size();
	ofstream myfile;
	myfile.open("degree_distribution.txt"); 
	for(int i=0;i<l;i++)
		{ 
			myfile<<i<<' '<<v[i]<<endl;
	    } 
	myfile.close(); 
}

void write_in_file1(vector<int > v) 
{
	int l=v.size();
	ofstream myfile;
	myfile.open("degree_distribution1.txt"); 
	for(int i=0;i<l;i++)
		{ 
			myfile<<i<<' '<<v[i]<<endl;
	    } 
	myfile.close(); 
}

int main()
{
	int N,K;
	cout<<"ENTER NUMBER OF NODES"<<endl;
    cin>>N;
    cout<<"ENTER THE VALUE OF K"<<endl;
    cin>>K;
	igraph_t graph;
	igraph_integer_t dia;
    igraph_empty(&graph, N, IGRAPH_UNDIRECTED);
    igraph_vector_t edges;
    igraph_vector_init(&edges, 2*N*K);
    int c=0,i,j;
    bool b;
    vector<int> ran;
    for(i=1;i<N;i++)
    {
    	ran.push_back(i);
    }
    for(i=0;i<N;i++)
    {
      vector<int> random=ran;
      if(i)
      {
      	random[i-1]=0;
      }
      for(j=0;j<K;j++)
      {
      	VECTOR(edges)[c++]=i;
        int x=rand()%(N-j-1);
        VECTOR(edges)[c++]=random[x];
        swap(random[x],random[N-j-2]);        
      }
    }
    igraph_add_edges(&graph, &edges, 0);
    igraph_simplify(&graph,1,1,0);
    igraph_real_t avg_path;
    igraph_average_path_length(&graph, &avg_path, IGRAPH_UNDIRECTED, 1);
    printf("Average path length : %f\n", (double) avg_path);
    igraph_diameter(&graph, &dia, 0, 0, 0, IGRAPH_UNDIRECTED, 1);
    int d=dia;
    cout<<"DIAMETER = "<<d<<endl;
    
    igraph_vector_t res;
    igraph_vs_t vids=igraph_vss_all();
    igraph_vector_init(&res , 0);
   
    igraph_degree(&graph, &res, vids, IGRAPH_ALL, 0);
    std::vector<int> deg_dist(N-1,0);
    for(int i=0;i<N;i++){
    	deg_dist[VECTOR(res)[i]]++;
    }
    /*for(int i=0;i<N-1;i++){
    	cout<<deg_dist[i]<<' ';
    }cout<<endl;
    */
    igraph_bool_t con;
    igraph_is_connected(&graph,&con,IGRAPH_WEAK);
    b=con;
    cout<<b<<endl;
  
    igraph_integer_t Z=igraph_ecount(&graph);
    cout<<"Number of edges in graph"<<" "<<(int)Z<<endl;
    //igraph_vector_print(&res);
    write_in_file(deg_dist);


    /****************deleting S nodes **************/

    int S;
    cout<<"ENTER THE VALUE OF S,NUMBER OF NODES TO BE DELTED"<<endl;
    cin>>S;
    igraph_vector_t  del;
    igraph_vector_init(&del,S);
    vector<int> v;
    for(i=0;i<N;i++)
    {
      v.push_back(i);
    }
    for(i=0;i<S;i++)
    {
    	int X=rand()%(N-i);
    	VECTOR(del)[i]=v[X];
    	swap(v[X],v[N-i-1]);
    }
    igraph_vs_t delet=igraph_vss_vector(&del);
    igraph_delete_vertices(&graph,delet);
    igraph_average_path_length(&graph, &avg_path, IGRAPH_UNDIRECTED, 1);
    printf("Average path length after deleting S nodes: %f\n", (double) avg_path);
    igraph_diameter(&graph, &dia, 0, 0, 0, IGRAPH_UNDIRECTED, 1);
    d=dia;
    cout<<"DIAMETER = "<<d<<endl;

    igraph_vector_t res1;
    igraph_vs_t vids1=igraph_vss_all();
    igraph_vector_init(&res1 , 0);
   
    igraph_degree(&graph, &res1, vids1, IGRAPH_ALL, 0);
    std::vector<int> deg_dist1(N-S-1,0);
    for(int i=0;i<N-S;i++){
    	deg_dist1[VECTOR(res1)[i]]++;
    }
    igraph_is_connected(&graph,&con,IGRAPH_WEAK);
  	b=con;
  	cout<<b<<endl;
    int cont=0;
    /*for(int i=0;i<N-S-1;i++){
    	
    }cout<<endl;*/
    //igraph_vector_print(&res1);
    
    igraph_integer_t Y=igraph_ecount(&graph);
    cout<<"Number of edges in graph after compromising nodes"<<" "<<(int)Y<<endl;
    int y=Y;
    int z=Z;
    printf("Fraction of edges disconnected %.10lf \n",1-((y+0.0)/z));
    cout<<"Number of nodes becoming isolated "<<deg_dist1[0]<<endl;


    write_in_file1(deg_dist1);
  
    igraph_vector_destroy(&edges);
    igraph_destroy(&graph);


    return 0;
}
