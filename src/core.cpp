#include "core.hpp"

#include <map>
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
    c.clear();

    map<int, int> sup;
    vector<Itemset*>::const_iterator itT;
    for (itT = this->transactions.begin();
         itT != this->transactions.end();
         ++itT) {
        Itemset* transaction = *itT;
        Itemset::const_iterator itI;
        for (itI = transaction->begin();
             itI != transaction->end();
             ++itI) {
            int item = *itI;
            if (sup.count(item) == 0) {
                sup[item] = 0;
            }
            ++sup[item];
        }
    }

    map<int, int>::const_iterator itR;
    for (itR = sup.begin();
         itR != sup.end();
         ++itR) {
        Itemset is;
        Apriori::TableRow* row;

        is.insert(itR->first);
        row = new Apriori::TableRow(is);
        row->sup = itR->second;

        c.insert(row);
    }
}

void Apriori::buildC(Apriori::Table& c, Apriori::Table& l) {
    c.clear();

    // step 1: self-joining
    Apriori::Table::const_iterator itRowI;
    const Apriori::Table::const_iterator itRowEnd = l.end();
    for (itRowI = l.begin(); itRowI != itRowEnd; ++itRowI) {
        Apriori::TableRow* rowI = *itRowI;
        Apriori::Table::const_iterator itRowJ = itRowI;

        ++itRowJ;
        while (itRowJ != itRowEnd) {
            Apriori::TableRow* rowJ = *itRowJ;

            if (!Apriori::hasSameHead(rowI->patt, rowJ->patt)) {
                break;
            }

            Itemset patt(rowI->patt);
            int lastItem = *(rowJ->patt.rbegin());
            patt.insert(lastItem);
            c.insert(new Apriori::TableRow(patt));
        }
    }

    // step 2: pruning
    // TODO: implement this one
}

bool Apriori::hasSameHead(Itemset& a, Itemset& b) {
    Itemset::const_iterator itA, itB, itLast;
    itA = a.begin();
    itB = b.begin();
    itLast = a.end();
    --itLast;

    while (itA != itLast) {
        if (*itA != *itB) {
            return false;
        }
        ++itA;
        ++itB;
    }
    return true;
}

void Apriori::incrementSup(Apriori::Table& c, Itemset& transaction) {
}

void Apriori::filterTable(Apriori::Table& c, Apriori::Table& l) {
}

void Apriori::mineRulesFrom(Apriori::TableRow& row) {
}

bool Apriori::TableCompare::operator() (const Apriori::TableRow* lhs, const Apriori::TableRow* rhs) const {
    const Itemset& isL = lhs->patt;
    const Itemset& isR = rhs->patt;
    Itemset::const_iterator itL = isL.begin();
    Itemset::const_iterator itR = isR.begin();

    while (itL != isL.end() && itR != isR.end()) {
        int itemL = *itL;
        int itemR = *itR;

        if (itemL < itemR) {
            return true;
        }

        ++itL;
        ++itR;
    }

    return false;
}
