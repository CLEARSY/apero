/** decomposition.cpp

   \copyright Copyright © CLEARSY 2022
   \license This file is part of ppTransSmt

   ppTransSmt is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ppTransSmt is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ppTransSmt. If not, see <https://www.gnu.org/licenses/>.
*/
#include <map>
#include "decomposition.h"
#include "exprDesc.h"
#include "predDesc.h"

inline bool isProduct(const BType &ty){
    return (ty.getKind() == BType::Kind::ProductType); 
}

inline bool isStruct(const BType &ty){
    return (ty.getKind() == BType::Kind::Struct); 
}

namespace decomp {
    class DContext {
        // Def: I call a normalized mapplet
        // - either an ident whose type is not a product nor a struct
        // - or a mapplet whose components are normalized mapplets.
        // - or a record whose components are normalized mapplets.
        public:
            // Return a normalized mapplet for [v].
            Expr decompose(const VarName &v, const BType &ty);
            // Decompose a vector of variables and push the result on the local context stack
            std::vector<TypedVar> push_vars(const std::vector<TypedVar> &vars);
            void push_vars2(const std::vector<TypedVar> &vars);
            // Pop the local symbols stack
            void pop_vars();
        private:
            std::map<VarName,Expr> gSubst; // normalized mapplets for already seen global symbols.
            std::vector<std::map<VarName,Expr>> lSubst; // normalized mapplets for local symbols.
            // Generated a normalized mapplet for [v]. Add the generated new variables in [vec].
            static Expr nameComponents(int &cpt, std::vector<TypedVar> &vec,const VarName &v, const BType &ty);
    };

    // Replace product-type variable with normalized mapplets in p/e/def/po/sg.
    void decompose(DContext &ctx, Pred &p);
    void decompose(DContext &ctx, Expr &e);
    void decompose(DContext &ctx, pog::Define &def);
    void decompose(DContext &ctx, pog::POGroup &po);
    void decompose(DContext &ctx, pog::PO &sg);
}

Expr decomp::DContext::nameComponents(int &cpt, std::vector<TypedVar> &vec,const VarName &v, const BType &ty){
    if(isProduct(ty)){
        auto &b = ty.toProductType();
        return Expr::makeBinaryExpr(
                Expr::BinaryOp::Mapplet,
                nameComponents(cpt,vec,v,b.lhs),
                nameComponents(cpt,vec,v,b.rhs),
                ty);
    } else if(isStruct(ty)){
        auto &s = ty.toRecordType();
        std::vector<std::pair<std::string,Expr>> fields;
        for(auto &p : s.fields)
            fields.push_back({p.first,nameComponents(cpt,vec,v,p.second)});
        return Expr::makeRecord(std::move(fields),ty);
    } else {
        ++cpt;

        assert(v.kind() != VarName::Kind::FreshId);
        assert(v.kind() != VarName::Kind::Tmp);
        VarName nv = (v.kind() == VarName::Kind::NoSuffix)?
            VarName::makeVarWithoutSuffix("_" + v.prefix() + "_" + std::to_string(cpt)):
            VarName::makeVar("_" + v.prefix() + "_" + std::to_string(cpt),v.suffix());
        vec.push_back({nv,ty});
        return Expr::makeIdent(nv,ty);
    }
}

Expr decomp::DContext::decompose(const VarName &v, const BType &ty){
    for(int i=lSubst.size()-1;i>=0;i--){
        auto it = lSubst[i].find(v);
        if(it != lSubst[i].end()){
            return it->second.copy();
        }
    }
    auto it = gSubst.find(v);
    if(it != gSubst.end()){
        return it->second.copy();
    } else {
        int cpt = 0;
        std::vector<TypedVar> dummy;
        Expr mp = nameComponents(cpt,dummy,v,ty);
        gSubst[v] = mp.copy();
        return mp;
    }
}

void decomp::DContext::pop_vars(){
    lSubst.pop_back();
}

std::vector<TypedVar> decomp::DContext::push_vars(const std::vector<TypedVar> &vars){
    std::vector<TypedVar> res;
    std::map<VarName,Expr> map;
    for(auto &v : vars){
        if(isProduct(v.type) || isStruct(v.type)){
            int cpt = 0;
            map[v.name] = nameComponents(cpt,res,v.name,v.type);
        } else {
            res.push_back(v);
        }
    }
    lSubst.push_back(std::move(map));
    return res;
}

void decomp::DContext::push_vars2(const std::vector<TypedVar> &vars){
    std::vector<TypedVar> res;
    std::map<VarName,Expr> map;
    for(auto &v : vars)
        res.push_back(v);
    lSubst.push_back(std::move(map));
}


