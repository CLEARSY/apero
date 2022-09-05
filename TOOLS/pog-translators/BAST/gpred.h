/** gpred.h

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
#ifndef GPred_H
#define GPred_H

#include <memory>
#include "pred.h"
#include "subst.h"
#include "hashutil.h"

class GPred {
public:
    enum class Kind { Implication, Equivalence, ExprComparison, Negation, Conjunction,
        Disjunction, Forall, Exists, TaggedPred, Sub, NotSubNot, LetFreshId };
    class ImplicationPred;
    class EquivalencePred;
    class ExprComparison;
    class NegationPred;
    class ConjunctionPred;
    class DisjunctionPred;
    class ForallPred;
    class ExistsPred;
    class TaggedPred;
    class Sub;
    class NotSubNot;
    class LetFreshId;
    Kind getKind() const;
    const ImplicationPred& toImplication() const;
    const EquivalencePred& toEquivalence() const;
    const ExprComparison& toExprComparison() const;
    const NegationPred& toNegationPred() const;
    const ConjunctionPred& toConjunction() const;
    const DisjunctionPred& toDisjunction() const;
    const ForallPred& toForall() const;
    const ExistsPred& toExists() const;
    const TaggedPred& toTaggedPred() const;
    const Sub& toSub() const;
    const NotSubNot& toNotSubNot() const;
    const LetFreshId& toLetFreshId() const;
    ImplicationPred& toImplication();
    EquivalencePred& toEquivalence();
    ExprComparison& toExprComparison();
    NegationPred& toNegationPred();
    ConjunctionPred& toConjunction();
    DisjunctionPred& toDisjunction();
    ForallPred& toForall();
    ExistsPred& toExists();
    TaggedPred& toTaggedPred();
    Sub& toSub();
    NotSubNot& toNotSubNot();
    LetFreshId& toLetFreshId();

    size_t hash_combine(size_t seed) const;
    class Visitor {
        public:
        virtual void visitImplication(const ImplicationPred &e) = 0;
        virtual void visitEquivalence(const EquivalencePred &e) = 0;
        virtual void visitExprComparison(const ExprComparison &e) = 0;
        virtual void visitNegationPred(const NegationPred &e) = 0;
        virtual void visitConjunction(const ConjunctionPred &e) = 0;
        virtual void visitDisjunction(const DisjunctionPred &e) = 0;
        virtual void visitForall(const ForallPred &e) = 0;
        virtual void visitExists(const ExistsPred &e) = 0;
        virtual void visitTaggedPred(const TaggedPred &e) = 0;
        virtual void visitSub(const Sub &e) = 0;
        virtual void visitNotSubNot(const NotSubNot &e) = 0;
        virtual void visitLetFreshId(const LetFreshId &e) = 0;
    };
    void accept(Visitor &v) const;
    static GPred makeImplication(GPred &&lhs, GPred &&rhs);
    static GPred makeEquivalence(GPred &&lhs, GPred &&rhs);
    static GPred makeExprComparison(Pred::ComparisonOp op, Expr &&lhs, Expr &&rhs);
    static GPred makeNegationPred(GPred &&content);
    static GPred makeConjunction(std::vector<GPred> &&content);
    static GPred makeDisjunction(std::vector<GPred> &&content);
    static GPred makeForall(const std::vector<TypedVar> &vars, GPred &&body);
    static GPred makeExists(const std::vector<TypedVar> &vars, GPred &&body);
    static GPred makeTaggedPred(const std::string &tag, GPred &&p);
    static GPred makeSub(Subst &&subst, GPred &&pred, bool overflow);
    static GPred makeNotSubNot(Subst &&subst, Pred &&pred);
    static GPred makeLetFreshId(const std::string &id, GPred &&pred);

    // Get all identifiers (free or bound) occuring in the expression
    void getAllVars(std::set<VarName> &accu) const;
    std::set<VarName> getAllVars() const {
        std::set<VarName> accu;
        getAllVars(accu);
        return accu;
    }
    // Used to instanciate LetFreshId's
    void substFreshId(const std::string &id, const VarName &v);

private:
    class AbstractGPred {
        public:
            virtual Kind getKind() const = 0;
            virtual void accept(Visitor &v) const = 0;
            virtual size_t hash_combine(size_t seed) const = 0;
            virtual void getAllVars(std::set<VarName> &accu) const = 0;
            virtual void substFreshId(const std::string &id, const VarName &v) = 0;
    };
    std::unique_ptr<AbstractGPred> ptr;
    GPred(AbstractGPred *ptr):ptr{ptr}{};
};

class GPred::ImplicationPred : public AbstractGPred {
    public:
        ImplicationPred(GPred &&lhs, GPred &&rhs):lhs{std::move(lhs)},rhs{std::move(rhs)}{};
        void accept(Visitor &v) const { v.visitImplication(*this); }
        Kind getKind() const { return Kind::Implication; }
        size_t hash_combine(size_t seed) const {
            return lhs.hash_combine( rhs.hash_combine(seed));
        }
        GPred lhs;
        GPred rhs;
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars();
            rhs.getAllVars();
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
};
class GPred::EquivalencePred : public AbstractGPred {
    public:
        EquivalencePred(GPred &&lhs, GPred &&rhs):lhs{std::move(lhs)},rhs{std::move(rhs)}{};
        void accept(Visitor &v) const { v.visitEquivalence(*this); }
        Kind getKind() const { return Kind::Equivalence; }
        size_t hash_combine(size_t seed) const {
            return lhs.hash_combine(rhs.hash_combine(seed));
        }
        GPred lhs;
        GPred rhs;
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars();
            rhs.getAllVars();
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
};
class GPred::ExprComparison : public AbstractGPred {
    public:
        ExprComparison(Pred::ComparisonOp op, Expr &&lhs, Expr &&rhs):op{op},lhs{std::move(lhs)},rhs{std::move(rhs)}{};
        void accept(Visitor &v) const { v.visitExprComparison(*this); }
        Kind getKind() const { return Kind::ExprComparison; }
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_int(static_cast<int>(op),
                    lhs.hash_combine(
                        rhs.hash_combine(seed)));
        }
        const Pred::ComparisonOp op;
        Expr lhs;
        Expr rhs;
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars();
            rhs.getAllVars();
        }
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
};
class GPred::NegationPred : public AbstractGPred {
    public:
        NegationPred(GPred &&content):content{std::move(content)}{};
        void accept(Visitor &v) const { v.visitNegationPred(*this); }
        Kind getKind() const { return Kind::Negation; }
        size_t hash_combine(size_t seed) const {
            return content.hash_combine(seed);
        }
        GPred content;
        void getAllVars(std::set<VarName> &accu) const {
            content.getAllVars();
        }
        void substFreshId(const std::string &id, const VarName &v){
            content.substFreshId(id,v);
        }
};
class GPred::ConjunctionPred : public AbstractGPred {
    public:
        ConjunctionPred(std::vector<GPred> &&content):content{std::move(content)}{};
        void accept(Visitor &v) const { v.visitConjunction(*this); }
        Kind getKind() const { return Kind::Conjunction; }
        size_t hash_combine(size_t seed) const {
            for(auto &p : content)
                seed = p.hash_combine(seed);
            return seed;
        }
        std::vector<GPred> content;
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p : content)
                p.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p : content)
                p.substFreshId(id,v);
        }
};
class GPred::DisjunctionPred : public AbstractGPred {
    public:
        DisjunctionPred(std::vector<GPred> &&content):content{std::move(content)}{};
        void accept(Visitor &v) const { v.visitDisjunction(*this); }
        Kind getKind() const { return Kind::Disjunction; }
        size_t hash_combine(size_t seed) const {
            for(auto &p : content)
                seed = p.hash_combine(seed);
            return seed;
        }
        std::vector<GPred> content;
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &p : content)
                p.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &p : content)
                p.substFreshId(id,v);
        }
};
class GPred::ForallPred : public AbstractGPred {
    public:
        ForallPred(const std::vector<TypedVar> &vars, GPred &&body):vars{vars},body{std::move(body)}{};
        void accept(Visitor &v) const { v.visitForall(*this); }
        Kind getKind() const { return Kind::Forall; }
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return body.hash_combine(seed);
        }
        std::vector<TypedVar> vars;
        GPred body;
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
};
class GPred::ExistsPred : public AbstractGPred {
    public:
        ExistsPred(const std::vector<TypedVar> &vars, GPred &&body):vars{vars},body{std::move(body)}{};
        void accept(Visitor &v) const { v.visitExists(*this); }
        Kind getKind() const { return Kind::Exists; }
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return body.hash_combine(seed);
        }
        std::vector<TypedVar> vars;
        GPred body;
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
};
class GPred::TaggedPred : public AbstractGPred {
    public:
        TaggedPred(const std::string &tag, GPred &&p):tag{tag},content{std::move(p)}{};
        void accept(Visitor &v) const { v.visitTaggedPred(*this); }
        Kind getKind() const { return Kind::TaggedPred; }
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_string(tag,
                    content.hash_combine(seed));
        }
        const std::string tag;
        GPred content;
        void getAllVars(std::set<VarName> &accu) const {
            content.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            content.substFreshId(id,v);
        }
};
class GPred::Sub : public AbstractGPred {
    public:
        Sub(Subst &&subst, GPred &&pred, bool overflow):sub{std::move(subst)},pred{std::move(pred)},overflow{overflow}{};
        void accept(Visitor &v) const { v.visitSub(*this); }
        Kind getKind() const { return Kind::Sub; }
        size_t hash_combine(size_t seed) const {
            return pred.hash_combine(
                    sub.hash_combine(seed));
        }
        Subst sub;
        GPred pred;
        const bool overflow;
        void getAllVars(std::set<VarName> &accu) const {
            sub.getAllVars(accu);
            pred.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            sub.substFreshId(id,v);
            pred.substFreshId(id,v);
        }
};
class GPred::NotSubNot : public AbstractGPred {
    public:
        NotSubNot(Subst &&subst, Pred &&pred):sub{std::move(subst)},pred{std::move(pred)}{};
        void accept(Visitor &v) const { v.visitNotSubNot(*this); }
        Kind getKind() const { return Kind::NotSubNot; }
        size_t hash_combine(size_t seed) const {
            return pred.hash_combine(
                    sub.hash_combine(seed));
        }
        Subst sub;
        Pred pred;
        void getAllVars(std::set<VarName> &accu) const {
            sub.getAllVars(accu);
            pred.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            sub.substFreshId(id,v);
            pred.substFreshId(id,v);
        }
};
class GPred::LetFreshId : public AbstractGPred {
    public:
        LetFreshId(const std::string &id, GPred &&pred):pred{std::move(pred)},id{id}{};
        void accept(Visitor &v) const { v.visitLetFreshId(*this); }
        Kind getKind() const { return Kind::LetFreshId; }
        size_t hash_combine(size_t seed) const {
            return pred.hash_combine(hashUtil::hash_combine_string(id,seed));
        }
        GPred pred;
        const std::string id;
        void getAllVars(std::set<VarName> &accu) const {
            pred.getAllVars(accu);
        }
        void substFreshId(const std::string &id2, const VarName &v){
            if(id != id2)
                pred.substFreshId(id2,v);
        }
};

namespace std {
    template <>
        class hash<GPred> {
            public:
                size_t operator()(const Pred &p) const { return p.hash_combine(0); };
        };
}
#endif
