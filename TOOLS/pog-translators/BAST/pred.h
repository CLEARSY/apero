/** pred.h

   \copyright Copyright © CLEARSY 2022
   \license This file is part of BAST.

   BAST is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    BAST is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with BAST. If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef PRED_H
#define PRED_H

#include <memory>
#include <cassert>
#include <vector>
#include <set>
#include <map>
#include <string>
#include "expr.h"

class Pred {
    public:
        enum class PKind {
            Implication, Equivalence, Conjunction, Disjunction, Forall, Exists,
            ExprComparison, Negation, True, False };

        enum class ComparisonOp {
            Membership, Subset, Strict_Subset, Equality,
            Ige, Igt, Ilt, Ile, // integer comparison
            Fge, Fgt, Flt, Fle, // float comparison
            Rle, Rlt, Rge, Rgt // real comparison
        };
        static std::string to_string(ComparisonOp op);

        Pred():desc{nullptr}{};
        PKind getTag() const;
        Pred copy() const;

        // Capture-avoiding substitution
        void subst(const std::map<VarName,Expr> &map);
        // Alpha renaming. The new var names must not occur (free or bound) in the expression
        void alpha(const std::map<VarName,VarName> &map);

        static Pred makeImplication(Pred &&lhs, Pred &&rhs, const std::string &goalTag = "");
        static Pred makeEquivalence(Pred &&lhs, Pred &&rhs, const std::string &goalTag = "");
        static Pred makeExprComparison(ComparisonOp op, Expr &&lhs, Expr &&rhs, const std::string &goalTag = "");
        static Pred makeNegation(Pred &&p, const std::string &goalTag = "");
        static Pred makeConjunction(std::vector<Pred> &&vec, const std::string &goalTag = "");
        static Pred makeDisjunction(std::vector<Pred> &&vec, const std::string &goalTag = "");
        static Pred makeForall(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag = "");
        static Pred makeExists(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag = "");
        static Pred makeTrue(const std::string &goalTag = ""); 
        static Pred makeFalse(const std::string &goalTag = ""); 

        static Pred makeExistsForWitness(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag = "");

        class Conjunction;
        class Disjunction;
        class ExprComparison;
        class NegationPred;
        class Implication;
        class Equivalence;
        class Forall;
        class Exists;
        class True;
        class False;

        const Implication& toImplication() const;
        Implication& toImplication();
        const Equivalence& toEquivalence() const;
        Equivalence& toEquivalence();
        const ExprComparison& toExprComparison() const;
        ExprComparison& toExprComparison();
        const NegationPred& toNegation() const;
        NegationPred& toNegation();
        const Conjunction& toConjunction() const;
        Conjunction& toConjunction();
        const Disjunction& toDisjunction() const;
        Disjunction& toDisjunction();
        const Forall& toForall() const;
        Forall& toForall();
        const Exists& toExists() const;
        Exists& toExists();

        class Visitor;
        void accept(Visitor &v) const;

        size_t hash_combine(size_t seed) const;
        const std::string& getGoalTag() const { return goalTag; };
        void setGoalTag(const std::string &s){ goalTag = s; }

        std::string show() const; // for debug

        static int compare(const Pred &v1, const Pred& v2);
        static int vec_compare(const std::vector<Pred> &v1, const std::vector<Pred>& v2);
        //inline bool operator==(const Pred& other) const { return compare(*this,other) == 0; }
        //inline bool operator!=(const Pred& other) const { return compare(*this,other) != 0; }
        inline bool operator< (const Pred& other) const { return compare(*this,other) <  0; }
        //inline bool operator> (const Pred& other) const { return compare(*this,other) >  0; }
        //inline bool operator<=(const Pred& other) const { return compare(*this,other) <= 0; }
        //inline bool operator>=(const Pred& other) const { return compare(*this,other) >= 0; }

        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const;
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const;
        // Get identifiers occuring free in the expression
        std::set<VarName> getFreeVars() const {
            std::set<VarName> accu;
            getFreeVars({},accu);
            return accu;
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const;
        std::set<TypedVar> getFreeTVars() const{
            std::set<TypedVar> accu;
            getFreeTVars({},accu);
            return accu;
        };

        // Get all identifiers (free or bound) occuring in the expression
        void getAllVars(std::set<VarName> &accu) const;
        std::set<VarName> getAllVars() const {
            std::set<VarName> accu;
            getAllVars(accu);
            return accu;
        }
        // This function is used to instanciate Gpred's tag LetFreshId
        void substFreshId(const std::string &id, const VarName &v);

        static bool alpha_equals(Context &ctx, const Pred& p1, const Pred& p2);
        static bool alpha_equals(const Pred& p1, const Pred& p2);
    private:
        class PredDesc;

        std::string goalTag; // Used to describe the source of the goal of a proof obligation
        std::unique_ptr<PredDesc> desc; // content of the predicate. Never null (except if default constructor is used)
        // Constructor
        Pred(PredDesc *desc, const std::string &gt):
            desc{desc},
            goalTag{gt}
        {};
};

class Pred::PredDesc {
    public:
        virtual ~PredDesc(){};
        virtual PKind tag() const = 0;
        virtual void accept(Visitor &visitor) const = 0;
        virtual size_t hash_combine(size_t seed) const = 0;
        virtual void subst(const std::map<VarName,Expr> &map) = 0;
        virtual void alpha(const std::map<VarName,VarName> &map) = 0;
        virtual void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const = 0;
        virtual void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const = 0;
        virtual void getAllVars(std::set<VarName> &accu) const = 0;
                virtual void getFreeTVars(const std::set<VarName> &bv, std::set<TypedVar> &accu) const = 0;
        virtual void substFreshId(const std::string &id, const VarName &v) = 0;
};

class Pred::Visitor {
    public:
        virtual void visitImplication(const Pred &lhs, const Pred &rhs) = 0;
        virtual void visitEquivalence(const Pred &lhs, const Pred &rhs) = 0;
        virtual void visitExprComparison(Pred::ComparisonOp op, const Expr &lhs, const Expr &rhs) = 0;
        virtual void visitNegation(const Pred &p) = 0;
        virtual void visitConjunction(const std::vector<Pred> &vec) = 0;
        virtual void visitDisjunction(const std::vector<Pred> &vec) = 0;
        virtual void visitForall(const std::vector<TypedVar> &vars, const Pred &p) = 0;
        virtual void visitExists(const std::vector<TypedVar> &vars, const Pred &p) = 0;
        virtual void visitTrue() = 0;
        virtual void visitFalse() = 0;
};

namespace std {
    template <>
        class hash<Pred> {
            public:
                size_t operator()(const Pred &p) const { return p.hash_combine(0); };
        };
}

#endif // PRED_H
