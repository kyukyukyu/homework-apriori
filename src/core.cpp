#include "core.hpp"

using namespace std;
using namespace apriori;

Apriori::Apriori(double minSup) : minSup(minSup), transactions(), rules() {}

Apriori::~Apriori() {
}

void Apriori::add(Itemset* itemset) {
}

const std::vector<AssocRule*>& Apriori::mineRules() {
    return this->rules;
}
