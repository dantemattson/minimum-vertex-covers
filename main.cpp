#include <iostream>
#include <chrono>
#include <fstream>
#include <set>
#include<map>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

#define EDGES set<pair<int,int>>

// Graph
class Graph {
public:
	set<int> *V;
	EDGES *E;
	map<int, int> *degrees;

	Graph(int numVerts) {
		V = new set<int>;
		E = new EDGES;
		degrees = new map<int, int>;
	}
	~Graph() {
		delete V;
		delete E;
		delete degrees;
	}
};

// Greedy maximal matching on the input graph in O(|E|)
EDGES *maximalMatching(Graph *G, int **inv) {

	EDGES *m = new EDGES;
	set<int> mmVerts;

	for (EDGES::iterator it = G->E->begin(); it != G->E->end(); it++) {
		// in inverted adj matrix and doesn't connect
		if (inv[it->first][it->second] && (mmVerts.find(it->first) == mmVerts.end()) && (mmVerts.find(it->second) == mmVerts.end())) {
			m->insert(*it);
			mmVerts.insert(it->first);
			mmVerts.insert(it->second);
		}
	}

	return m;

}

// Use the vertices that make up a maximal matching on the graph
set<int> *approxCover(Graph *G, int** inv) {

	set<int> *vertexCover = new set<int>;
	EDGES *mm = maximalMatching(G, inv);

	for (EDGES::iterator it = mm->begin(); it != mm->end(); it++) {
		vertexCover->insert(it->first);
		vertexCover->insert(it->second);
	}

	cout << "a MVC: " << vertexCover->size() << "\n";

	return vertexCover;
}

// Create and return the graph
Graph *createGraph(string filename, string target) {
	
	ifstream graph_file;
	graph_file.open(filename);
	
	string line;
	stringstream lineReader;
	string trash;
	int a = 0, b = 0;
	int n = 0;
	
	// Read in until p
	while (getline(graph_file, line)) {

		if (line[0] == 'p') {
			lineReader = stringstream(line);
			lineReader >> trash;
			lineReader >> trash;
			lineReader >> a;
			lineReader >> b;
			cout << "Loading graph of size " << a << "\n";
			n = a;
			break;
		}

	}

	Graph *G = new Graph(n);

	map<int, int> tempDegs;
	int **inv; // inverted adjacency matrix
	inv = new int*[n + 1];
	for (int i = 0; i < n + 1; i++) {
		inv[i] = new int[n + 1];
		for (int j = 0; j < n + 1; j++) {
			inv[i][j] = 1;
		}
	}

	for (int i = 0; i < n; i++) {
		tempDegs[i] = n-1;
		for (int j = 0; j < n; j++) {

			G->E->insert(make_pair(i, j));
			G->V->insert(i);
			G->V->insert(j);

		}
	}

	while (getline(graph_file, line)) {

		if (line[0] == 'e') {
			lineReader = stringstream(line);
			lineReader >> trash;
			lineReader >> a;
			lineReader >> b;

			inv[a][b] = 0;
			inv[b][a] = 0;

			G->E->erase(make_pair(a, b));
			G->E->erase(make_pair(b, a));
			G->V->erase(a);
			G->V->erase(b);
			
			tempDegs[a] -= 1;
			tempDegs[b] -= 1;

		}

	}

	graph_file.close();
	cout << "Done loading\n";

	for (auto i : tempDegs) {
		G->degrees->insert(make_pair(i.first, i.second));
	}
	set<int> *vc = approxCover(G, inv);
	
	// Convert target from command line to int
	stringstream temp(target);
	int t = 0;
	temp >> t;
	
	if (vc->size() <= t ) {
		cout << "Found cover<=" << t << ": " << vc->size() << "\n";
	} else {
		cout << "Could not find cover<=" << t << "\nCover of size " << vc->size() << " was found.\n";
	}

	delete vc;
	
	for (int i = 0; i < n; i++) {
		delete[] inv[i];
	}
	
	return G;
}

int main(int argc, const char** argv) 
{
	if (argc < 3) {
		cout << "usage: q5sub target_vertex_cover input_file_path\n";
		return -1;
	}
	
	auto start = chrono::high_resolution_clock::now();
	
	Graph *G = createGraph(argv[2], argv[1]);
	
	cout << "Program total: " << chrono::duration_cast<chrono::seconds>(high_resolution_clock::now() - start).count() << "s\n";
	
	delete G;

	return 0;
}
