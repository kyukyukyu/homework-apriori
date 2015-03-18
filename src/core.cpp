#include "core.hpp"

#include <map>
#include <stack>
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

    vector<Apriori::Table*>::iterator it2;
    // since the first element is always nullptr, advance iterator at first
    for (it2 = ++this->lList.begin();
         it2 != this->lList.end();
         ++it2) {
        Apriori::Table* l = *it2;
        Apriori::Table::iterator itRow;

        for (itRow = l->begin();
             itRow != l->end();
             ++itRow) {
            Apriori::TableRow* row = *itRow;
            delete row;
        }

        delete l;
    }

    vector<AssocRule*>::iterator it3;
    for (it3 = this->rules.begin();
         it3 != this->rules.end();
         ++it3) {
        AssocRule* rule = *it3;
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

    // compute minSupCount
    this->minSupCount = this->transactions.size() * this->minSup / 100.0;

    // build C1
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
        this->lList.push_back(l);
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
            ++itRowJ;
        }
    }

    // step 2: pruning
    Apriori::Table::iterator itRowC = c.begin();
    while (itRowC != c.end()) {
        Apriori::Table::iterator currIt = itRowC++;
        Apriori::TableRow* currRow = *currIt;
        Itemset& patt = currRow->patt;
        Itemset::const_iterator itItem;
        bool isValidCandidate = true;

        // generate subsets and check if all of them are in l
        for (itItem = patt.begin(); itItem != patt.end(); ++itItem) {
            Itemset subset(patt);
            subset.erase(subset.find(*itItem));
            Apriori::TableRow subsetRow(subset);

            if (l.find(&subsetRow) == l.end()) {
                // subset not found in l
                isValidCandidate = false;
                break;
            }
        }

        if (!isValidCandidate) {
            c.erase(currIt);
        }
    }
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
    Apriori::Table::iterator it;
    for (it = c.begin(); it != c.end(); ++it) {
        TableRow* row = *it;
        Itemset& patt = row->patt;
        if (std::includes(transaction.begin(), transaction.end(),
                          patt.begin(), patt.end())) {
            ++row->sup;
        }
    }
}

void Apriori::filterTable(Apriori::Table& c, Apriori::Table& l) {
    Apriori::Table::const_iterator it;
    for (it = c.begin(); it != c.end(); ++it) {
        Apriori::TableRow* row = *it;

        if (row->sup >= this->minSupCount) {
            l.insert(row);
        }
    }
}

void Apriori::mineRulesFrom(Apriori::TableRow& row) {
    Itemset& patt = row.patt;
    Itemset::size_type pattSize = patt.size();
    vector<bool> toLhs(pattSize, false);
    vector<bool>::size_type i;
    stack<bool> isUsed;

    i = 0;
    isUsed.push(false);
    while (!isUsed.empty()) {
        if (i == pattSize) {
            bool isAllTrue = true;
            bool isNotAllFalse = false;
            vector<bool>::const_iterator it = toLhs.begin();
            for (it = toLhs.begin(); it != toLhs.end(); ++it) {
                isAllTrue &= *it;
                isNotAllFalse |= *it;

                if (!isAllTrue && isNotAllFalse) {
                    break;
                }
            }

            if (!isAllTrue && isNotAllFalse) {
                AssocRule* rule = new AssocRule();
                this->makeRule(patt, toLhs, *rule);
                this->rules.push_back(rule);
            }

            isUsed.pop();
            --i;
        }

        bool top = isUsed.top();
        if (toLhs[i] == false && top == false) {
            toLhs[i] = false;
            isUsed.pop();
            isUsed.push(true);
            isUsed.push(false);
        } else if (toLhs[i] == false && top == true) {
            toLhs[i] = true;
        } else if (toLhs[i] == true && top == true) {
            isUsed.pop();
            --i;
            continue;
        }
        ++i;
    }

}

void Apriori::makeRule(Itemset& patt, std::vector<bool>& toLhs, AssocRule& rule) {
    Itemset* lhs = new Itemset();
    Itemset* rhs = new Itemset();
    Itemset::const_iterator it;
    Itemset::size_type pattSize = patt.size();
    Itemset::size_type i;

    for (it = patt.begin(), i = 0; i < pattSize; ++it, ++i) {
        int item = *it;
        if (toLhs[i] == true) {
            lhs->insert(item);
        } else {
            rhs->insert(item);
        }
    }

    Table* lLhs = this->lList[lhs->size()];
    Table* lUnion = this->lList[patt.size()];
    TableRow* rowTemp;
    TableRow* rowLhs;
    TableRow* rowUnion;

    rowTemp = new TableRow(*lhs);
    rowLhs = *(lLhs->find(rowTemp));
    delete rowTemp;
    rowTemp = new TableRow(patt);
    rowUnion = *(lUnion->find(rowTemp));
    delete rowTemp;

    rule.lhs = lhs;
    rule.rhs = rhs;
    rule.sup = (double) rowUnion->sup / (double) this->transactions.size();
    rule.conf = (double) rowUnion->sup / (double) rowLhs->sup;
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
