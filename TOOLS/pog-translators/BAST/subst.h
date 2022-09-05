/** subst.h

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
#ifndef SUBST_H
#define SUBST_H

#include "expr.h"
#include "pred.h"
#include "hashutil.h"

class Subst {
    public:
        enum class SKind { 
            Block, Skip, Assert, IfThen, IfThenElse, Select,
            SelectElse, Case, CaseElse, Any, OperationCall,
            While, Sequence, Parallel, Choice, SimpleAssignment, Witness };

        Subst():tag{SKind::Skip},desc{}{}
        SKind getTag() const { return tag; };

        struct CaseChoice;

        static Subst makeSkip();
        static Subst makeBlock(Subst &&s);
        static Subst makeAssert(Pred &&p, Subst &&s);
        static Subst makeIfThen(Pred &&cond, Subst &&s);
        static Subst makeIfThenElse(Pred &&cond, Subst &&s_if, Subst &&s_else);
        static Subst makeSimpleAssignment(const std::vector<TypedVar> &variables, std::vector<Expr> &&values);
        static Subst makeSelect(std::vector<std::pair<Pred,Subst>> &&clauses);
        static Subst makeSelectElse(std::vector<std::pair<Pred,Subst>> &&clauses, Subst &&els);
        static Subst makeCase(Expr &&e, std::vector<CaseChoice> &&cases);
        static Subst makeCaseElse(Expr &&e, std::vector<CaseChoice> &&cases, Subst &&els);
        static Subst makeAny(const std::vector<TypedVar> &vars, Pred &&p, Subst &&body);
        static Subst makeWitness(std::map<std::string,Expr> &&witnesses, Subst &&body);
        static Subst makeOpCall(const std::string &name, std::vector<Expr> &&input, const std::vector<TypedVar> &output,
                const std::vector<TypedVar> &op_input, const std::vector<TypedVar> &op_output, Pred &&op_precondition, Subst &&op_body);
        static Subst makeWhile(Pred &&cond, Subst &&body, Pred &&inv, Expr &&var);
        static Subst makeSequence(std::vector<Subst> &&vec);
        static Subst makeParallel(std::vector<Subst> &&vec);
        static Subst makeChoice(std::vector<Subst> &&vec);

        class Visitor {
            public:
                virtual void visitSkip() = 0;
                virtual void visitBlock(const Subst &s) = 0;
                virtual void visitAssert(const Pred &p, const Subst &s) = 0;
                virtual void visitIfThen(const Pred &p, const Subst &s) = 0;
                virtual void visitIfThenElse(const Pred &p, const Subst &s_if, const Subst &s_else) = 0;
                virtual void visitSimpleAssignment(const std::vector<TypedVar> &variables, const std::vector<Expr> &values) = 0;
                virtual void visitSelect(const std::vector<std::pair<Pred,Subst>> &clauses) = 0;
                virtual void visitSelectElse(const std::vector<std::pair<Pred,Subst>> &clauses, const Subst &els) = 0;
                virtual void visitCase(const Expr &e, const std::vector<CaseChoice> &cases) = 0;
                virtual void visitCaseElse(const Expr &e, const std::vector<CaseChoice> &cases, const Subst &els) = 0;
                virtual void visitAny(const std::vector<TypedVar> &vars, const Pred &p, const Subst &body) = 0;
                virtual void visitOpCall(const std::string &name,  const std::vector<Expr> &input, const std::vector<TypedVar> &output,
                        const std::vector<TypedVar> &op_input, const std::vector<TypedVar> &op_output, const Pred &op_precondition, const Subst &op_body) = 0;
                virtual void visitWhile(const Pred &cond, const Subst &body, const Pred &inv, const Expr &var) = 0;
                virtual void visitSequence(const std::vector<Subst> &vec) = 0;
                virtual void visitParallel(const std::vector<Subst> &vec) = 0;
                virtual void visitChoice(const std::vector<Subst> &vec) = 0;
                virtual void visitWitness(const std::map<std::string,Expr> &witnesses,const Subst &body) = 0;
        };
        void accept(Visitor &visitor) const;

        size_t hash_combine(size_t seed) const;

        class AssertSubst;
        class IfThenSubst;
        class IfThenElseSubst;
        class SimpleAssignmentSubst;
        class SelectSubst;
        class SelectElseSubst;
        class CaseSubst;
        class CaseElseSubst;
        class AnySubst;
        class OpCallSubst;
        class WhileSubst;
        class WitnessSubst;
        class SubstDesc;
        class BlockSubst;
        class NarySubst;

        const Subst& toBlock() const;
        Subst& toBlock();
        const AssertSubst& toAssert() const;
        AssertSubst& toAssert();
        const IfThenSubst& toIfThen() const;
        IfThenSubst& toIfThen();
        const IfThenElseSubst& toIfThenElse() const;
        IfThenElseSubst& toIfThenElse();
        const SimpleAssignmentSubst& toSimpleAssignment() const;
        SimpleAssignmentSubst& toSimpleAssignment();
        const SelectSubst& toSelect() const;
        SelectSubst& toSelect();
        const SelectElseSubst& toSelectElse() const;
        SelectElseSubst& toSelectElse();
        const CaseSubst& toCase() const;
        CaseSubst& toCase();
        const CaseElseSubst& toCaseElse() const;
        CaseElseSubst& toCaseElse();
        const AnySubst& toAny() const;
        AnySubst& toAny();
        const OpCallSubst& toOpCall() const;
        OpCallSubst& toOpCall();
        const WhileSubst& toWhile() const;
        WhileSubst& toWhile();
        const WitnessSubst& toWitness() const;
        WitnessSubst& toWitness();
        const std::vector<Subst>& toSequence() const;
        std::vector<Subst>& toSequence();
        const std::vector<Subst>& toParallel() const;
        std::vector<Subst>& toParallel();
        const std::vector<Subst>& toChoice() const;
        std::vector<Subst>& toChoice();

        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const;
        // Get identifiers occuring free in the substitution
        std::set<VarName> getFreeVars() const {
            std::set<VarName> accu;
            getFreeVars({},accu);
            return accu;
        };
        void getAllVars(std::set<VarName> &accu) const;
        // Get all identifiers (free or bound) occuring in the substitution
        std::set<VarName> getAllVars() const {
            std::set<VarName> accu;
            getAllVars(accu);
            return accu;
        };
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const;
        // Get all identifiers (possibly) modified by the substitution
        std::set<TypedVar> getModifiedVars() const {
            std::set<TypedVar> accu;
            getModifiedVars({},accu);
            return accu;
        };

        // Alpha renaming. The new var names must not occur (free or bound) in the substitution
        void alpha(const std::map<VarName,VarName> &map);

        // Get all identifiers occuring free in the body (or precondition) of the operations called in the substitution.
        void getInnerFreeVars(std::set<VarName> &accu) const;

        // This function is used to instanciate Gpred's tag LetFreshId
        void substFreshId(const std::string &id, const VarName &v);

        Subst copy() const;
    private:
        SKind tag; // The 'kind' of the substitution. Determine the class of desc
        std::unique_ptr<SubstDesc> desc; // the content of the substitution (may be null if the substitution is Skip).
        // Constructor
        Subst(SKind tag,SubstDesc *desc):tag{tag},desc{desc}{};
};

struct Subst::CaseChoice {
    std::vector<Expr> values;
    Subst body;
};

class Subst::SubstDesc {
    public:
        virtual ~SubstDesc(){};
        virtual size_t hash_combine(size_t seed) const = 0;
        virtual void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const = 0;
        virtual void getAllVars(std::set<VarName> &accu) const = 0;
        virtual void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const = 0;
        virtual void alpha(const std::map<VarName,VarName> &map) = 0;
        virtual void getInnerFreeVars(std::set<VarName> &accu) const = 0;
        virtual void substFreshId(const std::string &id, const VarName &v) = 0;
        virtual SubstDesc* copy() const = 0;
};

class Subst::AssertSubst : public SubstDesc {
    public:
        // Constructor
        AssertSubst(Pred &&c, Subst &&s):condition{std::move(c)},content{std::move(s)}{};
        // Members
        Pred condition;
        Subst content;
        // Methods
        size_t hash_combine(size_t seed) const {
            return condition.hash_combine(content.hash_combine(seed));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            condition.getFreeVars(boundVars,accu);
            content.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            condition.getAllVars(accu);
            content.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            content.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            condition.alpha(map);
            content.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            content.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            condition.substFreshId(id,v);
            content.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new AssertSubst(condition.copy(),content.copy());
        }
};

class Subst::WitnessSubst : public SubstDesc {
    public:
        // Constructor
        WitnessSubst(std::map<std::string,Expr> &&witnesses, Subst &&body)
            :body{std::move(body)},witnesses{std::move(witnesses)}{};
        // Members
        std::map<std::string,Expr> witnesses;
        Subst body;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &p : witnesses)
                seed = hashUtil::hash_combine_string(p.first,
                        p.second.hash_combine(seed));
            return body.hash_combine(seed);
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &p : witnesses)
                p.second.getFreeVars(boundVars,accu);
            body.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p : witnesses)
                p.second.getAllVars(accu);
            body.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            body.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(auto &p : witnesses)
                p.second.alpha(map);
            body.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            body.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p : witnesses)
                p.second.substFreshId(id,v);
            body.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::map<std::string,Expr> witnesses2;
            for(auto &p : witnesses)
                witnesses2[p.first] = p.second.copy();
            return new WitnessSubst(std::move(witnesses2),body.copy());
        }
};

class Subst::IfThenSubst : public SubstDesc {
    public:
        // Constructor
        IfThenSubst(Pred &&c, Subst &&s):
            condition{std::move(c)},s_if{std::move(s)}{};
        // Members
        Pred condition;
        Subst s_if;
        // Methods
        size_t hash_combine(size_t seed) const {
            return condition.hash_combine(s_if.hash_combine(seed));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            condition.getFreeVars(boundVars,accu);
            s_if.getFreeVars(boundVars,accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            s_if.getModifiedVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            condition.getAllVars(accu);
            s_if.getAllVars(accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            condition.alpha(map);
            s_if.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            s_if.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            condition.substFreshId(id,v);
            s_if.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new IfThenSubst(condition.copy(),s_if.copy());
        }
};

class Subst::IfThenElseSubst : public SubstDesc {
    public:
        // Constructor
        IfThenElseSubst(Pred &&c, Subst &&s_if, Subst &&s_else):
            condition{std::move(c)},s_if{std::move(s_if)},s_else{std::move(s_else)}{};
        // Members
        Pred condition;
        Subst s_if;
        Subst s_else;
        // Methods
        size_t hash_combine(size_t seed) const {
            return condition.hash_combine(s_if.hash_combine(s_else.hash_combine(seed)));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            condition.getFreeVars(boundVars,accu);
            s_if.getFreeVars(boundVars,accu);
            s_else.getFreeVars(boundVars,accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            s_if.getModifiedVars(boundVars,accu);
            s_else.getModifiedVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            condition.getAllVars(accu);
            s_if.getAllVars(accu);
            s_else.getAllVars(accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            condition.alpha(map);
            s_if.alpha(map);
            s_else.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            s_if.getInnerFreeVars(accu);
            s_else.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            condition.substFreshId(id,v);
            s_if.substFreshId(id,v);
            s_else.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new IfThenElseSubst(condition.copy(),s_if.copy(),s_else.copy());
        }
};

class Subst::SimpleAssignmentSubst : public SubstDesc {
    public:
        // Constructor
        SimpleAssignmentSubst(const std::vector<TypedVar> &vars, std::vector<Expr> &&exprs):
            vars{vars},exprs{std::move(exprs)}{};
        // Members
        std::vector<TypedVar> vars;
        std::vector<Expr> exprs;
        // Methods
        std::map<VarName,Expr> toMap() const;
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            for(auto &e : exprs)
                seed = e.hash_combine(seed);
            return seed;
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &v : vars){
                if(boundVars.find(v.name) != boundVars.end())
                    accu.insert(v.name);
            }
            for(auto &e : exprs)
                e.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            for(auto &e : exprs)
                e.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &v : vars){
                if(boundVars.find(v.name) == boundVars.end())
                    accu.insert(v);
            }
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(int i=0;i<vars.size();i++){
                auto it = map.find(vars[i].name);
                if(it != map.end())
                    vars[i].name = it->second;
            }
            for(auto &e : exprs)
                e.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const { }
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            for(auto &e : exprs)
                e.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::vector<Expr> exprs2;
            for(auto &e : exprs)
                exprs2.push_back(e.copy());
            return new SimpleAssignmentSubst(vars,std::move(exprs2));
        }
};

class Subst::SelectSubst : public SubstDesc {
    public:
        // Constructor
        SelectSubst(std::vector<std::pair<Pred,Subst>> &&clauses):
            clauses{std::move(clauses)}{};
        // Members
        std::vector<std::pair<Pred,Subst>> clauses;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &p : clauses)
                seed = p.first.hash_combine(p.second.hash_combine(seed));
            return seed;
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &p : clauses){
                p.first.getFreeVars(boundVars,accu);
                p.second.getFreeVars(boundVars,accu);
            }
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p : clauses){
                p.first.getAllVars(accu);
                p.second.getAllVars(accu);
            }
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &p : clauses)
                p.second.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(auto &p : clauses){
                p.first.alpha(map);
                p.second.alpha(map);
            }
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            for(auto &p : clauses)
                p.second.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p : clauses){
                p.first.substFreshId(id,v);
                p.second.substFreshId(id,v);
            }
        }
        SubstDesc* copy() const {
            std::vector<std::pair<Pred,Subst>> clauses2;
            for(auto &p : clauses)
                clauses2.push_back({p.first.copy(),p.second.copy()});
            return new SelectSubst(std::move(clauses2));
        }
};

class Subst::SelectElseSubst : public SubstDesc {
    public:
        // Constructor
        SelectElseSubst(std::vector<std::pair<Pred,Subst>> &&clauses, Subst &&els):
            clauses{std::move(clauses)},s_else{std::move(els)}{};
        // Members
        std::vector<std::pair<Pred,Subst>> clauses;
        Subst s_else;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &p : clauses)
                seed = p.first.hash_combine(p.second.hash_combine(seed));
            return s_else.hash_combine(seed);
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &p : clauses){
                p.first.getFreeVars(boundVars,accu);
                p.second.getFreeVars(boundVars,accu);
            }
            s_else.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p : clauses){
                p.first.getAllVars(accu);
                p.second.getAllVars(accu);
            }
            s_else.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &p : clauses)
                p.second.getModifiedVars(boundVars,accu);
            s_else.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(auto &p : clauses){
                p.first.alpha(map);
                p.second.alpha(map);
            }
            s_else.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            for(auto &p : clauses)
                p.second.getInnerFreeVars(accu);
            s_else.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p : clauses){
                p.first.substFreshId(id,v);
                p.second.substFreshId(id,v);
            }
            s_else.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::vector<std::pair<Pred,Subst>> clauses2;
            for(auto &p : clauses)
                clauses2.push_back({p.first.copy(),p.second.copy()});
            return new SelectElseSubst(std::move(clauses2),s_else.copy());
        }
};

class Subst::CaseSubst : public SubstDesc {
    public:
        // Constructor
        CaseSubst(Expr &&e, std::vector<CaseChoice> &&cases):
            e{std::move(e)}, cases{std::move(cases)}{};
        // Members
        Expr e;
        std::vector<CaseChoice> cases;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &ch : cases){
                seed = ch.body.hash_combine(seed);
                for(auto &e : ch.values)
                    seed = e.hash_combine(seed);
            }
            return e.hash_combine(seed);
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            e.getFreeVars(boundVars,accu);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.getFreeVars(boundVars,accu);
                ch.body.getFreeVars(boundVars,accu);
            }
        }
        void getAllVars(std::set<VarName> &accu) const {
            e.getAllVars(accu);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.getAllVars(accu);
                ch.body.getAllVars(accu);
            }
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &ch: cases)
                ch.body.getModifiedVars(boundVars,accu);
        }

        void alpha(const std::map<VarName,VarName> &map){
            e.alpha(map);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.alpha(map);
                ch.body.alpha(map);
            }
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            for(auto &ch: cases)
                ch.body.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            e.substFreshId(id,v);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.substFreshId(id,v);
                ch.body.substFreshId(id,v);
            }
        }
        SubstDesc* copy() const {
            std::vector<CaseChoice> cases2;
            for(auto &ch: cases){
                CaseChoice ch2;
                for(auto &e : ch.values)
                    ch2.values.push_back(e.copy());
                ch2.body = ch.body.copy();
                cases2.push_back(std::move(ch2));
            }
            return new CaseSubst(e.copy(),std::move(cases2));
        }
};

class Subst::CaseElseSubst : public SubstDesc {
    public:
        // Constructor
        CaseElseSubst(Expr &&e, std::vector<CaseChoice> &&cases, Subst &&els):
            e{std::move(e)}, cases{std::move(cases)},s_else{std::move(els)} {};
        // Members
        Expr e;
        std::vector<CaseChoice> cases;
        Subst s_else;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &ch : cases){
                for(auto &e : ch.values)
                    seed = e.hash_combine(seed);
                seed = ch.body.hash_combine(seed);
            }
            return e.hash_combine(s_else.hash_combine(seed));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            e.getFreeVars(boundVars,accu);
            for(auto &ch: cases){
                for(auto &e :ch.values)
                    e.getFreeVars(boundVars,accu);
                ch.body.getFreeVars(boundVars,accu);
            }
            s_else.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            e.getAllVars(accu);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.getAllVars(accu);
                ch.body.getAllVars(accu);
            }
            s_else.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &ch: cases)
                ch.body.getModifiedVars(boundVars,accu);
            s_else.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            e.alpha(map);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.alpha(map);
                ch.body.alpha(map);
            }
            s_else.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            for(auto &ch: cases)
                ch.body.getInnerFreeVars(accu);
            s_else.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            e.substFreshId(id,v);
            for(auto &ch: cases){
                for(auto &e : ch.values)
                    e.substFreshId(id,v);
                ch.body.substFreshId(id,v);
            }
            s_else.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::vector<CaseChoice> cases2;
            for(auto &ch: cases){
                CaseChoice ch2;
                for(auto &e : ch.values)
                    ch2.values.push_back(e.copy());
                ch2.body = ch.body.copy();
                cases2.push_back(std::move(ch2));
            }
            return new CaseElseSubst(e.copy(),std::move(cases2),s_else.copy());
        }
};

class Subst::AnySubst : public SubstDesc {
    public:
        // Constructor
        AnySubst(const std::vector<TypedVar> &vars, Pred &&p, Subst &&body):
            vars{vars},p{std::move(p)},body{std::move(body)} {};
        // Members
        std::vector<TypedVar> vars;
        Pred p;
        Subst body;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return p.hash_combine(body.hash_combine(seed));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            p.getFreeVars(boundVars2,accu);
            body.getFreeVars(boundVars2,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            p.getAllVars(accu);
            body.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            body.getModifiedVars(boundVars2,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            std::map<VarName,VarName> map2 { map };
            for(auto &v : vars){
                if(map.find(v.name) != map.end())
                    map2.erase(v.name);
            }
            if(!map2.empty()){
                p.alpha(map2);
                body.alpha(map2);
            }
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            body.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            p.substFreshId(id,v);
            body.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new AnySubst(vars,p.copy(),body.copy());
        }
};

class Subst::OpCallSubst : public SubstDesc {
    public:
        // Constructor
        OpCallSubst(const std::string &name,
                std::vector<Expr> &&input,
                const std::vector<TypedVar> &output,
                const std::vector<TypedVar> &op_input,
                const std::vector<TypedVar> &op_output,
                Pred &&op_precondition,
                Subst &&op_body):
            name{name},
            input{std::move(input)},
            output{output},
            op_input{op_input},
            op_output{op_output},
            op_precondition{std::move(op_precondition)},
            op_body{std::move(op_body)}
        {};
        // Members
        std::string name;
        std::vector<Expr> input;
        std::vector<TypedVar> output;

        std::vector<TypedVar> op_input;
        std::vector<TypedVar> op_output;
        Pred op_precondition;
        Subst op_body;

        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &e : input)
                seed = e.hash_combine(seed);
            for(auto &v : output)
                seed = v.hash_combine(seed);
            for(auto &v : op_input)
                seed = v.hash_combine(seed);
            for(auto &v : op_output)
                seed = v.hash_combine(seed);
            return hashUtil::hash_combine_string(name,
                    op_precondition.hash_combine(
                        op_body.hash_combine(seed)));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &v : output){
                if(boundVars.find(v.name) != boundVars.end())
                    accu.insert(v.name);
            }
            for(auto &e : input)
                e.getFreeVars(boundVars,accu);
            std::set<VarName> boundVars2;
            for(auto &v : op_input)
                boundVars2.insert(v.name);
            for(auto &v : op_output)
                boundVars2.insert(v.name);
            op_precondition.getFreeVars(boundVars2,accu);
            op_body.getFreeVars(boundVars2,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : output)
                accu.insert(v.name);
            for(auto &e : input)
                e.getAllVars(accu);
            for(auto &v : op_input)
                accu.insert(v.name);
            for(auto &v : op_output)
                accu.insert(v.name);
            op_precondition.getAllVars(accu);
            op_body.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            for(auto &v : output){
                if(boundVars.find(v.name) == boundVars.end())
                    accu.insert(v);
            }
            std::set<VarName> boundVars2;
            for(auto &v : op_input)
                boundVars2.insert(v.name);
            for(auto &v : op_output)
                boundVars2.insert(v.name);
            op_body.getModifiedVars(boundVars2,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            for(int i=0;i<output.size();i++){
                auto it = map.find(output[i].name);
                if(it != map.end())
                    output[i].name = it->second;
            }
            for(auto &e : input)
                e.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            std::set<VarName> boundVars;
            for(auto &v : op_input)
                boundVars.insert(v.name);
            for(auto &v : op_output)
                boundVars.insert(v.name);
            op_precondition.getFreeVars(boundVars,accu);
            op_body.getFreeVars(boundVars,accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<output.size();i++){
                if(output[i].name.kind() == VarName::Kind::FreshId && output[i].name.prefix() == id)
                    output[i].name = v;
            }
            for(auto &e : input)
                e.substFreshId(id,v);
            for(int i=0;i<op_output.size();i++){
                if(op_output[i].name.kind() == VarName::Kind::FreshId && op_output[i].name.prefix() == id)
                    op_output[i].name = v;
            }
            for(int i=0;i<op_input.size();i++){
                if(op_input[i].name.kind() == VarName::Kind::FreshId && op_input[i].name.prefix() == id)
                    op_input[i].name = v;
            }
            op_precondition.substFreshId(id,v);
            op_body.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            std::vector<Expr> input2;
            for(auto &e : input)
                input2.push_back(e.copy());
            return new OpCallSubst(name,std::move(input2),output,op_input,op_output,op_precondition.copy(),op_body.copy());
        }
};

class Subst::WhileSubst : public SubstDesc {
    public:
        // Constructor
        WhileSubst(Pred &&cond, Subst &&body, Pred &&inv, Expr &&var):
            cond{std::move(cond)},body{std::move(body)},
            inv{std::move(inv)},var{std::move(var)}{};
        // Members
        Pred cond;
        Subst body;
        Pred inv;
        Expr var;
        // Methods
        size_t hash_combine(size_t seed) const {
            return cond.hash_combine(
                    body.hash_combine(
                        inv.hash_combine(
                            var.hash_combine(seed))));
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            cond.getFreeVars(boundVars,accu);
            body.getFreeVars(boundVars,accu);
            inv.getFreeVars(boundVars,accu);
            var.getFreeVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            cond.getAllVars(accu);
            body.getAllVars(accu);
            inv.getAllVars(accu);
            var.getAllVars(accu);
        }
        void getModifiedVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
            body.getModifiedVars(boundVars,accu);
        }
        void alpha(const std::map<VarName,VarName> &map){
            cond.alpha(map);
            body.alpha(map);
            inv.alpha(map);
            var.alpha(map);
        }
        void getInnerFreeVars(std::set<VarName> &accu) const {
            body.getInnerFreeVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            cond.substFreshId(id,v);
            body.substFreshId(id,v);
            inv.substFreshId(id,v);
            var.substFreshId(id,v);
        }
        SubstDesc* copy() const {
            return new WhileSubst(cond.copy(),body.copy(),inv.copy(),var.copy());
        }
};

namespace std {
    template <>
        class hash<Subst> {
            public:
                size_t operator()(const Subst &s) const { return s.hash_combine(0); };
        };
}

#endif // SUBST_H
