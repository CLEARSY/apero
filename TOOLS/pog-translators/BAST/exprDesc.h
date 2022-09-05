/** exprDesc.h

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
#ifndef EXPRDESC_H
#define EXPRDESC_H

#include "expr.h"
#include "pred.h"
#include "hashutil.h"
#include <map>

class Expr::UnaryExpr : public Expr::ExprDesc {
    public:
        // Constructor
        UnaryExpr(UnaryOp op, Expr &&e):
            op{op},
            content{std::move(e)}
        {};
        // Members
        const UnaryOp op;
        Expr content;
        // Methods
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_int(static_cast<int>(op),
                    content.hash_combine(seed));
        }
        UnaryExpr* copy() const {
            return new UnaryExpr(op,content.copy());
        }
        void subst(const std::map<VarName,Expr> &map) {
            content.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            content.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            content.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            content.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            content.getFreeVars(boundVars,accu);
        };
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            content.getFreeTVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            content.substFreshId(id,v);
        }
};

class Expr::BinaryExpr : public Expr::ExprDesc {
    public:
        // Constructor
        BinaryExpr(BinaryOp op, Expr &&lhs, Expr &&rhs):
            op{op},
            lhs{std::move(lhs)},
            rhs{std::move(rhs)}
        {};
        // Members
        const BinaryOp op;
        Expr lhs;
        Expr rhs;
        // Methods
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_int(static_cast<int>(op),
                    lhs.hash_combine(
                        rhs.hash_combine(seed)));
        }
        BinaryExpr* copy() const {
            return new BinaryExpr(op,lhs.copy(),rhs.copy());
        }
        void subst(const std::map<VarName,Expr> &map) {
            lhs.subst(map);
            rhs.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            lhs.alpha(map);
            rhs.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            lhs.getAllVars(accu);
            rhs.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            lhs.getFreeVars(boundVars,freeVars,freeVarsThis);
            rhs.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            lhs.getFreeVars(boundVars,accu);
            rhs.getFreeVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            lhs.substFreshId(id,v);
            rhs.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            lhs.getFreeTVars(boundVars,accu);
            rhs.getFreeTVars(boundVars,accu);
        };
};

class Expr::TernaryExpr : public Expr::ExprDesc {
    public:
        // Constructor
        TernaryExpr(TernaryOp op, Expr &&fst, Expr &&snd, Expr &&thd)
            :op{op},
            fst{std::move(fst)},
            snd{std::move(snd)},
            thd{std::move(thd)}
        {};
        // Members
        const TernaryOp op;
        Expr fst;
        Expr snd;
        Expr thd;
        // Methods
        size_t hash_combine(size_t seed) const {
            return hashUtil::hash_combine_int(static_cast<int>(op),
                    fst.hash_combine(
                        snd.hash_combine(
                            thd.hash_combine(seed))));
        }
        TernaryExpr* copy() const {
            return new TernaryExpr(op,fst.copy(),snd.copy(),thd.copy());
        }
        void subst(const std::map<VarName,Expr> &map) {
            fst.subst(map);
            snd.subst(map);
            thd.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            fst.alpha(map);
            snd.alpha(map);
            thd.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            fst.getAllVars(accu);
            snd.getAllVars(accu);
            thd.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            fst.getFreeVars(boundVars,freeVars,freeVarsThis);
            snd.getFreeVars(boundVars,freeVars,freeVarsThis);
            thd.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            fst.getFreeVars(boundVars,accu);
            snd.getFreeVars(boundVars,accu);
            thd.getFreeVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            fst.substFreshId(id,v);
            snd.substFreshId(id,v);
            thd.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            fst.getFreeTVars(boundVars,accu);
            snd.getFreeTVars(boundVars,accu);
            thd.getFreeTVars(boundVars,accu);
        };
};

class Expr::NaryExpr : public Expr::ExprDesc {
    public:
        // Constructor
        NaryExpr(NaryOp op, std::vector<Expr> &&vec):
            op{op},
            vec{std::move(vec)}
        {};
        // Members
        const NaryOp op;
        std::vector<Expr> vec;
        // Methods
        size_t hash_combine(size_t seed) const {
            seed = hashUtil::hash_combine_int(static_cast<int>(op),seed);
            for(auto &e : vec)
                seed = e.hash_combine(seed);
            return seed;
        }
        NaryExpr* copy() const {
            std::vector<Expr> vec2;
            for(auto &p : vec)
                vec2.push_back(p.copy());
            return new NaryExpr(op,std::move(vec2));
        }
        void subst(const std::map<VarName,Expr> &map) {
            for(auto &e : vec)
                e.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            for(auto &e : vec)
                e.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &e: vec)
                e.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            for(auto &e: vec)
                e.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &e: vec)
                e.getFreeVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &e: vec)
                e.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            for(auto &e: vec)
                e.getFreeTVars(boundVars,accu);
        };
};

struct {
    bool operator()(const std::pair<std::string,Expr> &a, const std::pair<std::string,Expr> &b)
    {
        return a.first < b.first;
    }
} RecordFieldCmp;

class Expr::RecordExpr : public Expr::ExprDesc {
    public:
        // Constructor
        RecordExpr(std::vector<std::pair<std::string,Expr>> &&fds):
            fields{std::move(fds)}
        {
            assert(std::is_sorted(fds.begin(),fds.end(),RecordFieldCmp));
        };
        // Members
        std::vector<std::pair<std::string,Expr>> fields;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &p : fields)
                seed = hashUtil::hash_combine_string(p.first,
                        p.second.hash_combine(seed));
            return seed;
        }
        RecordExpr* copy() const {
            std::vector<std::pair<std::string,Expr>> fields2;
            for(auto &p : fields)
                fields2.push_back({p.first,p.second.copy()});
            return new RecordExpr(std::move(fields2));
        }
        void subst(const std::map<VarName,Expr> &map) {
            for(auto &p : fields)
                p.second.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            for(auto &p : fields)
                p.second.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &pair: fields)
                pair.second.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            for(auto &pair: fields)
                pair.second.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &pair: fields)
                pair.second.getFreeVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &pair: fields)
                pair.second.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            for(auto &pair: fields)
                pair.second.getFreeTVars(boundVars,accu);
        };
};

class Expr::StructExpr : public Expr::ExprDesc {
    public:
        // Constructor
        StructExpr(std::vector<std::pair<std::string,Expr>> &&fds):
            fields{std::move(fds)}
        {
            assert(std::is_sorted(fds.begin(),fds.end(),RecordFieldCmp));
        };
        // Members
        std::vector<std::pair<std::string,Expr>> fields;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &p : fields)
                seed = hashUtil::hash_combine_string(p.first,
                        p.second.hash_combine(seed));
            return seed;
        }
        StructExpr* copy() const {
            std::vector<std::pair<std::string,Expr>> fields2;
            for(auto &p : fields)
                fields2.push_back({p.first,p.second.copy()});
            return new StructExpr(std::move(fields2));
        }
        void subst(const std::map<VarName,Expr> &map) {
            for(auto &p : fields)
                p.second.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            for(auto &p : fields)
                p.second.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &pair: fields)
                pair.second.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            for(auto &pair: fields)
                pair.second.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            for(auto &pair: fields)
                pair.second.getFreeVars(boundVars,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            for(auto &pair: fields)
                pair.second.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            for(auto &pair: fields)
                pair.second.getFreeTVars(boundVars,accu);
        };
};

class Expr::QuantifiedSet : public Expr::ExprDesc {
    public:
        // Constructor
        QuantifiedSet(const std::vector<TypedVar> &vars, Pred &&p):
            vars{vars},
            cond{std::move(p)}
        {};
        // Members
        std::vector<TypedVar> vars;
        Pred cond;
        // Methods
        size_t hash_combine(size_t seed) const {
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return cond.hash_combine(seed);
        }
        QuantifiedSet* copy() const {
            return new QuantifiedSet(vars,cond.copy());
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
                cond.getAllVars(freeVars);
                Expr::renameVars(vars,freeVars,map2);
                cond.subst(map2);
            } else if(!map2.empty()){
                cond.subst(map2);
            }
        }
        void alpha(const std::map<VarName,VarName> &map) {
            std::map<VarName,VarName> map2 { map };
            for(auto &v : vars){
                if(map.find(v.name) != map.end())
                    map2.erase(v.name);
            }
            if(!map2.empty())
                cond.alpha(map2);
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            cond.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeVars(boundVars2,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeVars(boundVars2,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            cond.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeTVars(boundVars2,accu);
        };
};
class Expr::QuantifiedExpr : public Expr::ExprDesc {
    public:
        // Constructor
        QuantifiedExpr(QuantifiedOp op, const std::vector<TypedVar> &vars, Pred &&cond, Expr &&body):
            op{op},
            vars{vars},
            cond{std::move(cond)},
            body{std::move(body)}
        {};
        // Members
        const QuantifiedOp op;
        std::vector<TypedVar> vars;
        Pred cond;
        Expr body;
        // Methods
        size_t hash_combine(size_t seed) const {
            seed = hashUtil::hash_combine_int(static_cast<int>(op),seed);
            for(auto &v : vars)
                seed = v.hash_combine(seed);
            return cond.hash_combine(
                    body.hash_combine(seed));
        }
        QuantifiedExpr* copy() const {
            return new QuantifiedExpr(op,vars,cond.copy(),body.copy());
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
                cond.getAllVars(freeVars);
                body.getAllVars(freeVars);
                Expr::renameVars(vars,freeVars,map2);
                cond.subst(map2);
                body.subst(map2);
            } else if(!map2.empty()){
                cond.subst(map2);
                body.subst(map2);
            }
        }
        void alpha(const std::map<VarName,VarName> &map) {
            std::map<VarName,VarName> map2 { map };
            for(auto &v : vars){
                if(map.find(v.name) != map.end())
                    map2.erase(v.name);
            }
            if(!map2.empty()){
                cond.alpha(map2);
                body.alpha(map2);
            }
        }
        void getAllVars(std::set<VarName> &accu) const {
            for(auto &v : vars)
                accu.insert(v.name);
            cond.getAllVars(accu);
            body.getAllVars(accu);
        };
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeVars(boundVars2,freeVars,freeVarsThis);
            body.getFreeVars(boundVars2,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeVars(boundVars2,accu);
            body.getFreeVars(boundVars2,accu);
        };
        void substFreshId(const std::string &id, const VarName &v){
            for(int i=0;i<vars.size();i++){
                if(vars[i].name.kind() == VarName::Kind::FreshId && vars[i].name.prefix() == id)
                    vars[i].name = v;
            }
            cond.substFreshId(id,v);
            body.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            std::set<VarName> boundVars2 { boundVars };
            for(auto &v : vars)
                boundVars2.insert(v.name);
            cond.getFreeTVars(boundVars2,accu);
            body.getFreeTVars(boundVars2,accu);
        };
};

class Expr::RecordAccessExpr : public Expr::ExprDesc {
    public:
        // Constructor
        RecordAccessExpr(Expr &&e, const std::string &label):
            rec{std::move(e)},
            label{label}
        {};
        // Members
        Expr rec;
        const std::string label;
        // Methods
        size_t hash_combine(size_t seed) const {
            return rec.hash_combine(hashUtil::hash_combine_string(label,seed));
        }
        RecordAccessExpr* copy() const {
            return new RecordAccessExpr(rec.copy(),label);
        }
        void subst(const std::map<VarName,Expr> &map) {
            rec.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            rec.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            rec.getAllVars(accu);
        }
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            rec.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            rec.getFreeVars(boundVars,accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            rec.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            rec.getFreeTVars(boundVars,accu);
        };
};

class Expr::RecordUpdateExpr : public Expr::ExprDesc {
    public:
        // Constructor
        RecordUpdateExpr(Expr &&rec, const std::string &label, Expr &&fval):
            rec{std::move(rec)},
            label{label},
            fvalue{std::move(fval)}
        {};
        // Members
        Expr rec;
        const std::string label;
        Expr fvalue;
        // Methods
        size_t hash_combine(size_t seed) const {
            return rec.hash_combine(fvalue.hash_combine
                    (hashUtil::hash_combine_string(label,seed)));
        }
        RecordUpdateExpr* copy() const {
            return new RecordUpdateExpr(rec.copy(),label,fvalue.copy());
        }
        void subst(const std::map<VarName,Expr> &map) {
            rec.subst(map);
            fvalue.subst(map);
        }
        void alpha(const std::map<VarName,VarName> &map) {
            rec.alpha(map);
            fvalue.alpha(map);
        }
        void getAllVars(std::set<VarName> &accu) const {
            rec.getAllVars(accu);
            fvalue.getAllVars(accu);
        }
        void getFreeVars(const std::set<VarName> &boundVars, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const {
            rec.getFreeVars(boundVars,freeVars,freeVarsThis);
            fvalue.getFreeVars(boundVars,freeVars,freeVarsThis);
        };
        void getFreeVars(const std::set<VarName> &boundVars, std::set<VarName> &accu) const {
            rec.getFreeVars(boundVars,accu);
            fvalue.getFreeVars(boundVars,accu);
        }
        void substFreshId(const std::string &id, const VarName &v){
            rec.substFreshId(id,v);
            fvalue.substFreshId(id,v);
        }
        void getFreeTVars(const std::set<VarName> &boundVars, std::set<TypedVar> &accu, const BType &) const {
            rec.getFreeTVars(boundVars,accu);
            fvalue.getFreeTVars(boundVars,accu);
        };
};

#endif