void decomp::decompose(DContext &ctx, Expr &e){
    switch(e.getTag()){
        case Expr::EKind::EmptySet:
        case Expr::EKind::INTEGER:
        case Expr::EKind::STRING:
        case Expr::EKind::BOOL:
        case Expr::EKind::REAL:
        case Expr::EKind::FLOAT:
        case Expr::EKind::NATURAL:
        case Expr::EKind::NATURAL1:
        case Expr::EKind::INT:
        case Expr::EKind::NAT:
        case Expr::EKind::NAT1:
        case Expr::EKind::StringLiteral:
        case Expr::EKind::RealLiteral:
        case Expr::EKind::IntegerLiteral:
        case Expr::EKind::TRUE:
        case Expr::EKind::FALSE:
        case Expr::EKind::MinInt:
        case Expr::EKind::MaxInt:
        case Expr::EKind::Predecessor:
    case Expr::EKind::Successor:
            {
                return;
            }
        case Expr::EKind::Id:
            {
                if(isProduct(e.getType()) || isStruct(e.getType())){
                    e = ctx.decompose(e.getId(),e.getType());
                }
                return;
            }
        case Expr::EKind::UnaryExpr:
            {
                Expr::UnaryExpr &u = e.toUnaryExpr();
                decompose(ctx,u.content);
                return;
            }
        case Expr::EKind::BinaryExpr:
            {
                auto &b = e.toBinaryExpr();
                decompose(ctx,b.lhs);
                decompose(ctx,b.rhs);
                return;
            }
        case Expr::EKind::NaryExpr:
            {
                auto &n = e.toNaryExpr();
                for(auto &e : n.vec)
                    decompose(ctx,e);
                return;
            }
        case Expr::EKind::TernaryExpr:
            {
                auto &t = e.toTernaryExpr();
                decompose(ctx,t.fst);
                decompose(ctx,t.snd);
                decompose(ctx,t.thd);
                return;
            }
        case Expr::EKind::BooleanExpr:
            {
                decompose(ctx,e.toBooleanExpr());
                return;
            }
        case Expr::EKind::QuantifiedExpr:
            {
                auto &q = e.toQuantiedExpr();
                if(q.op == Expr::QuantifiedOp::Lambda){
                    ctx.push_vars2(q.vars);
                    decompose(ctx,q.body);
                    decompose(ctx,q.cond);
                    ctx.pop_vars();
                    return;
                } else {
                    q.vars = ctx.push_vars(q.vars);
                    decompose(ctx,q.body);
                    decompose(ctx,q.cond);
                    ctx.pop_vars();
                    return;
                }
            }
        case Expr::EKind::QuantifiedSet:
            {
                auto &q = e.toQuantifiedSet();
                ctx.push_vars2(q.vars);
                decompose(ctx,q.cond);
                ctx.pop_vars();
                return;
            }
        case Expr::EKind::Record:
            {
                auto &r = e.toRecordExpr();
                for(auto &pair : r.fields)
                    decompose(ctx,pair.second);
                return;
            }
        case Expr::EKind::Struct:
            {
                auto &r = e.toStructExpr();
                for(auto &pair : r.fields)
                    decompose(ctx,pair.second);
                return;
            }
        case Expr::EKind::Record_Field_Access:
            {
                auto &a = e.toRecordAccess();
                decompose(ctx,a.rec);
                return;
            }
        case Expr::EKind::Record_Field_Update:
            {
                auto &u = e.toRecordUpdate();
                decompose(ctx,u.rec);
                decompose(ctx,u.fvalue);
                return;
            }
    }
    assert(false); // unreachable
}

void decomp::decompose(DContext &ctx, Pred &p){
    switch(p.getTag()){
        case Pred::PKind::True:
        case Pred::PKind::False:
            return;
        case Pred::PKind::Implication:
            {
                auto &b = p.toImplication();
                decompose(ctx,b.lhs);
                decompose(ctx,b.rhs);
                return;
            }
        case Pred::PKind::Equivalence:
            {
                auto &e = p.toEquivalence();
                decompose(ctx,e.lhs);
                decompose(ctx,e.rhs);
                return;
            }
        case Pred::PKind::ExprComparison:
            {
                auto &cmp = p.toExprComparison();
                decompose(ctx,cmp.lhs);
                decompose(ctx,cmp.rhs);
                return;
            }
        case Pred::PKind::Negation:
            {
                auto &neg = p.toNegation();
                decompose(ctx,neg.operand);
                return;
            }
        case Pred::PKind::Conjunction:
            {
                auto &n = p.toConjunction();
                for(auto &q : n.operands)
                    decompose(ctx,q);
                return;
            }
        case Pred::PKind::Disjunction:
            {
                auto &n = p.toDisjunction();
                for(auto &q : n.operands)
                    decompose(ctx,q);
                return;
            }
        case Pred::PKind::Forall:
            {
                auto &q = p.toForall();
                auto vars = ctx.push_vars(q.vars);
                decompose(ctx,q.body);
                ctx.pop_vars();
                return;
            }
        case Pred::PKind::Exists:
            {
                auto &q = p.toExists();
                auto vars = ctx.push_vars(q.vars);
                decompose(ctx,q.body);
                ctx.pop_vars();
                return;
            }
    }
    assert(false); // unreachable
}

void decomp::decompose(DContext &ctx, pog::Define &def){
    // Remark: hash is not recomputed
    for(auto &p : def.ghyps)
        decompose(ctx,p);
}

void decomp::decompose(DContext &ctx, pog::PO &sg){
    decompose(ctx,sg.goal);
}

void decomp::decompose(DContext &ctx, pog::POGroup &po){
    // Remark: hash is not recomputed
    for(auto &p : po.hyps)
        decompose(ctx,p);
    for(auto &p : po.localHyps)
        decompose(ctx,p);
    for(auto &sg : po.simpleGoals)
        decompose(ctx,sg);
}

void decomp::decompose(pog::Pog &pog){
    DContext ctx;
    for(auto &def : pog.defines)
        decompose(ctx,def);
    for(auto &po : pog.pos)
        decompose(ctx,po);
}
