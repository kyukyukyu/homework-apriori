#include "core.hpp"

#include <vector>

using namespace std;
using namespace apriori;

AssocRule::~AssocRule() {
    delete this->lhs;
    delete this->rhs;
}

Apriori::Apriori(double minSup) : minSup(minSup), transactions(), rules() {}

Apriori::~Apriori() {
    vector<Itemset*>::iterator it1;
    for (it1 = this->transactions.begin();
         it1 != this->transactions.end();
         ++it1) {
        Itemset* transaction = *it1;
        delete transaction;
    }

    vector<AssocRule*>::iterator it2;
    for (it2 = this->rules.begin();
         it2 != this->rules.end();
         ++it2) {
        AssocRule* rule = *it2;
        delete rule;
    }
}

void Apriori::add(Itemset* itemset) {
}

const std::vector<AssocRule*>& Apriori::mineRules() {
    return this->rules;
}
