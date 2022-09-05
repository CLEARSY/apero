/** gpred.cpp

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
#include <cassert>
#include "gpred.h"

GPred::Kind GPred::getKind() const { return ptr->getKind(); };
void GPred::accept(Visitor &v) const { ptr->accept(v); };
const GPred::ImplicationPred& GPred::toImplication() const{
  assert(getKind() == Kind::Implication);
  return static_cast<ImplicationPred&>(*ptr);
}
const GPred::EquivalencePred& GPred::toEquivalence() const{
  assert(getKind() == Kind::Equivalence);
  return static_cast<EquivalencePred&>(*ptr);
}
const GPred::ExprComparison& GPred::toExprComparison() const{
  assert(getKind() == Kind::ExprComparison);
  return static_cast<ExprComparison&>(*ptr);
}
const GPred::NegationPred& GPred::toNegationPred() const{
  assert(getKind() == Kind::Negation);
  return static_cast<NegationPred&>(*ptr);
}
const GPred::ForallPred& GPred::toForall() const{
  assert(getKind() == Kind::Forall);
  return static_cast<ForallPred&>(*ptr);
}
const GPred::ExistsPred& GPred::toExists() const{
  assert(getKind() == Kind::Exists);
  return static_cast<ExistsPred&>(*ptr);
}
const GPred::ConjunctionPred& GPred::toConjunction() const{
  assert(getKind() == Kind::Conjunction);
  return static_cast<ConjunctionPred&>(*ptr);
}
const GPred::DisjunctionPred& GPred::toDisjunction() const{
  assert(getKind() == Kind::Disjunction);
  return static_cast<DisjunctionPred&>(*ptr);
}
const GPred::TaggedPred& GPred::toTaggedPred() const{
  assert(getKind() == Kind::TaggedPred);
  return static_cast<TaggedPred&>(*ptr);
}
const GPred::Sub& GPred::toSub() const{
  assert(getKind() == Kind::Sub);
  return static_cast<Sub&>(*ptr);
}
const GPred::NotSubNot& GPred::toNotSubNot() const{
  assert(getKind() == Kind::NotSubNot);
  return static_cast<NotSubNot&>(*ptr);
}
const GPred::LetFreshId& GPred::toLetFreshId() const{
  assert(getKind() == Kind::LetFreshId);
  return static_cast<LetFreshId&>(*ptr);
}
GPred::ImplicationPred& GPred::toImplication() {
  assert(getKind() == Kind::Implication);
  return static_cast<ImplicationPred&>(*ptr);
}
GPred::EquivalencePred& GPred::toEquivalence() {
  assert(getKind() == Kind::Equivalence);
  return static_cast<EquivalencePred&>(*ptr);
}
GPred::ExprComparison& GPred::toExprComparison() {
  assert(getKind() == Kind::ExprComparison);
  return static_cast<ExprComparison&>(*ptr);
}
GPred::NegationPred& GPred::toNegationPred() {
  assert(getKind() == Kind::Negation);
  return static_cast<NegationPred&>(*ptr);
}
GPred::ForallPred& GPred::toForall() {
  assert(getKind() == Kind::Forall);
  return static_cast<ForallPred&>(*ptr);
}
GPred::ExistsPred& GPred::toExists() {
  assert(getKind() == Kind::Exists);
  return static_cast<ExistsPred&>(*ptr);
}
GPred::ConjunctionPred& GPred::toConjunction() {
  assert(getKind() == Kind::Conjunction);
  return static_cast<ConjunctionPred&>(*ptr);
}
GPred::DisjunctionPred& GPred::toDisjunction() {
  assert(getKind() == Kind::Disjunction);
  return static_cast<DisjunctionPred&>(*ptr);
}
GPred::TaggedPred& GPred::toTaggedPred() {
  assert(getKind() == Kind::TaggedPred);
  return static_cast<TaggedPred&>(*ptr);
}
GPred::Sub& GPred::toSub() {
  assert(getKind() == Kind::Sub);
  return static_cast<Sub&>(*ptr);
}
GPred::NotSubNot& GPred::toNotSubNot() {
  assert(getKind() == Kind::NotSubNot);
  return static_cast<NotSubNot&>(*ptr);
}
GPred::LetFreshId& GPred::toLetFreshId() {
  assert(getKind() == Kind::LetFreshId);
  return static_cast<LetFreshId&>(*ptr);
}
GPred GPred::makeImplication(GPred &&lhs, GPred &&rhs){ return GPred(new ImplicationPred(std::move(lhs),std::move(rhs))); };
GPred GPred::makeEquivalence(GPred &&lhs, GPred &&rhs){ return GPred(new EquivalencePred(std::move(lhs),std::move(rhs))); };
GPred GPred::makeExprComparison(Pred::ComparisonOp op, Expr &&lhs, Expr &&rhs){ return GPred(new ExprComparison(op,std::move(lhs),std::move(rhs))); };
GPred GPred::makeNegationPred(GPred &&content){ return GPred(new NegationPred(std::move(content))); };
GPred GPred::makeConjunction(std::vector<GPred> &&content){ return GPred(new ConjunctionPred(std::move(content))); };
GPred GPred::makeDisjunction(std::vector<GPred> &&content){ return GPred(new DisjunctionPred(std::move(content))); };
GPred GPred::makeForall(const std::vector<TypedVar> &vars, GPred &&body){ return GPred(new ForallPred(vars,std::move(body))); };
GPred GPred::makeExists(const std::vector<TypedVar> &vars, GPred &&body){ return GPred(new ExistsPred(vars,std::move(body))); };
GPred GPred::makeTaggedPred(const std::string &tag, GPred &&p){ return GPred(new TaggedPred(tag,std::move(p))); };
GPred GPred::makeSub(Subst &&subst, GPred &&pred, bool overflow){ return GPred(new Sub(std::move(subst),std::move(pred),overflow)); };
GPred GPred::makeNotSubNot(Subst &&subst, Pred &&pred){ return GPred(new NotSubNot(std::move(subst),std::move(pred))); };
GPred GPred::makeLetFreshId(const std::string &id, GPred &&pred){ return GPred(new LetFreshId(id,std::move(pred))); };

size_t GPred::hash_combine(size_t seed) const {
    return hashUtil::hash_combine_int(static_cast<int>(getKind()),ptr->hash_combine(seed));
}
void GPred::getAllVars(std::set<VarName> &accu) const {
    ptr->getAllVars(accu);
}
void GPred::substFreshId(const std::string &id, const VarName &v){
    ptr->substFreshId(id,v);
}
