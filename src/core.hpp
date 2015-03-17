#ifndef _APRIORI_CORE_H_
#define _APRIORI_CORE_H_

#include <set>
#include <utility>
#include <vector>

namespace apriori {
    typedef std::set<int> Itemset;

    struct AssocRule {
        Itemset* lhs;
        Itemset* rhs;
        double sup;
        double conf;
        ~AssocRule();
    };

    class Apriori {
        struct TableRow {
            Itemset patt;
            int sup;
            TableRow(Itemset& patt) : patt(patt), sup(0) {}
        };

        struct TableCompare {
            bool operator() (const TableRow* lhs, const TableRow* rhs) const;
        };

        typedef std::set<TableRow*, TableCompare> Table;

        private:
            double minSup;
            int minSupCount;    // initialized in mineFreqPatts()
            std::vector<Itemset*> transactions;
            std::vector<Table*> lList;
            std::vector<AssocRule*> rules;
            void mineFreqPatts();
            void buildC(Table& c);
            void buildC(Table& c, Table& l);
            static bool hasSameHead(Itemset& a, Itemset& b);
            static void incrementSup(Table& c, Itemset& transaction);
            void filterTable(Table& c, Table& l);
            void mineRulesFrom(TableRow& row);

        public:
            Apriori(double minSup);
            ~Apriori();
            void add(Itemset* itemset);
            const std::vector<AssocRule*>& mineRules();
    };
}   // apriori

#endif
