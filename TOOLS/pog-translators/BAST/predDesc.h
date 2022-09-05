/** predDesc.h

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
#ifndef PREDDESC_H
#define PREDDESC_H

#include "expr.h"
#include "pred.h"
#include "hashutil.h"

class Pred::Implication : public PredDesc {
    public:
        // Constructor
        Implication(Pred &&lhs, Pred &&rhs):
            lhs{std::move(lhs)},
            rhs{std::move(rhs)}
        {};
        // Members
        Pred lhs;
        Pred rhs;
        // Methods
        PKind tag() const { return PKind::Implication; }
        void accept(Visitor &v) const { v.visitImplication(lhs,rhs); };
        size_t hash_combine(size_t seed) const {
            return lhs.hash_combine(rhs.hash_combine(seed));
        }
        void subst(const std::map<VarName,Expr> &map) {
            lhs.subst(map);
            rhs.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            lhs.alpha(map);
            rhs.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            lhs.getFreeVars(boundVars,freeVars,freeVarsThis);
            rhs.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            lhs.getFreeVars(boundVars,accu);
            rhs.getFreeVars(boundVars,accu);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            lhs.getFreeTVars(boundVars,accu);
            rhs.getFreeTVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars(accu);
            rhs.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
};
class Pred::Equivalence : public PredDesc {
    public:
        // Constructor
        Equivalence(Pred &&lhs, Pred &&rhs):
            lhs{std::move(lhs)},
            rhs{std::move(rhs)}
        {};
        // Members
        Pred lhs;
        Pred rhs;
        // Methods
        PKind tag() const { return PKind::Equivalence; }
        void accept(Visitor &v) const { v.visitEquivalence(lhs,rhs); };
        size_t hash_combine(size_t seed) const {
            return lhs.hash_combine(rhs.hash_combine(seed));
        }
        void subst(const std::map<VarName,Expr> &map) {
            lhs.subst(map);
            rhs.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            lhs.alpha(map);
            rhs.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            lhs.getFreeVars(boundVars,freeVars,freeVarsThis);
            rhs.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            lhs.getFreeVars(boundVars,accu);
            rhs.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars(accu);
            rhs.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            lhs.getFreeTVars(boundVars,accu);
            rhs.getFreeTVars(boundVars,accu);
        }
};
class Pred::ExprComparison : public PredDesc {
    public:
        // Constructor
        ExprComparison(Pred::ComparisonOp op, Expr &&lhs, Expr &&rhs):
            op{op},
            lhs{std::move(lhs)},
            rhs{std::move(rhs)}
        {};
        // Members
        const Pred::ComparisonOp op;
        Expr lhs;
        Expr rhs;
        // Methods
        PKind tag() const { return PKind::ExprComparison; }
        void accept(Visitor &v) const { v.visitExprComparison(op,lhs,rhs); };
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_int(static_cast<int>(op),
                    lhs.hash_combine(
                        rhs.hash_combine(seed)));
        }
        void subst(const std::map<VarName,Expr> &map) {
            lhs.subst(map);
            rhs.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            lhs.alpha(map);
            rhs.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            lhs.getFreeVars(boundVars,freeVars,freeVarsThis);
            rhs.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            lhs.getFreeVars(boundVars,accu);
            rhs.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars(accu);
            rhs.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            lhs.getFreeTVars(boundVars,accu);
            rhs.getFreeTVars(boundVars,accu);
        }
};
class Pred::NegationPred : public PredDesc {
    public:
        // Constructor
        NegationPred(Pred &&p):
            operand{std::move(p)}
        {};
        // Members
        Pred operand;
        // Methods
        PKind tag() const { return PKind::Negation; }
        void accept(Visitor &v) const { v.visitNegation(operand); };
        size_t hash_combine(size_t seed) const {
            return operand.hash_combine(seed);
        }
        void subst(const std::map<VarName,Expr> &map) {
            operand.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            operand.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            operand.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            operand.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            operand.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            operand.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            operand.getFreeTVars(boundVars,accu);
        }
};
class Pred::Conjunction : public PredDesc {
    public:
        // Constructor
        Conjunction(std::vector<Pred> &&vec):
            operands{std::move(vec)}
        {};
        // Members
        std::vector<Pred> operands;
        // Methods
        PKind tag() const { return PKind::Conjunction; }
        void accept(Visitor &v) const { v.visitConjunction(operands); };
        size_t hash_combine(size_t seed) const {
            for(auto &p : operands)
                seed = p.hash_combine(seed);
            return seed;
        }
        void subst(const std::map<VarName,Expr> &map) {
            for(auto &p : operands)
                p.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            for(auto &p : operands)
                p.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            for(auto &p: operands)
                p.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &p: operands)
                p.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p: operands)
                p.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p: operands)
                p.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &p: operands)
                p.getFreeTVars(boundVars,accu);
        }
};

class Pred::Disjunction : public PredDesc {
    public:
        // Constructor
        Disjunction(std::vector<Pred> &&vec):
            operands{std::move(vec)}
        {};
        // Members
        std::vector<Pred> operands;
        // Methods
        PKind tag() const { return PKind::Disjunction; }
        void accept(Visitor &v) const { v.visitDisjunction(operands); };
        size_t hash_combine(size_t seed) const {
            for(auto &p : operands)
                seed = p.hash_combine(seed);
            return seed;
        }
        void subst(const std::map<VarName,Expr> &map) {
            for(auto &p : operands)
                p.subst(map);
        };
        void alpha(const std::map<VarName,VarName> &map) {
            for(auto &p : operands)
                p.alpha(map);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            for(auto &p: operands)
                p.getFreeVars(boundVars,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &p: operands)
                p.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p: operands)
                p.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p: operands)
                p.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &p: operands)
                p.getFreeTVars(boundVars,accu);
        }
};

class Pred::Forall : public PredDesc {
    public:
        // Constructor
        Forall(const std::vector<TypedVar> &ids, Pred &&body):
            vars{ids},
            body{std::move(body)}
        {};
        // Members
        std::vector<TypedVar> vars;
        Pred body;
        // Methods
        PKind tag() const { return PKind::Forall; }
        void accept(Visitor &v) const { v.visitForall(vars,body); };
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return body.hash_combine(seed);
        }
        void subst(const std::map<VarName,Expr> &map) {
            std::map<VarName,Expr> map2;
            for(auto &p : map)
                map2[p.first] = p.second.copy();
            for(auto &v : vars)
                map2.erase(v.name);
            std::set<VarName> freeVars;
            for(auto &p : map)
                p.second.getFreeVars({},freeVars);
            if(Expr::isRenamingNeeded(vars,freeVars)){
                body.getAllVars(freeVars);
                Expr::renameVars(vars,freeVars,map2);
                body.subst(map2);
            } else if(!map2.empty()){
                body.subst(map2);
            }
        };
        void alpha(const std::map<VarName,VarName> &map) {
            std::map<VarName,VarName> map2 { map };
            for(auto &v : vars){
                if(map.find(v.name) != map.end())
                    map2.erase(v.name);
            }
            if(!map2.empty())
                body.alpha(map2);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeVars(boundVars2,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeVars(boundVars2,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            body.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            body.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeTVars(boundVars2,accu);
        }
};
class Pred::Exists : public PredDesc {
    public:
        // Constructor
        Exists(const std::vector<TypedVar> &ids, Pred &&body):
            vars{ids},
            body{std::move(body)},
            allowWitnessInstanciation{false}
        {};
        Exists(const std::vector<TypedVar> &ids, Pred &&body, bool witness):
            vars{ids},
            body{std::move(body)},
            allowWitnessInstanciation{witness}
        {};
        // Members
        std::vector<TypedVar> vars;
        Pred body;
        const bool allowWitnessInstanciation;
        // Methods
        PKind tag() const { return PKind::Exists; }
        void accept(Visitor &v) const { v.visitExists(vars,body); };
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return hashUtil::hash_combine_int(
                    allowWitnessInstanciation? 1 : 0,
                    body.hash_combine(seed));
        }
        void subst(const std::map<VarName,Expr> &map) {
            std::map<VarName,Expr> map2;
            for(auto &p : map)
                map2[p.first] = p.second.copy();
            for(auto &v : vars)
                map2.erase(v.name);
            std::set<VarName> freeVars;
            for(auto &p : map)
                p.second.getFreeVars({},freeVars);
            if(Expr::isRenamingNeeded(vars,freeVars)){
                body.getAllVars(freeVars);
                Expr::renameVars(vars,freeVars,map2);
                body.subst(map2);
            } else if(!map2.empty()){
                body.subst(map2);
            }
        };
        void alpha(const std::map<VarName,VarName> &map) {
            std::map<VarName,VarName> map2 { map };
            for(auto &v : vars){
                if(map.find(v.name) != map.end())
                    map2.erase(v.name);
            }
            if(!map2.empty())
                body.alpha(map2);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeVars(boundVars2,freeVars,freeVarsThis);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeVars(boundVars2,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            body.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            body.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getFreeTVars(boundVars2,accu);
        }
};
class Pred::True : public PredDesc {
    public:
        // Constructor
        True(){};
        // Members
        // Methods
        PKind tag() const { return PKind::True; }
        void accept(Visitor &v) const { v.visitTrue(); };
        size_t hash_combine(size_t seed) const {
            return seed;
        }
        void subst(const std::map<VarName,Expr> &map) { };
        void alpha(const std::map<VarName,VarName> &map) { };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {}
        void getAllVars(std::set<VarName> &accu) const {}
        void substFreshId(const std::string &id, const VarName &v){}
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const { }
};
class Pred::False : public PredDesc {
    public:
        // Constructor
        False(){};
        // Members
        // Methods
        PKind tag() const { return PKind::False; }
        void accept(Visitor &v) const { v.visitFalse(); };
        size_t hash_combine(size_t seed) const {
            return seed;
        }
        void subst(const std::map<VarName,Expr> &map) { };
        void alpha(const std::map<VarName,VarName> &map) { };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {}
        void getAllVars(std::set<VarName> &accu) const {}
        void substFreshId(const std::string &id, const VarName &v){}
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const { }
};
#endif
