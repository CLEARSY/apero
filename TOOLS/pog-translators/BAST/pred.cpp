/** pred.cpp

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
#include "pred.h"
#include "predDesc.h"

void Pred::getAllVars(std::set<VarName> &accu) const {
    desc->getAllVars(accu);
}
void Pred::substFreshId(const std::string &id, const VarName &v){
    desc->substFreshId(id,v);
}
void Pred::getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
    desc->getFreeVars(boundVars,freeVars,freeVarsThis);
}
void Pred::getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
    desc->getFreeVars(boundVars,accu);
}
void Pred::getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
    desc->getFreeTVars(boundVars,accu);
}
Pred Pred::makeImplication(Pred &&lhs, Pred &&rhs, const std::string &goalTag){
    return Pred(new Implication(std::move(lhs),std::move(rhs)),goalTag);
};
Pred Pred::makeEquivalence(Pred &&lhs, Pred &&rhs, const std::string &goalTag){
    return Pred(new Equivalence(std::move(lhs),std::move(rhs)),goalTag);
};
Pred Pred::makeExprComparison(Pred::ComparisonOp op, Expr &&lhs, Expr &&rhs, const std::string &goalTag){
    if(op == ComparisonOp::Equality){
        assert(lhs.getType() == rhs.getType());
    }
    return Pred(new ExprComparison(op,std::move(lhs),std::move(rhs)),goalTag);
};
Pred Pred::makeNegation(Pred &&p, const std::string &goalTag){
    return Pred(new NegationPred(std::move(p)),goalTag);
};
Pred Pred::makeConjunction(std::vector<Pred> &&vec, const std::string &goalTag){
    return Pred(new Conjunction(std::move(vec)),goalTag);
};
Pred Pred::makeDisjunction(std::vector<Pred> &&vec, const std::string &goalTag){
    return Pred(new Disjunction(std::move(vec)),goalTag);
};
Pred Pred::makeForall(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag){
    return Pred(new Forall(ids,std::move(body)),goalTag);
};
Pred Pred::makeExists(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag){
    return Pred(new Exists(ids,std::move(body)),goalTag);
};
Pred Pred::makeExistsForWitness(const std::vector<TypedVar> &ids, Pred &&body, const std::string &goalTag){
    return Pred(new Exists(ids,std::move(body),true),goalTag);
}
Pred Pred::makeTrue(const std::string &goalTag){  
    return Pred(new True(),goalTag);
}
Pred Pred::makeFalse(const std::string &goalTag){
    return Pred(new False(),goalTag);
}
void Pred::accept(Visitor &visitor) const {
    desc->accept(visitor);
};
void Pred::subst(const std::map<VarName,Expr> &map) {
    if(!map.empty())
        desc->subst(map);
};
void Pred::alpha(const std::map<VarName,VarName> &map) {
    desc->alpha(map);
};
const Pred::Implication& Pred::toImplication() const {
    assert(desc->tag() == PKind::Implication);
    return static_cast<Implication&>(*desc);
};
const Pred::Equivalence& Pred::toEquivalence() const {
    assert(desc->tag() == PKind::Equivalence);
    return static_cast<Equivalence&>(*desc);
};
const Pred::ExprComparison& Pred::toExprComparison() const {
    assert(desc->tag() == PKind::ExprComparison);
    return static_cast<ExprComparison&>(*desc);
};
const Pred::NegationPred& Pred::toNegation() const{
    assert(desc->tag() == PKind::Negation);
    return static_cast<NegationPred&>(*desc);
};
const Pred::Conjunction& Pred::toConjunction() const{
    assert(desc->tag() == PKind::Conjunction);
    return static_cast<Conjunction&>(*desc);
};
const Pred::Disjunction& Pred::toDisjunction() const{
    assert(desc->tag() == PKind::Disjunction);
    return static_cast<Disjunction&>(*desc);
};
const Pred::Forall& Pred::toForall() const{
    assert(desc->tag() == PKind::Forall);
    return static_cast<Forall&>(*desc);
};
const Pred::Exists& Pred::toExists() const {
    assert(desc->tag() == PKind::Exists);
    return static_cast<Exists&>(*desc);
};
Pred::Implication& Pred::toImplication() {
    assert(desc->tag() == PKind::Implication);
    return static_cast<Implication&>(*desc);
};
Pred::Equivalence& Pred::toEquivalence() {
    assert(desc->tag() == PKind::Equivalence);
    return static_cast<Equivalence&>(*desc);
};
Pred::ExprComparison& Pred::toExprComparison() {
    assert(desc->tag() == PKind::ExprComparison);
    return static_cast<ExprComparison&>(*desc);
};
Pred::NegationPred& Pred::toNegation(){
    assert(desc->tag() == PKind::Negation);
    return static_cast<NegationPred&>(*desc);
};
Pred::Conjunction& Pred::toConjunction(){
    assert(desc->tag() == PKind::Conjunction);
    return static_cast<Conjunction&>(*desc);
};
Pred::Disjunction& Pred::toDisjunction(){
    assert(desc->tag() == PKind::Disjunction);
    return static_cast<Disjunction&>(*desc);
};
Pred::Forall& Pred::toForall(){
    assert(desc->tag() == PKind::Forall);
    return static_cast<Forall&>(*desc);
};
Pred::Exists& Pred::toExists(){
    assert(desc->tag() == PKind::Exists);
    return static_cast<Exists&>(*desc);
};

Pred::PKind Pred::getTag() const { return desc->tag(); }

int Pred::compare(const Pred &p1, const Pred& p2){
    if(p1.getTag() == p2.getTag()){
        switch(p1.getTag()){
            case Pred::PKind::True:
            case Pred::PKind::False:
                return 0;
            case Pred::PKind::Negation:
                {
                    auto &u1 = p1.toNegation();
                    auto &u2 = p2.toNegation();
                    return compare(u1.operand, u2.operand);
                }
            case Pred::PKind::Implication:
                {
                    auto &b1 = p1.toImplication();
                    auto &b2 = p2.toImplication();
                    int res = compare(b1.lhs, b2.lhs);
                    if(res == 0)
                        return compare(b1.rhs, b2.rhs);
                    else
                        return res;
                }
            case Pred::PKind::Equivalence:
                {
                    auto &b1 = p1.toEquivalence();
                    auto &b2 = p2.toEquivalence();
                    int res = compare(b1.lhs, b2.lhs);
                    if(res == 0)
                        return compare(b1.rhs, b2.rhs);
                    else
                        return res;
                }
            case Pred::PKind::ExprComparison:
                {
                    auto &b1 = p1.toExprComparison();
                    auto &b2 = p2.toExprComparison();
                    if(b1.op == b2.op){
                        int res = Expr::compare(b1.lhs,b2.lhs);
                        if(res == 0){
                            return Expr::compare(b1.rhs,b2.rhs);
                        } else {
                            return res;
                        }
                    } else if (b1.op < b2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Pred::PKind::Disjunction:
                {
                    auto &n1 = p1.toDisjunction();
                    auto &n2 = p2.toDisjunction();
                    return vec_compare(n1.operands,n2.operands);
                }
            case Pred::PKind::Conjunction:
                {
                    auto &n1 = p1.toConjunction();
                    auto &n2 = p2.toConjunction();
                    return vec_compare(n1.operands,n2.operands);
                }
            case Pred::PKind::Exists:
                {
                    auto &q1 = p1.toExists();
                    auto &q2 = p2.toExists();
                    int i = TypedVar::vec_compare(q1.vars,q2.vars);
                    if(i == 0){
                        return Pred::compare(q1.body,q2.body);
                    } else {
                        return i;
                    }
                }
            case Pred::PKind::Forall:
                {
                    auto &q1 = p1.toForall();
                    auto &q2 = p2.toForall();
                    int i = TypedVar::vec_compare(q1.vars,q2.vars);
                    if(i == 0){
                        return Pred::compare(q1.body,q2.body);
                    } else {
                        return i;
                    }
                }
        }
        assert(false); // unreachable
    } else if (p1.getTag() < p2.getTag()){
        return -1;
    } else {
        return 1;
    }
}

int Pred::vec_compare(const std::vector<Pred> &lhs, const std::vector<Pred>& rhs){
       if(lhs.size() == rhs.size()){
        int i = 0;
        while(i<lhs.size()){
            int res = compare(lhs.at(i),rhs.at(i));
            if(res != 0) return res;
            i++;
        }
        return 0;
    } else {
        return (lhs.size() - rhs.size());
    }
    return 0;
}

size_t Pred::hash_combine(size_t seed) const {
    seed = desc->hash_combine(seed);
    return hashUtil::hash_combine_int(static_cast<int>(desc->tag()),seed);
}

std::string Pred::to_string(ComparisonOp op){
    switch(op){
        case ComparisonOp::Membership: return ":";
        case ComparisonOp::Subset: return "<:";
        case ComparisonOp::Strict_Subset: return "<<:";
        case ComparisonOp::Equality: return "=";
        case ComparisonOp::Fge: return ">=f";
        case ComparisonOp::Fgt: return ">f";
        case ComparisonOp::Flt: return "<f";
        case ComparisonOp::Fle: return "<=f";
        case ComparisonOp::Rle: return "<=r";
        case ComparisonOp::Rlt: return "<r";
        case ComparisonOp::Rge: return ">=r";
        case ComparisonOp::Rgt: return ">r";
        case ComparisonOp::Ile: return "<=i";
        case ComparisonOp::Ilt: return "<i";
        case ComparisonOp::Ige: return ">=i";
        case ComparisonOp::Igt: return ">i";
    }
    assert(false); // unreachable
};

std::string Pred::show() const {
    switch(desc->tag()){
        case Pred::PKind::Implication:
            {
                auto& b = toImplication();
                return "(=> " + b.lhs.show() + " " + b.rhs.show() + ")";
            }
        case Pred::PKind::Equivalence:
            {
                auto& b = toEquivalence();
                return "(<=> " + b.lhs.show() + " " + b.rhs.show() + ")";
            }
        case Pred::PKind::ExprComparison:
            {
                auto& c = toExprComparison();
                return "(" + to_string(c.op) + " " + c.lhs.show() + " " + c.rhs.show() + ")";
            }
        case Pred::PKind::Negation:
            return "(not " + toNegation().operand.show() + ")";
        case Pred::PKind::Conjunction:
            {
                auto& prd = toConjunction();
                std::string accu;
                for(auto &p : prd.operands)
                    accu += " " + p.show();
                return "(and" + accu + ")";
            }
        case Pred::PKind::Disjunction:
            {
                auto& prd = toDisjunction();
                std::string accu;
                for(auto &p : prd.operands)
                    accu += " " + p.show();
                return "(or" + accu + ")";
            }
        case Pred::PKind::Forall:
            {
                auto& q = toForall();
                assert(q.vars.size()>0);
                auto vars = q.vars[0].name.show();
                for(int i=1;i<q.vars.size();++i)
                    vars += " " + q.vars[i].name.show();
                return "(forall (" + vars + ") " + q.body.show() + ")";
            }
        case Pred::PKind::Exists:
            {
                auto& q = toExists();
                assert(q.vars.size()>0);
                auto vars = q.vars[0].name.show();
                for(int i=1;i<q.vars.size();++i)
                    vars += " " + q.vars[i].name.show();
                return "(exists (" + vars + ") " + q.body.show() + ")";
            }
        case Pred::PKind::True:
            return "btrue";
        case Pred::PKind::False:
            return "bfalse";
    };
    assert(false); // unreachable
}

Pred Pred::copy() const {
    switch(desc->tag()){
        case Pred::PKind::Implication:
            {
                auto& b = toImplication();
                return makeImplication(b.lhs.copy(),b.rhs.copy(),goalTag);
            }
        case Pred::PKind::Equivalence:
            {
                auto& b = toEquivalence();
                return makeEquivalence(b.lhs.copy(),b.rhs.copy(),goalTag);
            }
        case Pred::PKind::ExprComparison:
            {
                auto& c = toExprComparison();
                return makeExprComparison(c.op,c.lhs.copy(),c.rhs.copy(),goalTag);
            }
        case Pred::PKind::Negation:
            return makeNegation(toNegation().operand.copy(),goalTag);
        case Pred::PKind::Conjunction:
            {
                auto& prd = toConjunction();
                std::vector<Pred> accu;
                for(auto &p : prd.operands)
                    accu.push_back(p.copy());
                return makeConjunction(std::move(accu),goalTag);
            }
        case Pred::PKind::Disjunction:
            {
                auto& prd = toDisjunction();
                std::vector<Pred> accu;
                for(auto &p : prd.operands)
                    accu.push_back(p.copy());
                return makeDisjunction(std::move(accu),goalTag);
            }
        case Pred::PKind::Forall:
            {
                auto& q = toForall();
                return makeForall(q.vars,q.body.copy(),goalTag);
            }
        case Pred::PKind::Exists:
            {
                auto& q = toExists();
                return makeExists(q.vars,q.body.copy(),goalTag);
            }
        case Pred::PKind::True:
            return makeTrue(goalTag);
        case Pred::PKind::False:
            return makeFalse(goalTag);
    };
    assert(false); // unreachable
}
bool Pred::alpha_equals(Context &ctx, const Pred& p1, const Pred& p2){
    if(p1.getTag() != p2.getTag())
        return false;

    switch(p1.getTag()){
        case Pred::PKind::Implication:
            {
                auto& b1 = p1.toImplication();
                auto& b2 = p2.toImplication();
                return alpha_equals(ctx,b1.lhs,b2.lhs) && alpha_equals(ctx,b1.rhs,b2.rhs);
            }
        case Pred::PKind::Equivalence:
            {
                auto& b1 = p1.toEquivalence();
                auto& b2 = p2.toEquivalence();
                return alpha_equals(ctx,b1.lhs,b2.lhs) && alpha_equals(ctx,b1.rhs,b2.rhs);
            }
        case Pred::PKind::ExprComparison:
            {
                auto& c1 = p1.toExprComparison();
                auto& c2 = p2.toExprComparison();
                return c1.op == c2.op && Expr::alpha_equals(ctx,c1.lhs,c2.lhs) && Expr::alpha_equals(ctx,c1.rhs,c2.rhs);
            }
        case Pred::PKind::Negation:
            {
                return alpha_equals(ctx,p1.toNegation().operand,p2.toNegation().operand);
            }

        case Pred::PKind::Conjunction:
            {
                auto& prd1 = p1.toConjunction();
                auto& prd2 = p2.toConjunction();
                if(prd1.operands.size() != prd2.operands.size())
                    return false;
                for(int i=0;i<prd1.operands.size();i++){
                    if(!alpha_equals(ctx,prd1.operands[i],prd2.operands[i]))
                        return false;
                }
                return true;
            }
        case Pred::PKind::Disjunction:
            {
                auto& prd1 = p1.toDisjunction();
                auto& prd2 = p2.toDisjunction();
                if(prd1.operands.size() != prd2.operands.size())
                    return false;
                for(int i=0;i<prd1.operands.size();i++){
                    if(!alpha_equals(ctx,prd1.operands[i],prd2.operands[i]))
                        return false;
                }
                return true;
            }
        case Pred::PKind::Forall:
            {
                auto& q1 = p1.toForall();
                auto& q2 = p2.toForall();
                if(!ctx.push(q1.vars,q2.vars))
                    return false;
                bool res = Pred::alpha_equals(ctx,q1.body,q2.body);
                ctx.pop();
                return res;
            }
        case Pred::PKind::Exists:
            {
                auto& q1 = p1.toExists();
                auto& q2 = p2.toExists();
                if(!ctx.push(q1.vars,q2.vars))
                    return false;
                bool res = Pred::alpha_equals(ctx,q1.body,q2.body);
                ctx.pop();
                return res;
            }
        case Pred::PKind::True:
        case Pred::PKind::False:
            return true;
    };
    assert(false); // unreachable
}
bool Pred::alpha_equals(const Pred& p1, const Pred& p2){
    Context ctx;
    return alpha_equals(ctx,p1,p2);
}
