#include "core.hpp"

#include <vector>

using namespace std;
using namespace apriori;

AssocRule::~AssocRule() {
    delete this->lhs;
    delete this->rhs;
}

Apriori::Apriori(double minSup)
    : minSup(minSup), transactions(), lList(), rules() {}

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
    this->transactions.push_back(itemset);
}

const std::vector<AssocRule*>& Apriori::mineRules() {
    vector<Table*>::const_iterator itTable;
    this->mineFreqPatts();
    for (itTable = this->lList.begin();
         itTable != this->lList.end();
         ++itTable) {
        if (itTable == this->lList.begin()) {
            continue;
        }

        Apriori::Table* table = *itTable;
        Apriori::Table::const_iterator itRow;
        for (itRow = table->begin();
             itRow != table->end();
             ++itRow) {
            Apriori::TableRow* row = *itRow;
            this->mineRulesFrom(*row);
        }
    }
    return this->rules;
}

void Apriori::mineFreqPatts() {
    Apriori::Table c = Apriori::Table();
    Apriori::Table* l = new Apriori::Table();
    this->buildC(c);

    // initialize the list of L-table
    this->lList.clear();
    this->lList.push_back(nullptr);

    // filter C1, build L1, and save it
    this->filterTable(c, *l);
    this->lList.push_back(l);

    for (int k = 1; !l->empty(); ++k) {
        this->buildC(c, *l);

        vector<Itemset*>::const_iterator it;
        for (it = this->transactions.begin();
             it != this->transactions.end();
             ++it) {
            Itemset* transaction = *it;
            this->incrementSup(c, *transaction);
        }

        l = new Apriori::Table();
        this->filterTable(c, *l);
    }
}

void Apriori::buildC(Apriori::Table& c) {
}

void Apriori::buildC(Apriori::Table& c, Apriori::Table& l) {
}

void Apriori::incrementSup(Apriori::Table& c, Itemset& transaction) {
}

void Apriori::filterTable(Apriori::Table& c, Apriori::Table& l) {
}

void Apriori::mineRulesFrom(Apriori::TableRow& row) {
}
