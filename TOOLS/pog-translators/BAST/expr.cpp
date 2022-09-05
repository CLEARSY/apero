/** expr.cpp

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
#include<cassert>
#include<cstring> // strcmp
#include<utility> // swap
#include<map>

#include "exprDesc.h"
#include "predDesc.h"

Expr Expr::copy() const {
    if(desc == nullptr) return Expr(tag,nullptr,type,bxmlTag);
    else return Expr(tag,desc->copy(),type,bxmlTag);
}

void Expr::getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
    if(desc != nullptr)
        desc->getFreeVars(boundVars,freeVars,freeVarsThis);
}
void Expr::getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
    if(desc != nullptr)
        desc->getFreeVars(boundVars,accu);
}
void Expr::getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu) const {
    if(desc != nullptr)
        desc->getFreeTVars(boundVars,accu,type);
}
void Expr::substFreshId(const std::string &id, const VarName &v){
    if(desc != nullptr)
        desc->substFreshId(id,v);
}

void Expr::getAllVars(std::set<VarName> &accu) const {
    if(desc != nullptr)
        desc->getAllVars(accu);
}

class Expr::IntegerLiteral : public ExprDesc {
    public:
        IntegerLiteral(const std::string &s):value{s}{
            assert(s.size() > 0);
            if(s[0] == '-'){
                assert(s.size() > 1);
                assert(s[1] != '0');
                for(int i=1;i<s.size();i++)
                    assert(isdigit(s[i]));
            } else if (s[0] == '0'){
                assert(s.size() == 1);
            } else {
                for(int i=0;i<s.size();i++)
                    assert(isdigit(s[i]));
            }
        };
        ~IntegerLiteral(){};
        const std::string value;
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_string(value,seed);
        }
        IntegerLiteral* copy() const {
            return new IntegerLiteral(value);
        }
        void subst(const std::map<VarName,Expr> &map) {}
        void alpha(const std::map<VarName,VarName> &map) {}
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {}
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &ty) const {}
        void getAllVars(std::set<VarName> &accu) const {}
        void substFreshId(const std::string &id, const VarName &v){}
};

class Expr::StringLiteral : public ExprDesc {
    public:
        StringLiteral(const std::string &i):value{i}{};
        ~StringLiteral(){};
        const std::string value;
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_string(value,seed);
        }
        StringLiteral* copy() const {
            return new StringLiteral(value);
        }
        void subst(const std::map<VarName,Expr> &map) {}
        void alpha(const std::map<VarName,VarName> &map) {}
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {}
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &ty) const {}
        void getAllVars(std::set<VarName> &accu) const {}
        void substFreshId(const std::string &id, const VarName &v){}
};

class Expr::RealLiteral : public ExprDesc {
    public:
        RealLiteral(const Decimal &r):value{r}{};
        ~RealLiteral(){};
        const Decimal value;
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_string(value.integerPart,
                    hashUtil::hash_combine_string(value.fractionalPart,seed));
        }
        RealLiteral* copy() const {
            return new RealLiteral(value);
        }
        void subst(const std::map<VarName,Expr> &map) {}
        void alpha(const std::map<VarName,VarName> &map) {}
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {}
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &ty) const {}
        void getAllVars(std::set<VarName> &accu) const {}
        void substFreshId(const std::string &id, const VarName &v){}
};

class Expr::IdentExpr : public ExprDesc {
    public:
        IdentExpr(const VarName &v):value{v}{};
        ~IdentExpr(){};
        VarName value;
        size_t hash_combine(size_t seed) const {
            return value.hash_combine(seed);
        }
        IdentExpr* copy() const {
            return new IdentExpr(value);
        }
        void subst(const std::map<VarName,Expr> &map) {
            assert(false); // should not be called
        }
        void alpha(const std::map<VarName,VarName> &map) {
            auto it = map.find(value);
            if (it != map.end())
                value = it->second;
        }
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            if(boundVars.find(value) != boundVars.end())
                return;
            if(freeVars.find(value) != freeVars.end())
                return;
            freeVarsThis.insert(value);
	}
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            if(boundVars.find(value) == boundVars.end())
                accu.insert(value);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &ty) const {
            if(boundVars.find(value) == boundVars.end())
                accu.insert({value,ty});
        }
        void getAllVars(std::set<VarName> &accu) const {
            accu.insert(value);
        }
        void substFreshId(const std::string &id, const VarName &v){
            if(value.kind() == VarName::Kind::FreshId && value.prefix() == id)
                value = v;
        }
};

class Expr::BooleanExpr : public Expr::ExprDesc {
    public:
        BooleanExpr(Pred &&p):pred{std::move(p)}{};
        ~BooleanExpr(){};
        Pred pred;
        size_t hash_combine(size_t seed) const {
            return pred.hash_combine(seed);
        }
        BooleanExpr* copy() const {
            return new BooleanExpr(pred.copy());
        }
        void subst(const std::map<VarName,Expr> &map) {
            pred.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            pred.alpha(map);
        }
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            pred.getFreeVars(boundVars,freeVars, freeVarsThis);
        }
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            pred.getFreeVars(boundVars,accu);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &ty) const {
            pred.getFreeTVars(boundVars,accu);
        }
        void getAllVars(std::set<VarName> &accu) const {
            pred.getAllVars(accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            pred.substFreshId(id,v);
        }
};


int Expr::Decimal::compare(const Expr::Decimal &other) const {
    int cmp = integerPart.compare(other.integerPart);
    if(cmp != 0)
        return cmp;
    return fractionalPart.compare(other.fractionalPart);
}

void Expr::accept(Visitor &visitor) const {
    switch(tag){
        case EKind::INTEGER:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::INTEGER);
                break;
            }
        case EKind::NATURAL:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::NATURAL);
                break;
            }
        case EKind::NATURAL1:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::NATURAL1);
                break;
            }
        case EKind::INT:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::INT);
                break;
            }
        case EKind::MaxInt:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::MaxInt);
                break;
            }
        case EKind::MinInt:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::MinInt);
                break;
            }
        case EKind::NAT:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::NAT);
                break;
            }
        case EKind::NAT1:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::NAT1);
                break;
            }
        case EKind::TRUE:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::TRUE);
                break;
            }
        case EKind::FALSE:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::FALSE);
                break;
            }
        case EKind::BOOL:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::BOOL);
                break;
            }
        case EKind::STRING:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::STRING);
                break;
            }
        case EKind::REAL:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::REAL);
                break;
            }
        case EKind::FLOAT:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::FLOAT);
                break;
            }
        case EKind::Successor:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::Successor);
                break;
            }
        case EKind::Predecessor:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::Predecessor);
                break;
            }
        case EKind::EmptySet:
            {
                visitor.visitConstant(type,bxmlTag,Visitor::EConstant::EmptySet);
                break;
            }
        case EKind::Id:
            {
                visitor.visitIdent(type,bxmlTag,static_cast<IdentExpr&>(*desc).value);
                break;
            }
        case EKind::QuantifiedSet:
            {
                QuantifiedSet& e = static_cast<QuantifiedSet&>(*desc);
                visitor.visitQuantifiedSet(type,bxmlTag,e.vars,e.cond);
                break;
            }
        case EKind::QuantifiedExpr:
            {
                QuantifiedExpr& e = static_cast<QuantifiedExpr&>(*desc);
                visitor.visitQuantifiedExpr(type,bxmlTag,e.op,e.vars,e.cond,e.body);
                break;
            }
        case EKind::IntegerLiteral:
            {
                visitor.visitIntegerLiteral(type,bxmlTag,static_cast<IntegerLiteral&>(*desc).value);
                break;
            }
        case EKind::StringLiteral:
            {
                visitor.visitStringLiteral(type,bxmlTag,static_cast<StringLiteral&>(*desc).value);
                break;
            }
        case EKind::RealLiteral:
            {
                visitor.visitRealLiteral(type,bxmlTag,static_cast<RealLiteral&>(*desc).value);
                break;
            }
        case EKind::UnaryExpr:
            {
                UnaryExpr& e = static_cast<UnaryExpr&>(*desc);
                visitor.visitUnaryExpression(type,bxmlTag,e.op,e.content);
                break;
            }
        case EKind::BinaryExpr:
            {
                BinaryExpr& e = static_cast<BinaryExpr&>(*desc);
                visitor.visitBinaryExpression(type,bxmlTag,e.op,e.lhs,e.rhs);
                break;
            }
        case EKind::TernaryExpr:
            {
                TernaryExpr& e = static_cast<TernaryExpr&>(*desc);
                visitor.visitTernaryExpression(type,bxmlTag,e.op,e.fst,e.snd,e.thd);
                break;
            }
        case EKind::NaryExpr:
            {
                NaryExpr& e = static_cast<NaryExpr&>(*desc);
                visitor.visitNaryExpression(type,bxmlTag,e.op,e.vec);
                break;
            }
        case EKind::BooleanExpr:
            {
                visitor.visitBooleanExpression(type,bxmlTag,static_cast<BooleanExpr&>(*desc).pred);
                break;
            }
        case EKind::Struct:
            {
                visitor.visitStruct(type,bxmlTag,static_cast<StructExpr&>(*desc).fields);
                break;
            }
        case EKind::Record:
            {
                visitor.visitRecord(type,bxmlTag,static_cast<RecordExpr&>(*desc).fields);
                break;
            }
        case EKind::Record_Field_Access:
            {
                RecordAccessExpr &acc = static_cast<RecordAccessExpr&>(*desc);
                visitor.visitRecordAccess(type,bxmlTag,acc.rec,acc.label);
                break;
            }
        case EKind::Record_Field_Update:
            {
                RecordUpdateExpr &up = static_cast<RecordUpdateExpr&>(*desc);
                visitor.visitRecordUpdate(type,bxmlTag,up.rec,up.label,up.fvalue);
                break;
            }

    };
};

const std::string& Expr::getIntegerLiteral() const {
    assert(tag == EKind::IntegerLiteral);
    return static_cast<IntegerLiteral&>(*desc).value;
};

const Expr::Decimal& Expr::getRealLiteral() const {
    assert(tag == EKind::RealLiteral);
    return static_cast<RealLiteral&>(*desc).value;
};

const std::string& Expr::getStringLiteral() const {
    assert(tag == EKind::StringLiteral);
    return static_cast<StringLiteral&>(*desc).value;
};

const VarName& Expr::getId() const {
    assert(tag == EKind::Id);
    return static_cast<IdentExpr&>(*desc).value;
};

const Expr::BinaryExpr& Expr::toBinaryExpr() const{
    assert(tag == EKind::BinaryExpr);
    return static_cast<BinaryExpr&>(*desc);
};

const Expr::TernaryExpr& Expr::toTernaryExpr() const{
    assert(tag == EKind::TernaryExpr);
    return static_cast<TernaryExpr&>(*desc);
};

const Expr::UnaryExpr& Expr::toUnaryExpr() const{
    assert(tag == EKind::UnaryExpr);
    return static_cast<UnaryExpr&>(*desc);
};

const Expr::NaryExpr& Expr::toNaryExpr() const {
    assert(tag == EKind::NaryExpr);
    return static_cast<NaryExpr&>(*desc);
};

const Pred& Expr::toBooleanExpr() const {
    assert(tag == EKind::BooleanExpr);
    return static_cast<BooleanExpr&>(*desc).pred;
};

const Expr::QuantifiedSet& Expr::toQuantifiedSet() const {
    assert(tag == EKind::QuantifiedSet);
    return static_cast<QuantifiedSet&>(*desc);
};

const Expr::QuantifiedExpr& Expr::toQuantiedExpr() const {
    assert(tag == EKind::QuantifiedExpr);
    return static_cast<QuantifiedExpr&>(*desc);
};

const Expr::RecordExpr& Expr::toRecordExpr() const {
    assert(tag == EKind::Record);
    return static_cast<RecordExpr&>(*desc);
};

const Expr::StructExpr& Expr::toStructExpr() const {
    assert(tag == EKind::Struct);
    return static_cast<StructExpr&>(*desc);
};

const Expr::RecordAccessExpr& Expr::toRecordAccess() const {
    assert(tag == EKind::Record_Field_Access);
    return static_cast<RecordAccessExpr&>(*desc);
};

const Expr::RecordUpdateExpr& Expr::toRecordUpdate() const {
    assert(tag == EKind::Record_Field_Update);
    return static_cast<RecordUpdateExpr&>(*desc);
};

Expr::BinaryExpr& Expr::toBinaryExpr() {
    assert(tag == EKind::BinaryExpr);
    return static_cast<BinaryExpr&>(*desc);
};

Expr::TernaryExpr& Expr::toTernaryExpr() {
    assert(tag == EKind::TernaryExpr);
    return static_cast<TernaryExpr&>(*desc);
};

Expr::UnaryExpr& Expr::toUnaryExpr() {
    assert(tag == EKind::UnaryExpr);
    return static_cast<UnaryExpr&>(*desc);
};

Expr::NaryExpr& Expr::toNaryExpr()  {
    assert(tag == EKind::NaryExpr);
    return static_cast<NaryExpr&>(*desc);
};

Pred& Expr::toBooleanExpr()  {
    assert(tag == EKind::BooleanExpr);
    return static_cast<BooleanExpr&>(*desc).pred;
};

Expr::QuantifiedSet& Expr::toQuantifiedSet()  {
    assert(tag == EKind::QuantifiedSet);
    return static_cast<QuantifiedSet&>(*desc);
};

Expr::QuantifiedExpr& Expr::toQuantiedExpr()  {
    assert(tag == EKind::QuantifiedExpr);
    return static_cast<QuantifiedExpr&>(*desc);
};

Expr::RecordExpr& Expr::toRecordExpr()  {
    assert(tag == EKind::Record);
    return static_cast<RecordExpr&>(*desc);
};

Expr::StructExpr& Expr::toStructExpr()  {
    assert(tag == EKind::Struct);
    return static_cast<StructExpr&>(*desc);
};

Expr::RecordAccessExpr& Expr::toRecordAccess()  {
    assert(tag == EKind::Record_Field_Access);
    return static_cast<RecordAccessExpr&>(*desc);
};

Expr::RecordUpdateExpr& Expr::toRecordUpdate() {
    assert(tag == EKind::Record_Field_Update);
    return static_cast<RecordUpdateExpr&>(*desc);
};

Expr Expr::makeInteger(const std::string &i, const QStringList &bxmlTag){
    return Expr(
            EKind::IntegerLiteral,
            new IntegerLiteral(i),
            BType::INT,bxmlTag);
};

Expr Expr::makeString(const std::string &s, const QStringList &bxmlTag){
    return Expr(
            EKind::StringLiteral,
            new StringLiteral(s),
            BType::STRING,bxmlTag);
};

Expr Expr::makeReal(const Decimal &d, const QStringList &bxmlTag){
    return Expr(
            EKind::RealLiteral,
            new RealLiteral(d),
            BType::REAL,bxmlTag);
};

Expr Expr::makeIdent(const VarName &id, const BType &type, const QStringList &bxmlTag){
    return Expr(
            EKind::Id,
            new IdentExpr(id),
            type,bxmlTag);
};

Expr Expr::makePredecessor(const BType &type, const QStringList &bxmlTag){ return Expr(EKind::Predecessor,nullptr,type,bxmlTag); };
Expr Expr::makeSuccessor(const BType &type, const QStringList &bxmlTag){ return Expr(EKind::Successor,nullptr,type,bxmlTag); };
Expr Expr::makeEmptySet(const BType &type, const QStringList &bxmlTag){ return Expr(EKind::EmptySet,nullptr,type,bxmlTag); };
Expr Expr::makeMaxInt(const QStringList &bxmlTag){ return Expr(EKind::MaxInt,nullptr,BType::INT,bxmlTag); };
Expr Expr::makeMinInt(const QStringList &bxmlTag){ return Expr(EKind::MinInt,nullptr,BType::INT,bxmlTag); };
Expr Expr::makeINTEGER(const QStringList &bxmlTag){ return Expr(EKind::INTEGER,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeNATURAL(const QStringList &bxmlTag){ return Expr(EKind::NATURAL,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeNATURAL1(const QStringList &bxmlTag){ return Expr(EKind::NATURAL1,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeINT(const QStringList &bxmlTag){ return Expr(EKind::INT,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeNAT(const QStringList &bxmlTag){ return Expr(EKind::NAT,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeNAT1(const QStringList &bxmlTag){ return Expr(EKind::NAT1,nullptr,BType::POW_INT,bxmlTag); };
Expr Expr::makeSTRING(const QStringList &bxmlTag){ return Expr(EKind::STRING,nullptr,BType::POW_STRING,bxmlTag); };
Expr Expr::makeBOOL(const QStringList &bxmlTag){ return Expr(EKind::BOOL,nullptr,BType::POW_BOOL,bxmlTag); };
Expr Expr::makeTRUE(const QStringList &bxmlTag){ return Expr(EKind::TRUE,nullptr,BType::BOOL,bxmlTag); };
Expr Expr::makeFALSE(const QStringList &bxmlTag){ return Expr(EKind::FALSE,nullptr,BType::BOOL,bxmlTag); };
Expr Expr::makeREAL(const QStringList &bxmlTag){ return Expr(EKind::REAL,nullptr,BType::POW_REAL,bxmlTag); };
Expr Expr::makeFLOAT(const QStringList &bxmlTag){ return Expr(EKind::FLOAT,nullptr,BType::POW_FLOAT,bxmlTag); };

Expr Expr::makeBinaryExpr(BinaryOp op, Expr &&lhs, Expr &&rhs, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::BinaryExpr, new BinaryExpr(op,std::move(lhs),std::move(rhs)), type,bxmlTag);
};
Expr Expr::makeTernaryExpr(TernaryOp op, Expr &&fst, Expr &&snd, Expr &&thd, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::TernaryExpr, new TernaryExpr(op,std::move(fst),std::move(snd),std::move(thd)), type,bxmlTag);
};
Expr Expr::makeUnaryExpr(UnaryOp op, Expr &&e, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::UnaryExpr, new UnaryExpr(op,std::move(e)), type,bxmlTag);
};
Expr Expr::makeNaryExpr(NaryOp op, std::vector<Expr> &&vec, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::NaryExpr, new NaryExpr(op,std::move(vec)), type,bxmlTag);
};
Expr Expr::makeBooleanExpr(Pred &&p, const QStringList &bxmlTag){
    return Expr( EKind::BooleanExpr, new BooleanExpr(std::move(p)), BType::BOOL,bxmlTag);
};
Expr Expr::makeRecord(std::vector<std::pair<std::string,Expr>> &&fds, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::Record, new RecordExpr(std::move(fds)),type,bxmlTag);
};
Expr Expr::makeStruct(std::vector<std::pair<std::string,Expr>> &&fds, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::Struct, new StructExpr(std::move(fds)),type,bxmlTag);
};
Expr Expr::makeQuantifiedExpr(QuantifiedOp op,const std::vector<TypedVar> vars, Pred &&cond, Expr &&body, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::QuantifiedExpr, new QuantifiedExpr(op,vars,std::move(cond),std::move(body)),type,bxmlTag);
};
Expr Expr::makeQuantifiedSet(const std::vector<TypedVar> vars, Pred &&cond, const BType &type, const QStringList &bxmlTag){
    return Expr( EKind::QuantifiedSet, new QuantifiedSet(vars,std::move(cond)),type,bxmlTag);
};
Expr Expr::makeRecordFieldUpdate(Expr &&rec, const std::string &label, Expr &&value, const BType &type, const QStringList &bxmlTag){
    return Expr(EKind::Record_Field_Update, new RecordUpdateExpr(std::move(rec),label,std::move(value)) ,type,bxmlTag);
};
Expr Expr::makeRecordFieldAccess(Expr &&rec, const std::string &label, const BType &type, const QStringList &bxmlTag){
    return Expr(EKind::Record_Field_Access, new RecordAccessExpr(std::move(rec),label) ,type,bxmlTag);
};

void Expr::addBxmlTags(const QStringList &bxmlTag){
    this->bxmlTag << bxmlTag;
}

int compare_field_vec(const std::vector<std::pair<std::string,Expr>>& lhs, const std::vector<std::pair<std::string,Expr>>& rhs){
    if(lhs.size() == rhs.size()){
        int i = 0;
        while(i<lhs.size()){
            auto &p1 = lhs.at(i);
            auto &p2 = rhs.at(i);
            int res = p1.first.compare(p2.first);
            if(res != 0) return res;
            res = Expr::compare(p1.second,p2.second);
            if(res != 0) return res;
            i++;
        }
        return 0;
    } else {
        return (lhs.size() - rhs.size());
    }
};

int Expr::compare(const Expr& e1, const Expr& e2){
    if(e1.getTag() == e2.getTag()){
        switch(e1.getTag()){
            case Expr::EKind::INTEGER:
            case Expr::EKind::NATURAL:
            case Expr::EKind::NATURAL1:
            case Expr::EKind::INT:
            case Expr::EKind::MaxInt:
            case Expr::EKind::MinInt:
            case Expr::EKind::NAT:
            case Expr::EKind::NAT1:
            case Expr::EKind::TRUE:
            case Expr::EKind::FALSE:
            case Expr::EKind::BOOL:
            case Expr::EKind::STRING:
            case Expr::EKind::REAL:
            case Expr::EKind::FLOAT:
            case Expr::EKind::Successor:
            case Expr::EKind::Predecessor:
                return 0;
            case Expr::EKind::EmptySet:
                return BType::compare(e1.type,e2.type);
            case Expr::EKind::IntegerLiteral:
                return e1.getIntegerLiteral().compare(e2.getIntegerLiteral());
            case Expr::EKind::StringLiteral:
                return e1.getStringLiteral().compare(e2.getStringLiteral());
            case Expr::EKind::RealLiteral:
                return e1.getRealLiteral().compare(e2.getRealLiteral());
            case Expr::EKind::Id:
                return TypedVar::compare({e1.getId(),e1.type},{e2.getId(),e2.type});
            case Expr::EKind::UnaryExpr:
                {
                    auto &u1 = e1.toUnaryExpr();
                    auto &u2 = e2.toUnaryExpr();
                    if(u1.op == u2.op){
                        return compare(u1.content, u2.content);
                    } else if (u1.op < u2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::BinaryExpr:
                {
                    auto &b1 = e1.toBinaryExpr();
                    auto &b2 = e2.toBinaryExpr();
                    if(b1.op == b2.op){
                        int res = compare(b1.lhs,b2.lhs);
                        if (res == 0){
                            return compare(b1.rhs,b2.rhs);
                        } else {
                            return res;
                        }
                    } else if (b1.op < b2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::TernaryExpr:
                {
                    auto &t1 = e1.toTernaryExpr();
                    auto &t2 = e2.toTernaryExpr();
                    if(t1.op == t2.op){
                        int res = compare(t1.fst,t2.fst);
                        if (res == 0){
                            res = compare(t1.snd,t2.snd);
                            if (res == 0){
                                return compare(t1.thd,t2.thd);
                            } else {
                                return res;
                            }
                        } else {
                            return res;
                        }
                    } else if (t1.op < t2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::NaryExpr:
                {
                    auto &n1 = e1.toNaryExpr();
                    auto &n2 = e2.toNaryExpr();
                    if(n1.op == n2.op){
                        return vec_compare(n1.vec,n2.vec);
                    } else if (n1.op < n2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::BooleanExpr:
                {
                    return Pred::compare(e1.toBooleanExpr(),e2.toBooleanExpr());
                }
            case Expr::EKind::Struct:
                {
                    auto &s1 = e1.toStructExpr();
                    auto &s2 = e2.toStructExpr();
                    return compare_field_vec(s1.fields,s2.fields);
                }
            case Expr::EKind::Record:
                {
                    auto &s1 = e1.toRecordExpr();
                    auto &s2 = e2.toRecordExpr();
                    return compare_field_vec(s1.fields,s2.fields);
                }
            case Expr::EKind::QuantifiedSet:
                {
                    auto &s1 = e1.toQuantifiedSet();
                    auto &s2 = e2.toQuantifiedSet();
                    int i = TypedVar::vec_compare(s1.vars,s2.vars);
                    if(i == 0){
                        return Pred::compare(s1.cond,s2.cond);
                    } else {
                        return i;
                    }
                }
            case Expr::EKind::QuantifiedExpr:
                {
                    auto &s1 = e1.toQuantiedExpr();
                    auto &s2 = e2.toQuantiedExpr();
                    if(s1.op == s2.op){
                        int i = TypedVar::vec_compare(s1.vars,s2.vars);
                        if(i == 0){
                            i = Pred::compare(s1.cond,s2.cond);
                            if(i == 0){
                                return compare(s1.body,s2.body);
                            } else {
                                return i;
                            }
                        } else {
                            return i;
                        }
                    } else if (s1.op < s2.op){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::Record_Field_Update:
                {
                    auto &t1 = e1.toRecordUpdate();
                    auto &t2 = e2.toRecordUpdate();
                    if(t1.label == t2.label){
                        int res = compare(t1.rec,t2.rec);
                            if (res == 0){
                                return compare(t1.fvalue,t2.fvalue);
                            } else {
                                return res;
                            }
                    } else if (t1.label < t2.label){
                        return -1;
                    } else {
                        return 1;
                    }
                }
            case Expr::EKind::Record_Field_Access:
                {
                    auto &t1 = e1.toRecordAccess();
                    auto &t2 = e2.toRecordAccess();
                    if(t1.label == t2.label){
                        return compare(t1.rec,t2.rec);
                    } else if (t1.label < t2.label){
                        return -1;
                    } else {
                        return 1;
                    }
                }
        }
        assert(false); // unreachable
    }
    else if (e1.getTag() < e2.getTag()){
        return -1;
    } else {
        return 1;
    }
};

int Expr::vec_compare(const std::vector<Expr>& lhs, const std::vector<Expr>& rhs){
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
}

size_t Expr::hash_combine(size_t seed) const {
    if(desc != nullptr)
        seed = desc->hash_combine(seed);
    return hashUtil::hash_combine_int(static_cast<int>(tag),seed);
}

std::string Expr::to_string(UnaryOp op){
    switch(op){
        case UnaryOp::IMaximum: return "imax";
        case UnaryOp::IMinimum: return "imin";
        case UnaryOp::RMaximum: return "rmax";
        case UnaryOp::RMinimum: return "rmin";
        case UnaryOp::Cardinality: return "card";
        case UnaryOp::Domain: return "dom";
        case UnaryOp::Range: return "ran";
        case UnaryOp::Subsets: return "POW";
        case UnaryOp::Non_Empty_Subsets: return "POW1";
        case UnaryOp::Finite_Subsets: return "FIN";
        case UnaryOp::Non_Empty_Finite_Subsets: return "FIN1";
        case UnaryOp::Union: return "union";
        case UnaryOp::Intersection: return "inter";
        case UnaryOp::Sequences: return "seq";
        case UnaryOp::Non_Empty_Sequences: return "seq1";
        case UnaryOp::Injective_Sequences: return "iseq";
        case UnaryOp::Non_Empty_Injective_Sequences: return "iseq1";
        case UnaryOp::IMinus: return "-i";
        case UnaryOp::RMinus: return "-r";
        case UnaryOp::Inverse: return "~";
        case UnaryOp::Size: return "size";
        case UnaryOp::Permutations: return "perm";
        case UnaryOp::First: return "first";
        case UnaryOp::Last: return "last";
        case UnaryOp::Identity: return "id";
        case UnaryOp::Closure: return "closure";
        case UnaryOp::Transitive_Closure: return "closure1";
        case UnaryOp::Tail: return "tail";
        case UnaryOp::Front: return "front";
        case UnaryOp::Reverse: return "rev";
        case UnaryOp::Concatenation: return "conc";
        case UnaryOp::Rel: return "rel";
        case UnaryOp::Fnc: return "fnc";
        case UnaryOp::Real: return "real";
        case UnaryOp::Floor: return "floor";
        case UnaryOp::Ceiling: return "ceiling";
        case UnaryOp::Tree: return "tree";
        case UnaryOp::Btree: return "btree";
        case UnaryOp::Top: return "top";
        case UnaryOp::Sons: return "sons";
        case UnaryOp::Prefix: return "prefix";
        case UnaryOp::Postfix: return "postfix";
        case UnaryOp::Sizet: return "sizet";
        case UnaryOp::Mirror: return "mirror";
        case UnaryOp::Left: return "left";
        case UnaryOp::Right: return "right";
        case UnaryOp::Infix: return "infix";
        case UnaryOp::Bin: return "bin";
    };
    assert(false); // unreachable
};

std::string Expr::to_string(BinaryOp op){
    switch(op){
        case BinaryOp::IMultiplication: return "*i";
        case BinaryOp::RMultiplication: return "*r";
        case BinaryOp::FMultiplication: return "*f";
        case BinaryOp::Cartesian_Product: return "*s";
        case BinaryOp::IExponentiation: return "**i";
        case BinaryOp::RExponentiation: return "**r";
        case BinaryOp::IAddition: return "+i";
        case BinaryOp::RAddition: return "+r";
        case BinaryOp::FAddition: return "+f";
        case BinaryOp::Partial_Functions: return "+->";
        case BinaryOp::Partial_Surjections: return "+->>";
        case BinaryOp::ISubtraction: return "-i";
        case BinaryOp::RSubtraction: return "-r";
        case BinaryOp::FSubtraction: return "-f";
        case BinaryOp::Set_Difference: return "-s";
        case BinaryOp::Total_Functions: return "-->";
        case BinaryOp::Total_Surjections: return "-->>";
        case BinaryOp::Head_Insertion: return "->";
        case BinaryOp::Interval: return "..";
        case BinaryOp::IDivision: return "/i";
        case BinaryOp::RDivision: return "/r";
        case BinaryOp::FDivision: return "/f";
        case BinaryOp::Intersection: return "/\\";
        case BinaryOp::Head_Restriction: return "/|\\";
        case BinaryOp::Composition: return ";";
        case BinaryOp::Surcharge: return "<+";
        case BinaryOp::Relations: return "<->";
        case BinaryOp::Tail_Insertion: return "<-";
        case BinaryOp::Domain_Subtraction: return "<<|";
        case BinaryOp::Domain_Restriction: return "<|";
        case BinaryOp::Partial_Injections: return ">+>";
        case BinaryOp::Total_Injections: return ">->";
        case BinaryOp::Partial_Bijections: return ">+>>";
        case BinaryOp::Total_Bijections: return ">->>";
        case BinaryOp::Direct_Product: return "><";
        case BinaryOp::Parallel_Product: return "||";
        case BinaryOp::Union: return "\\/";
        case BinaryOp::Tail_Restriction: return "\\|/";
        case BinaryOp::Concatenation: return "^";
        case BinaryOp::Modulo: return "mod";
        case BinaryOp::Mapplet: return "|->";
        case BinaryOp::Range_Restriction: return "|>";
        case BinaryOp::Range_Subtraction: return "|>>";
        case BinaryOp::Image: return "[";
        case BinaryOp::Application: return "(";
        case BinaryOp::First_Projection: return "prj1";
        case BinaryOp::Second_Projection: return "prj2";
        case BinaryOp::Iteration: return "iterate";
        case BinaryOp::Const: return "const";
        case BinaryOp::Rank: return "rank";
        case BinaryOp::Father: return "father";
        case BinaryOp::Subtree: return "subtree";
        case BinaryOp::Arity: return "arity";
    };
    assert(false); // unreachable
};

std::string Expr::to_string(NaryOp op){
    switch(op){
        case NaryOp::Sequence: return "[";
        case NaryOp::Set: return "{";
    };
    assert(false); // unreachable
};

std::string Expr::to_string(TernaryOp op){
    switch(op){
        case TernaryOp::Bin: return "bin";
        case TernaryOp::Son: return "son";
    };
    assert(false); // unreachable
};

std::string Expr::to_string(QuantifiedOp op){
    switch(op){
        case QuantifiedOp::Lambda: return "%";
        case QuantifiedOp::Intersection: return "INTER";
        case QuantifiedOp::Union: return "UNION";
        case QuantifiedOp::RProduct: return "rPI";
        case QuantifiedOp::RSum: return "rSIGMA";
        case QuantifiedOp::IProduct: return "iPI";
        case QuantifiedOp::ISum: return "iSIGMA";
    }
    assert(false); // unreachable
};

std::string Expr::show() const {
    switch(tag){
        case EKind::INTEGER:
            return "INTEGER";
        case EKind::NATURAL:
            return "NATURAL";
        case EKind::NATURAL1:
            return "NATURAL1";
        case EKind::INT:
            return "INT";
        case EKind::MaxInt:
            return "MaxInt";
        case EKind::MinInt:
            return "MinInt";
        case EKind::NAT:
            return "NAT";
        case EKind::NAT1:
            return "NAT1";
        case EKind::TRUE:
            return "TRUE";
        case EKind::FALSE:
            return "FALSE";
        case EKind::BOOL:
            return "BOOL";
        case EKind::STRING:
            return "STRING";
        case EKind::REAL:
            return "REAL";
        case EKind::FLOAT:
            return "FLOAT";
        case EKind::EmptySet:
            return "{}";
        case EKind::Successor:
            return "succ";
        case EKind::Predecessor:
            return "pred";
        case EKind::IntegerLiteral:
            return getIntegerLiteral();
        case EKind::StringLiteral:
            return "\"" + getStringLiteral() + "\"";
        case EKind::RealLiteral:
            {
                auto &d = getRealLiteral();
                return d.integerPart + "." + d.fractionalPart;
            }
        case EKind::Id:
            return getId().show();
        case EKind::QuantifiedSet:
            {
                auto& q = toQuantifiedSet();
                assert(q.vars.size()>0);
                auto vars = q.vars[0].name.show();
                for(int i=1;i<q.vars.size();++i)
                    vars += " " + q.vars[i].name.show();
                return "(QSet (" + vars + ") " + q.cond.show() + ")";
            }
        case EKind::QuantifiedExpr:
            {
                auto& q = toQuantiedExpr();
                assert(q.vars.size()>0);
                auto vars = q.vars[0].name.show();
                for(int i=1;i<q.vars.size();++i)
                    vars += " " + q.vars[i].name.show();
                return "(" + to_string(q.op) + " (" + vars + ") "
                    + q.cond.show() + " " + q.body.show() + ")";
            }
        case EKind::UnaryExpr:
            {
                auto& u = toUnaryExpr();
                return "(" + to_string(u.op) + " " + u.content.show() +")";
            }
        case EKind::BinaryExpr:
            {
                auto& b = toBinaryExpr();
                return "(" + to_string(b.op) + " " + b.lhs.show()
                    + " " + b.rhs.show() + ")";
            }
        case EKind::TernaryExpr:
            {
                auto& t = toTernaryExpr();
                return "(" + to_string(t.op) + " " + t.fst.show()
                    + " " + t.snd.show() + " " + t.thd.show() + ")";
            }
        case EKind::NaryExpr:
            {
                auto& n = toNaryExpr();
                std::string accu;
                for(auto &e : n.vec)
                    accu += " " + e.show();
                switch(n.op){
                    case Expr::NaryOp::Sequence:
                        return "(Seq" + accu + ")";
                    case Expr::NaryOp::Set:
                        return "(Set" + accu + ")";
                }
                assert(false); // unreachable
            }
        case EKind::BooleanExpr:
            return "(bool " + toBooleanExpr().show() + ")";
        case EKind::Struct:
            {
                std::string accu;
                for(auto &pair : toStructExpr().fields)
                    accu += " (" + pair.first + " " + pair.second.show() + ")";
                return "(struct" + accu + ")";
            }
        case EKind::Record:
            {
                std::string accu;
                for(auto &pair : toRecordExpr().fields)
                    accu += " (" + pair.first + " " + pair.second.show() + ")";
                return "(rec" + accu + ")";
            }
        case EKind::Record_Field_Access:
            {
                auto &a = toRecordAccess();
                return "(recordAccess " + a.rec.show() + " " + a.label + ")";
            }
        case EKind::Record_Field_Update:
            {
                auto &u = toRecordUpdate();
                return "(recordUpdate " + u.rec.show() + " " + u.label
                    + " " + u.fvalue.show() + ")";
            }
    };
    assert(false); // unreachable
}

void Expr::subst(const std::map<VarName,Expr> &map) {
    if(!map.empty()){
       switch(tag){
           case EKind::MaxInt:
           case EKind::MinInt:
           case EKind::INTEGER:
           case EKind::NATURAL:
           case EKind::NATURAL1:
           case EKind::INT:
           case EKind::NAT:
           case EKind::NAT1:
           case EKind::STRING:
           case EKind::BOOL:
           case EKind::REAL:
           case EKind::FLOAT:
           case EKind::TRUE:
           case EKind::FALSE:
           case EKind::EmptySet:
           case EKind::IntegerLiteral:
           case EKind::StringLiteral:
           case EKind::RealLiteral:
           case EKind::Successor:
           case EKind::Predecessor:
               return;
           case EKind::Id:
               {
                   auto it = map.find(static_cast<IdentExpr&>(*desc).value);
                   if(it != map.end()){
                       tag = it->second.tag;
                       //type = it->second.type;
                       bxmlTag << it->second.bxmlTag;
                       if(it->second.desc != nullptr)
                           desc = std::unique_ptr<ExprDesc>(it->second.desc->copy());
                       else
                           desc = nullptr;
                   }
                   return;
               }
           case EKind::BooleanExpr:
           case EKind::QuantifiedExpr:
           case EKind::QuantifiedSet:
           case EKind::UnaryExpr:
           case EKind::BinaryExpr:
           case EKind::NaryExpr:
           case EKind::Struct:
           case EKind::Record:
           case EKind::TernaryExpr:
           case EKind::Record_Field_Access:
           case EKind::Record_Field_Update:
               return desc->subst(map);
       }
       assert(false); // unreachable
    }
};

void Expr::alpha(const std::map<VarName,VarName> &map) {
    if(desc != nullptr)
        desc->alpha(map);
};
bool Expr::isRenamingNeeded(const std::vector<TypedVar> &vars,const std::set<VarName> freeVars){
    for(auto &v : vars){
        if(freeVars.find(v.name) != freeVars.end())
            return true;
    }
    return false;
}
void Expr::renameVars(std::vector<TypedVar> &vars, const std::set<VarName> freeVars, std::map<VarName,Expr> &map2){
    for(int i=0;i<vars.size();i++){
        VarName nv = VarName::getFreshVar(vars[i].name.prefix(),freeVars);
        map2[vars[i].name] = Expr::makeIdent(nv,vars[i].type);
        vars[i].name = nv;
    }
}

bool Expr::alpha_equals(Context &ctx, const Expr& e1, const Expr& e2){
    if(e1.getType() != e2.getType())
        return false;

    if(e1.getTag() == e2.getTag()){
        switch(e1.getTag()){
            case Expr::EKind::INTEGER:
            case Expr::EKind::NATURAL:
            case Expr::EKind::NATURAL1:
            case Expr::EKind::INT:
            case Expr::EKind::MaxInt:
            case Expr::EKind::MinInt:
            case Expr::EKind::NAT:
            case Expr::EKind::NAT1:
            case Expr::EKind::TRUE:
            case Expr::EKind::FALSE:
            case Expr::EKind::BOOL:
            case Expr::EKind::STRING:
            case Expr::EKind::REAL:
            case Expr::EKind::FLOAT:
            case Expr::EKind::EmptySet:
            case Expr::EKind::Predecessor:
            case Expr::EKind::Successor:
            //case Expr::EKind::EmptySeq:
                return true;
            case Expr::EKind::IntegerLiteral:
                return e1.getIntegerLiteral() == e2.getIntegerLiteral();
            case Expr::EKind::StringLiteral:
                return e1.getStringLiteral() == e2.getStringLiteral();
            case Expr::EKind::RealLiteral:
                {
                    auto &r1 = e1.getRealLiteral();
                    auto &r2 = e2.getRealLiteral();
                    return r1.integerPart == r2.integerPart
                        && r1.fractionalPart == r2.fractionalPart;
                }
            case Expr::EKind::Id:
                {
                    return ctx.equals( e1.getId(),e2.getId());
                }
            case Expr::EKind::UnaryExpr:
                {
                    auto &u1 = e1.toUnaryExpr();
                    auto &u2 = e2.toUnaryExpr();
                    return u1.op == u2.op
                        && alpha_equals(ctx,u1.content,u2.content);
                }
            case Expr::EKind::BinaryExpr:
                {
                    auto &b1 = e1.toBinaryExpr();
                    auto &b2 = e2.toBinaryExpr();
                    return b1.op == b2.op
                        && alpha_equals(ctx,b1.lhs,b2.lhs)
                        && alpha_equals(ctx,b1.rhs,b2.rhs);
                }
            case Expr::EKind::TernaryExpr:
                {
                    auto &t1 = e1.toTernaryExpr();
                    auto &t2 = e2.toTernaryExpr();
                    return t1.op == t2.op
                        && alpha_equals(ctx,t1.fst,t2.fst)
                        && alpha_equals(ctx,t1.snd,t2.snd)
                        && alpha_equals(ctx,t1.thd,t2.thd);
                }
            case Expr::EKind::NaryExpr:
                {
                    auto &n1 = e1.toNaryExpr();
                    auto &n2 = e2.toNaryExpr();
                    if(n1.op != n2.op)
                        return false;
                    if(n1.vec.size() != n2.vec.size())
                        return false;
                    for(int i=0;i<n1.vec.size();i++){
                        if(!alpha_equals(ctx,n1.vec[i],n2.vec[i]))
                            return false;
                    }
                    return true;
                }
            case Expr::EKind::BooleanExpr:
                {
                    return Pred::alpha_equals(ctx,e1.toBooleanExpr(),e2.toBooleanExpr());
                }
            case Expr::EKind::Struct:
                {
                    auto &s1 = e1.toStructExpr();
                    auto &s2 = e2.toStructExpr();
                    if(s1.fields.size() != s2.fields.size())
                        return false;
                    for(int i=0;i<s1.fields.size();i++){
                        if(s1.fields[i].first != s2.fields[i].first)
                            return false;
                        if(!alpha_equals(ctx,s1.fields[i].second,s2.fields[i].second))
                            return false;
                    }
                    return true;
                }
            case Expr::EKind::Record:
                {
                    auto &s1 = e1.toRecordExpr();
                    auto &s2 = e2.toRecordExpr();
                    if(s1.fields.size() != s2.fields.size())
                        return false;
                    for(int i=0;i<s1.fields.size();i++){
                        if(s1.fields[i].first != s2.fields[i].first)
                            return false;
                        if(!alpha_equals(ctx,s1.fields[i].second,s2.fields[i].second))
                            return false;
                    }
                    return true;
                }
            case Expr::EKind::QuantifiedSet:
                {
                    auto &s1 = e1.toQuantifiedSet();
                    auto &s2 = e2.toQuantifiedSet();
                    if(!ctx.push(s1.vars,s2.vars))
                        return false;
                    bool res = Pred::alpha_equals(ctx,s1.cond,s2.cond);
                    ctx.pop();
                    return res;
                }
            case Expr::EKind::QuantifiedExpr:
                {
                    auto &s1 = e1.toQuantiedExpr();
                    auto &s2 = e2.toQuantiedExpr();
                    if(s1.op != s2.op)
                        return false;
                    if(!ctx.push(s1.vars,s2.vars))
                        return false;
                    bool res = s1.op == s2.op
                      &&  Pred::alpha_equals(ctx,s1.cond,s2.cond)
                      &&  alpha_equals(ctx,s1.body,s2.body);
                    ctx.pop();
                    return res;
                }
            case Expr::EKind::Record_Field_Update:
                {
                    auto &t1 = e1.toRecordUpdate();
                    auto &t2 = e2.toRecordUpdate();
                    return t1.label == t2.label
                        && alpha_equals(ctx,t1.fvalue,t2.fvalue)
                        && alpha_equals(ctx,t1.rec,t2.rec);
                }
            case Expr::EKind::Record_Field_Access:
                {
                    auto &t1 = e1.toRecordAccess();
                    auto &t2 = e2.toRecordAccess();
                    return t1.label == t2.label
                        && alpha_equals(ctx,t1.rec,t2.rec);
                }
        }
        assert(false); // unreachable
    } else {
        return false;
    }

}

bool Expr::alpha_equals(const Expr& e1, const Expr& e2){
    Context ctx;
    return alpha_equals(ctx,e1,e2);
}
