#include <bits/stdc++.h>

using namespace std;

typedef unsigned char byte;

int main(int argc, char * argv[]) {
	if(argc != 2) {
		cout << "Usage: " << argv[0] << " <arquivo_entrada>\n"; 
		return 0;
	}

	string filename = argv[1];

	ifstream file(filename);
	if(!file.is_open()) {
		cerr << "Could not open file " << filename << '\n';
		return 0;
	}

	// le os simbolos e marca a frequencia deles
	map<char, int> freq;
	int c;
	while((c = file.get()) != EOF) {
		freq[c]++;
	}

	// Criacao da arvore de huffman
	int n = freq.size(); // quantos 

	if(n == 1) {
		file.close();
		cout << "O arquivo precisa ter pelo menos dois simbolos diferentes" << endl;
		return 0;
	}

	// a arvore sempre vai ter n - 1 nos internos, e n folhas
	vector<int> pai(n + n - 1, -1), code(n + n - 1);

	map<char, int> mp; // mapeia simbolo pra nó da arvore

	int cur = 0;
	priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

	for(auto & [c, x] : freq) {
		mp[c] = cur;
		pq.emplace(x, cur);
		cout << x << endl;
		cur++;
	}


	while(pq.size() > 1) {
		int x, y, u, v;
		tie(x, u) = pq.top(); pq.pop(); // u eh filho da aresta 1
		tie(y, v) = pq.top(); pq.pop(); // v eh filho da aresta 0

		pai[u] = cur;
		code[u] = 1;
		pai[v] = cur;
		code[v] = 0;
		pq.emplace(x + y, cur);

		cur++;
	}

	// acha os codigos de cada simbolo
	auto find_symbol = [&](char c) {
		string ret;
		for(int u = mp[c]; pai[u] != -1; u = pai[u]) {
			ret += '0' + code[u]; 
		}
		reverse(ret.begin(), ret.end());
		return ret;
	};

	// imprime codigos resultantes

	for(auto & [c, x] : freq) {
		cout << c << ": " << find_symbol(c) << endl;
	}

	// imprime pra <filename>.out

	file.clear(); // fala que pode voltar a ler
	file.seekg(0, ios::beg); // volta pro inicio do arquivo

	ofstream out(filename + ".out", ios::out | ios::binary);

	if(!out.is_open()) {
		cout << "Could not open write file" << endl;
		return 0;
	}

	string str = "";
	while((c = file.get()) != EOF) {
		str += find_symbol(c);
	}

	// write to output
	int cnt = 0;
	byte aux = 0;
	for(char c : str) {
		if(cnt == 8 * sizeof(aux)) {
			out << aux;
			aux = 0;
			cnt = 0;
		}
		c -= '0';
		aux <<= 1;
		aux |= c;
		cnt++;
	}
	aux <<= 8 - cnt;
	out << aux;
	out.close();

	cout << "Terminou!" << endl;
}