#include "core.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace apriori;

Itemset* parseLine(string& line);
void writeTo(const vector<AssocRule*>& rules, ostream& os);

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: apriori.exe <min_sup> <input_file> <output_file>"
             << endl;
        return 1;
    }

    double minSup = atof(argv[1]);
    string inputFile = argv[2];
    string outputFile = argv[3];

    ifstream fin(inputFile);
    if (!fin.is_open()) {
        cerr << "Failed to open input file" << endl;
        return 1;
    }

    Apriori* apr = new Apriori(minSup);

    while (!fin.eof()) {
        string line;
        Itemset* is;

        getline(fin, line);
        is = parseLine(line);
        apr->add(is);
    }

    fin.close();

    const vector<AssocRule*>& rules = apr->mineRules();
    ofstream fout(outputFile);

    if (!fout.is_open()) {
        cerr << "Failed to open output file" << endl;
    } else {
        writeTo(rules, fout);
    }

    fout.close();

    delete apr;

    return 0;
}

Itemset* parseLine(string& line) {
}

void writeTo(const vector<AssocRule*>& rules, ostream& os) {
}
