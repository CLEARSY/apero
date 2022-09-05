/** subst.cpp

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
#include "subst.h"

void Subst::alpha(const std::map<VarName,VarName> &map){
    if(desc != nullptr)
        desc->alpha(map);
}
void Subst::getInnerFreeVars(std::set<VarName> &accu) const {
    if(desc != nullptr)
        desc->getInnerFreeVars(accu);
}
Subst Subst::copy() const {
    if(tag == SKind::Skip)
        return makeSkip();
    else
        return Subst(tag,desc->copy());
}
class Subst::BlockSubst : public SubstDesc {
    public:
        BlockSubst(Subst &&s):content{std::move(s)}{};
        Subst content;
        size_t hash_combine(size_t seed) const {
            return content.hash_combine(seed);
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            content.getFreeVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            content.alpha(map);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            content.getModifiedVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            content.getAllVars(accu);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            content.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v) {
            content.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new BlockSubst(content.copy());
        }
};

class Subst::NarySubst : public SubstDesc {
    public:
        NarySubst(std::vector<Subst> &&vec):content{std::move(vec)}{};
        std::vector<Subst> content;
        size_t hash_combine(size_t seed) const {
            for(auto &s : content)
                seed = s.hash_combine(seed);
            return seed;
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &s : content)
                s.getFreeVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(auto &s : content)
                s.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &s : content)
                s.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &s : content)
                s.getModifiedVars(boundVars,accu);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            for(auto &s : content)
                s.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v) {
            for(auto &s : content)
                s.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::vector<Subst> vec;
            for(auto &s : content)
                vec.push_back(s.copy());
            return new NarySubst(std::move(vec));
        }
};

Subst Subst::makeSkip(){ return Subst(SKind::Skip,nullptr); }
Subst Subst::makeBlock(Subst &&s){
    return Subst(SKind::Block, new BlockSubst(std::move(s))) ;
};
Subst Subst::makeAssert(Pred &&p, Subst &&s){
    return Subst(SKind::Assert, new AssertSubst(std::move(p),std::move(s)) );
};
Subst Subst::makeIfThen(Pred &&cond, Subst &&s){
    return Subst(SKind::IfThen, new IfThenSubst(std::move(cond),std::move(s)) );
};
Subst Subst::makeIfThenElse(Pred &&cond, Subst &&s_if, Subst &&s_else){
    return Subst(SKind::IfThenElse, new IfThenElseSubst(std::move(cond),std::move(s_if),std::move(s_else)) );
};
Subst Subst::makeSimpleAssignment(const std::vector<TypedVar> &variables, std::vector<Expr> &&values){
    assert(variables.size() == values.size());
    return Subst(SKind::SimpleAssignment, new SimpleAssignmentSubst(variables,std::move(values)) );
};
Subst Subst::makeSelect(std::vector<std::pair<Pred,Subst>> &&clauses){
    return Subst(SKind::Select, new SelectSubst(std::move(clauses)) );
};
Subst Subst::makeSelectElse(std::vector<std::pair<Pred,Subst>> &&clauses, Subst &&els){
    return Subst(SKind::SelectElse, new SelectElseSubst(std::move(clauses),std::move(els)) );
};
Subst Subst::makeCase(Expr &&e, std::vector<CaseChoice> &&cases){
    return Subst(SKind::Case, new CaseSubst(std::move(e),std::move(cases)) );
};
Subst Subst::makeCaseElse(Expr &&e, std::vector<CaseChoice> &&cases, Subst &&els){
    return Subst(SKind::CaseElse, new CaseElseSubst(std::move(e),std::move(cases),std::move(els)) );
};
Subst Subst::makeAny(const std::vector<TypedVar> &vars, Pred &&p, Subst &&body){
    return Subst(SKind::Any, new AnySubst(vars,std::move(p),std::move(body)));
};
Subst Subst::makeOpCall(const std::string &name, std::vector<Expr> &&input, const std::vector<TypedVar> &output,
        const std::vector<TypedVar> &op_input, const std::vector<TypedVar> &op_output, Pred &&op_precondition, Subst &&op_body){
    assert(input.size() == op_input.size());
    assert(output.size() == op_output.size());
    return Subst(SKind::OperationCall,
            new OpCallSubst(name,std::move(input),output,op_input,op_output,std::move(op_precondition),std::move(op_body))) ;
};
Subst Subst::makeWhile(Pred &&cond, Subst &&body, Pred &&inv, Expr &&var){
    return Subst(SKind::While, new WhileSubst(std::move(cond),std::move(body),std::move(inv),std::move(var)) );
};
Subst Subst::makeWitness(std::map<std::string,Expr> &&witnesses, Subst &&body){
    return Subst(SKind::Witness, new WitnessSubst(std::move(witnesses),std::move(body)) );
};
Subst Subst::makeSequence(std::vector<Subst> &&vec){
    return Subst(SKind::Sequence, new NarySubst(std::move(vec)) );
};
Subst Subst::makeParallel(std::vector<Subst> &&vec){
    return Subst(SKind::Parallel, new NarySubst(std::move(vec)) );
};
Subst Subst::makeChoice(std::vector<Subst> &&vec){
    return Subst(SKind::Choice, new NarySubst(std::move(vec)) );
};
void Subst::accept(Visitor &visitor) const {
    switch(tag){
        case Subst::SKind::Any:
            {
                AnySubst &e = static_cast<AnySubst&>(*desc);
                visitor.visitAny(e.vars,e.p,e.body);
                break;
            }
        case Subst::SKind::Block:
            {
                BlockSubst &e = static_cast<BlockSubst&>(*desc);
                visitor.visitBlock(e.content);
                break;
            }
        case Subst::SKind::Skip:
            {
                visitor.visitSkip();
                break;
            }
        case Subst::SKind::Assert:
            {
                AssertSubst &e = static_cast<AssertSubst&>(*desc);
                visitor.visitAssert(e.condition,e.content);
                break;
            }
        case Subst::SKind::IfThen:
            {
                IfThenSubst &e = static_cast<IfThenSubst&>(*desc);
                visitor.visitIfThen(e.condition,e.s_if);
                break;
            }
        case Subst::SKind::IfThenElse:
            {
                IfThenElseSubst &e = static_cast<IfThenElseSubst&>(*desc);
                visitor.visitIfThenElse(e.condition,e.s_if,e.s_else);
                break;
            }
        case Subst::SKind::Select:
            {
                SelectSubst &e = static_cast<SelectSubst&>(*desc);
                visitor.visitSelect(e.clauses);
                break;
            }
        case Subst::SKind::SelectElse:
            {
                SelectElseSubst &e = static_cast<SelectElseSubst&>(*desc);
                visitor.visitSelectElse(e.clauses,e.s_else);
                break;
            }
        case Subst::SKind::Case:
            {
                CaseSubst &e = static_cast<CaseSubst&>(*desc);
                visitor.visitCase(e.e,e.cases);
                break;
            }
        case Subst::SKind::CaseElse:
            {
                CaseElseSubst &e = static_cast<CaseElseSubst&>(*desc);
                visitor.visitCaseElse(e.e,e.cases,e.s_else);
                break;
            }
        case Subst::SKind::OperationCall:
            {
                OpCallSubst &e = static_cast<OpCallSubst&>(*desc);
                visitor.visitOpCall(e.name,e.input,e.output,e.op_input,e.op_output,e.op_precondition,e.op_body);
                break;
            }
        case Subst::SKind::While:
            {
                WhileSubst &e = static_cast<WhileSubst&>(*desc);
                visitor.visitWhile(e.cond,e.body,e.inv,e.var);
                break;
            }
        case Subst::SKind::Sequence:
            {
                NarySubst &e = static_cast<NarySubst&>(*desc);
                visitor.visitSequence(e.content);
                break;
            }
        case Subst::SKind::Parallel:
            {
                NarySubst &e = static_cast<NarySubst&>(*desc);
                visitor.visitParallel(e.content);
                break;
            }
        case Subst::SKind::Choice:
            {
                NarySubst &e = static_cast<NarySubst&>(*desc);
                visitor.visitChoice(e.content);
                break;
            }
        case Subst::SKind::SimpleAssignment:
            {
                SimpleAssignmentSubst &e = static_cast<SimpleAssignmentSubst&>(*desc);
                visitor.visitSimpleAssignment(e.vars,e.exprs);
                break;
            }
        case Subst::SKind::Witness:
            {
                WitnessSubst &e = static_cast<WitnessSubst&>(*desc);
                visitor.visitWitness(e.witnesses,e.body);
                break;
            }
    };
};
const Subst& Subst::toBlock() const {
    assert(tag == SKind::Block);
    return static_cast<BlockSubst&>(*desc).content;
};
const Subst::AssertSubst& Subst::toAssert() const {
    assert(tag == SKind::Assert);
    return static_cast<AssertSubst&>(*desc);
};
const Subst::IfThenSubst& Subst::toIfThen() const {
    assert(tag == SKind::IfThen);
    return static_cast<IfThenSubst&>(*desc);
};
const Subst::IfThenElseSubst& Subst::toIfThenElse() const {
    assert(tag == SKind::IfThenElse);
    return static_cast<IfThenElseSubst&>(*desc);
};
const Subst::SimpleAssignmentSubst& Subst::toSimpleAssignment() const {
    assert(tag == SKind::SimpleAssignment);
    return static_cast<SimpleAssignmentSubst&>(*desc);
};
const Subst::SelectSubst& Subst::toSelect() const {
    assert(tag == SKind::Select);
    return static_cast<SelectSubst&>(*desc);
};
const Subst::SelectElseSubst& Subst::toSelectElse() const {
    assert(tag == SKind::SelectElse);
    return static_cast<SelectElseSubst&>(*desc);
};
const Subst::CaseSubst& Subst::toCase() const {
    assert(tag == SKind::Case);
    return static_cast<CaseSubst&>(*desc);
};
const Subst::CaseElseSubst& Subst::toCaseElse() const {
    assert(tag == SKind::CaseElse);
    return static_cast<CaseElseSubst&>(*desc);
};
const Subst::AnySubst& Subst::toAny() const {
    assert(tag == SKind::Any);
    return static_cast<AnySubst&>(*desc);
};
const Subst::OpCallSubst& Subst::toOpCall() const {
    assert(tag == SKind::OperationCall);
    return static_cast<OpCallSubst&>(*desc);
};
const Subst::WhileSubst& Subst::toWhile() const {
    assert(tag == SKind::While);
    return static_cast<WhileSubst&>(*desc);
};
const std::vector<Subst>& Subst::toSequence() const {
    assert(tag == SKind::Sequence);
    return static_cast<NarySubst&>(*desc).content;
};
const std::vector<Subst>& Subst::toParallel() const {
    assert(tag == SKind::Parallel);
    return static_cast<NarySubst&>(*desc).content;
};
const std::vector<Subst>& Subst::toChoice() const {
    assert(tag == SKind::Choice);
    return static_cast<NarySubst&>(*desc).content;
};
const Subst::WitnessSubst& Subst::toWitness() const {
    assert(tag == SKind::Witness);
    return static_cast<WitnessSubst&>(*desc);
};

Subst& Subst::toBlock() {
    assert(tag == SKind::Block);
    return static_cast<BlockSubst&>(*desc).content;
};
Subst::AssertSubst& Subst::toAssert() {
    assert(tag == SKind::Assert);
    return static_cast<AssertSubst&>(*desc);
};
Subst::IfThenSubst& Subst::toIfThen() {
    assert(tag == SKind::IfThen);
    return static_cast<IfThenSubst&>(*desc);
};
Subst::IfThenElseSubst& Subst::toIfThenElse() {
    assert(tag == SKind::IfThenElse);
    return static_cast<IfThenElseSubst&>(*desc);
};
Subst::SimpleAssignmentSubst& Subst::toSimpleAssignment() {
    assert(tag == SKind::SimpleAssignment);
    return static_cast<SimpleAssignmentSubst&>(*desc);
};
Subst::SelectSubst& Subst::toSelect() {
    assert(tag == SKind::Select);
    return static_cast<SelectSubst&>(*desc);
};
Subst::SelectElseSubst& Subst::toSelectElse() {
    assert(tag == SKind::SelectElse);
    return static_cast<SelectElseSubst&>(*desc);
};
Subst::CaseSubst& Subst::toCase() {
    assert(tag == SKind::Case);
    return static_cast<CaseSubst&>(*desc);
};
Subst::CaseElseSubst& Subst::toCaseElse() {
    assert(tag == SKind::CaseElse);
    return static_cast<CaseElseSubst&>(*desc);
};
Subst::AnySubst& Subst::toAny() {
    assert(tag == SKind::Any);
    return static_cast<AnySubst&>(*desc);
};
Subst::OpCallSubst& Subst::toOpCall() {
    assert(tag == SKind::OperationCall);
    return static_cast<OpCallSubst&>(*desc);
};
Subst::WhileSubst& Subst::toWhile() {
    assert(tag == SKind::While);
    return static_cast<WhileSubst&>(*desc);
};
std::vector<Subst>& Subst::toSequence() {
    assert(tag == SKind::Sequence);
    return static_cast<NarySubst&>(*desc).content;
};
std::vector<Subst>& Subst::toParallel() {
    assert(tag == SKind::Parallel);
    return static_cast<NarySubst&>(*desc).content;
};
std::vector<Subst>& Subst::toChoice() {
    assert(tag == SKind::Choice);
    return static_cast<NarySubst&>(*desc).content;
};
Subst::WitnessSubst& Subst::toWitness() {
    assert(tag == SKind::Witness);
    return static_cast<WitnessSubst&>(*desc);
};

void Subst::getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
    if(desc != nullptr)
        desc->getFreeVars(boundVars,accu);
}
void Subst::getAllVars(std::set<VarName> &accu) const {
    if(desc != nullptr)
        desc->getAllVars(accu);
}

void Subst::getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
    if(desc != nullptr)
        desc->getModifiedVars(boundVars,accu);
}

std::map<VarName,Expr> Subst::SimpleAssignmentSubst::toMap() const {
    assert(vars.size() == exprs.size());
    std::map<VarName,Expr> map;
    for(int i = 0;i<vars.size();i++)
        map[vars.at(i).name] = exprs.at(i).copy();
    return map;
};

size_t Subst::hash_combine(size_t seed) const {
    if(desc != nullptr)
        seed = desc->hash_combine(seed);
    return hashUtil::hash_combine_int(static_cast<int>(tag),seed);
}

void Subst::substFreshId(const std::string &id, const VarName &v){
    if(desc != nullptr)
        desc->substFreshId(id,v);
}
