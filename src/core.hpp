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
        private:
            double minSup;
            std::vector<Itemset*> transactions;
            std::vector<AssocRule*> rules;

        public:
            Apriori(double minSup);
            ~Apriori();
            void add(Itemset* itemset);
            const std::vector<AssocRule*>& mineRules();
    };
}   // apriori

#endif
