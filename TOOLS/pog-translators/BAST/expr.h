/** expr.h

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
#ifndef EXPR_H
#define EXPR_H

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <cassert>
#include <set>
#include <QStringList>
#include <cctype> // isdigit
#include "btype.h"
#include "vars.h"

class Pred;

class Expr {
    public:
        enum class EKind {
            MaxInt, MinInt, INTEGER, NATURAL, NATURAL1, INT, NAT, NAT1, STRING,
            BOOL, REAL, FLOAT, TRUE, FALSE, EmptySet, IntegerLiteral,
            StringLiteral, RealLiteral, Id, BooleanExpr, QuantifiedExpr,
            QuantifiedSet, UnaryExpr, BinaryExpr, NaryExpr, Struct, Record,
            TernaryExpr, Record_Field_Access, Record_Field_Update, Successor,
            Predecessor
        }; 

        enum class BinaryOp {
            Mapplet, Cartesian_Product, Partial_Functions, Partial_Surjections,
            Set_Difference, Total_Functions, Total_Surjections, Head_Insertion,
            Interval, Intersection, Head_Restriction, Composition, Surcharge,
            Relations, Tail_Insertion, Domain_Subtraction, Domain_Restriction,
            Partial_Injections, Total_Injections, Partial_Bijections,
            Total_Bijections, Direct_Product, Parallel_Product, Union,
            Tail_Restriction, Concatenation, Modulo, Range_Restriction,
            Range_Subtraction, Image, Application,
            IAddition, ISubtraction, IMultiplication, IDivision, IExponentiation,
            RAddition, RSubtraction, RMultiplication, RDivision, RExponentiation,
            FAddition, FSubtraction, FMultiplication, FDivision,
            Iteration, First_Projection, Second_Projection,
            Const, Rank, Father, Subtree, Arity
        };
        static std::string to_string(BinaryOp op);

        enum class TernaryOp {
            Son, Bin
        };
        static std::string to_string(TernaryOp op);

        enum class QuantifiedOp {
            Lambda, Intersection, Union, ISum, IProduct, RSum, RProduct
        };
        static std::string to_string(QuantifiedOp op);

        enum class UnaryOp {
            Cardinality, Domain, Range, Subsets, Non_Empty_Subsets, Finite_Subsets,
            Non_Empty_Finite_Subsets, Union, Intersection, IMinimum, IMaximum,
            Sequences, Non_Empty_Sequences, Injective_Sequences, Non_Empty_Injective_Sequences,
            IMinus, RMinus, Inverse, Size, Permutations, First, Last, Identity,
            Closure, Transitive_Closure, Tail, Front, Reverse, Concatenation,
            Rel, Fnc, Real, Floor, Ceiling, RMinimum, RMaximum, Tree, Btree, Top,
            Sons, Prefix, Postfix, Sizet, Mirror, Left, Right, Infix, Bin
        };
        static std::string to_string(UnaryOp op);

        enum class NaryOp {
            Sequence, Set
        };
        static std::string to_string(NaryOp op);

        Expr():
            tag{EKind::MaxInt}
        ,desc{nullptr}
        ,type{BType::INT}
        ,bxmlTag{}
        {};

        struct Decimal {
            Decimal(const std::string &integerPart, const std::string &fractionalPart):
                integerPart{integerPart},
                fractionalPart{fractionalPart}
            {
                assert(integerPart.size()>0);
                assert(fractionalPart.size()>0);
                assert(integerPart == "0" || integerPart[0] != '0');
                assert(fractionalPart == "0" || fractionalPart[fractionalPart.size()-1] != '0');
                for(int i=1;i<integerPart.size();i++)
                    assert(isdigit(integerPart[i]));
                for(int i=1;i<fractionalPart.size();i++)
                    assert(isdigit(fractionalPart[i]));
            };
            Decimal(const std::string &integerPart):
                Decimal(integerPart,"0")
            {};
            std::string integerPart;
            std::string fractionalPart;
            int compare(const Decimal &other) const;
        };

        static Expr makeInteger(const std::string &i, const QStringList &bxmlTag = {});
        static Expr makeString(const std::string &s, const QStringList &bxmlTag = {});
        static Expr makeReal(const Decimal &d, const QStringList &bxmlTag = {});
        static Expr makeIdent(const VarName &s, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeEmptySet(const BType &ty, const QStringList &bxmlTag = {});
        static Expr makePredecessor(const BType &ty, const QStringList &bxmlTag = {});
        static Expr makeSuccessor(const BType &ty, const QStringList &bxmlTag = {});
        static Expr makeMaxInt(const QStringList &bxmlTag = {});
        static Expr makeMinInt(const QStringList &bxmlTag = {});
        static Expr makeINTEGER(const QStringList &bxmlTag = {});
        static Expr makeNATURAL(const QStringList &bxmlTag = {});
        static Expr makeNATURAL1(const QStringList &bxmlTag = {});
        static Expr makeINT(const QStringList &bxmlTag = {});
        static Expr makeNAT(const QStringList &bxmlTag = {});
        static Expr makeNAT1(const QStringList &bxmlTag = {});
        static Expr makeSTRING(const QStringList &bxmlTag = {});
        static Expr makeBOOL(const QStringList &bxmlTag = {});
        static Expr makeTRUE(const QStringList &bxmlTag = {});
        static Expr makeFALSE(const QStringList &bxmlTag = {});
        static Expr makeREAL(const QStringList &bxmlTag = {});
        static Expr makeFLOAT(const QStringList &bxmlTag = {});
        static Expr makeBinaryExpr(BinaryOp op, Expr &&lhs, Expr &&rhs, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeUnaryExpr(UnaryOp op, Expr &&lhs, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeNaryExpr(NaryOp op, std::vector<Expr> &&vec, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeTernaryExpr(TernaryOp op, Expr &&fst, Expr &&snd, Expr &&thd, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeBooleanExpr(Pred &&p, const QStringList &bxmlTag = {});
        static Expr makeRecord(std::vector<std::pair<std::string,Expr>> &&fds, const BType &type, const QStringList &bxmlTag = {}); // /!\ fields must be sorted alphabetically
        static Expr makeStruct(std::vector<std::pair<std::string,Expr>> &&fds, const BType &type, const QStringList &bxmlTag = {}); // /!\ fields must be sorted alphabetically
        static Expr makeQuantifiedExpr(QuantifiedOp op,const std::vector<TypedVar> vars,
                Pred &&cond, Expr &&body, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeQuantifiedSet(const std::vector<TypedVar> vars, Pred &&cond, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeRecordFieldUpdate(Expr &&rec, const std::string &label,Expr &&value, const BType &type, const QStringList &bxmlTag = {});
        static Expr makeRecordFieldAccess(Expr &&rec, const std::string &label, const BType &type, const QStringList &bxmlTag = {});

        EKind getTag() const { return tag; };
        const BType& getType() const { return type; };
        const QStringList& getBxmlTag() const { return bxmlTag; };

        void addBxmlTags(const QStringList &bxmlTag);
        
        // Capture-avoiding substitution
        void subst(const std::map<VarName,Expr> &map);
        // Alpha renaming. The new var names must not occur (free or bound) in the expression
        void alpha(const std::map<VarName,VarName> &map);

	/** \brief Get new identifiers occurring free in expression 
	 * \param boundVars set of bound variables in the current context
	 * \param freeVars set of free variables already declared
	 * \param freeVarsThis receives the set of free variables occuring in expression that are not in freeVars.
	 */
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

        void getAllVars(std::set<VarName> &accu) const;
        // Get all identifiers (free or bound) occuring in the expression
        std::set<VarName> getAllVars() const {
            std::set<VarName> accu;
            getAllVars(accu);
            return accu;
        }

        // This function is used to instanciate Gpred's tag LetFreshId
        void substFreshId(const std::string &id, const VarName &v);

        class Visitor;
        void accept(Visitor &visitor) const;

        Expr copy() const;

        const std::string& getIntegerLiteral() const;
        const std::string& getStringLiteral() const;
        const Decimal& getRealLiteral() const;
        const VarName& getId() const;
        const std::string& getDollar0() const;

        class UnaryExpr;
        class BinaryExpr;
        class TernaryExpr;
        class NaryExpr;
        class StructExpr;
        class RecordExpr;
        class QuantifiedExpr;
        class QuantifiedSet;
        class RecordAccessExpr;
        class RecordUpdateExpr;
        class BooleanExpr;
        class IntegerLiteral;
        class RealLiteral;
        class StringLiteral;
        class IdentExpr;

        const TernaryExpr& toTernaryExpr() const;
        const BinaryExpr& toBinaryExpr() const;
        const UnaryExpr& toUnaryExpr() const;
        const NaryExpr& toNaryExpr() const;
        const Pred& toBooleanExpr() const;
        const QuantifiedSet& toQuantifiedSet() const;
        const QuantifiedExpr& toQuantiedExpr() const;
        const RecordExpr& toRecordExpr() const;
        const StructExpr& toStructExpr() const;
        const RecordAccessExpr& toRecordAccess() const;
        const RecordUpdateExpr& toRecordUpdate() const;

        TernaryExpr& toTernaryExpr();
        BinaryExpr& toBinaryExpr();
        UnaryExpr& toUnaryExpr();
        NaryExpr& toNaryExpr();
        Pred& toBooleanExpr();
        QuantifiedSet& toQuantifiedSet();
        QuantifiedExpr& toQuantiedExpr();
        RecordExpr& toRecordExpr();
        StructExpr& toStructExpr();
        RecordAccessExpr& toRecordAccess();
        RecordUpdateExpr& toRecordUpdate();

        std::string show() const; // for debug

        // Convient pour des algorithmes de classement
        // Remarque 1: le type de l'expression est pris en compte
        // Remarque 2: ne permet pas de comparer numériquement (les littéraux entiers ou décimaux)
        static int compare(const Expr& lhs, const Expr& rhs);
        static int vec_compare(const std::vector<Expr>& lhs, const std::vector<Expr>& rhs);

        // test for equality, modulo arithmetic equality:
        // compare(a,b) == 0 => equals(a, b)  
        //static bool equals(const Expr& e1, const Expr& e2);

        static bool alpha_equals(Context &ctx, const Expr& e1, const Expr& e2);
        static bool alpha_equals(const Expr& e1, const Expr& e2);

        // Attention il ne faut pas utiliser ces opérateurs pour comparer des expressions
        // numériques à des fins de simplification.
        //inline bool operator==(const Expr& other) const { return compare(*this,other) == 0; }
        //inline bool operator!=(const Expr& other) const { return compare(*this,other) != 0; }
        inline bool operator< (const Expr& other) const { return compare(*this,other) <  0; }
        //inline bool operator> (const Expr& other) const { return compare(*this,other) >  0; }
        //inline bool operator<=(const Expr& other) const { return compare(*this,other) <= 0; }
        //inline bool operator>=(const Expr& other) const { return compare(*this,other) >= 0; }

        // Remarque: le hashage ne prend pas en compte le type
        size_t hash_combine(size_t seed) const;

        // Auxilliary functions used for avoiding variable capture
        static bool isRenamingNeeded(const std::vector<TypedVar> &vars,const std::set<VarName> freeVars);
        static void renameVars(std::vector<TypedVar> &vars, const std::set<VarName> freeVars, std::map<VarName,Expr> &map2);
    private:
        class ExprDesc {
            public:
                virtual ~ExprDesc(){};
                virtual size_t hash_combine(size_t seed) const = 0;
                virtual ExprDesc* copy() const = 0;
                virtual void subst(const std::map<VarName,Expr> &map) = 0;
                virtual void alpha(const std::map<VarName,VarName> &map) = 0;
                virtual void getFreeVars(const std::set<VarName> &bv, const std::set<VarName> &freeVars, std::set<VarName>& freeVarsThis) const = 0;
                virtual void getFreeVars(const std::set<VarName> &bv, std::set<VarName> &accu) const = 0;
                virtual void getFreeTVars(const std::set<VarName> &bv, std::set<TypedVar> &accu, const BType &ty) const = 0;
                virtual void getAllVars(std::set<VarName> &accu) const = 0;
                virtual void substFreshId(const std::string &id, const VarName &v) = 0;
        };

        // Attributes
        EKind tag;  // the 'kind' of the expression. Determine the class of desc.
        std::unique_ptr<ExprDesc> desc; // the content of the expression (if any)
        BType type; // the type of the expression
        QStringList bxmlTag; // tracability tags

        // Constructor
        Expr(EKind tag,ExprDesc *desc,const BType &ty, const QStringList &bxmlTag):
            tag{tag}
        ,desc{desc}
        ,type{ty}
        ,bxmlTag{bxmlTag}
        {};
};

class Expr::Visitor {
    public:
        enum class EConstant {
            MaxInt, MinInt, INTEGER, NATURAL, NATURAL1, INT, NAT, NAT1, STRING,
            BOOL, REAL, FLOAT, TRUE, FALSE, EmptySet, Successor, Predecessor };

        virtual void visitConstant(const BType &type, const QStringList &bxmlTag, EConstant c) = 0;
        virtual void visitIdent(const BType &type, const QStringList &bxmlTag, const VarName &b) = 0;
        virtual void visitIntegerLiteral(const BType &type, const QStringList &bxmlTag, const std::string & i) = 0;
        virtual void visitStringLiteral(const BType &type, const QStringList &bxmlTag, const std::string &b) = 0;
        virtual void visitRealLiteral(const BType &type, const QStringList &bxmlTag, const Decimal &d) = 0;
        virtual void visitUnaryExpression(const BType &type, const QStringList &bxmlTag, Expr::UnaryOp op,const Expr &e) = 0;
        virtual void visitBinaryExpression(const BType &type, const QStringList &bxmlTag, Expr::BinaryOp op, const Expr &lhs, const Expr &rhs) = 0;
        virtual void visitTernaryExpression(const BType &type, const QStringList &bxmlTag, Expr::TernaryOp op, const Expr &fst, const Expr &snd, const Expr &thd) = 0;
        virtual void visitNaryExpression(const BType &type, const QStringList &bxmlTag, Expr::NaryOp op, const std::vector<Expr> &vec) = 0;
        virtual void visitBooleanExpression(const BType &type, const QStringList &bxmlTag, const Pred &p) = 0;
        virtual void visitRecord(const BType &type, const QStringList &bxmlTag, const std::vector<std::pair<std::string,Expr>> &fds) = 0;
        virtual void visitStruct(const BType &type, const QStringList &bxmlTag, const std::vector<std::pair<std::string,Expr>> &fds) = 0;
        virtual void visitQuantifiedExpr(const BType &type, const QStringList &bxmlTag, Expr::QuantifiedOp op,const std::vector<TypedVar> vars,const Pred &cond, const Expr &body) = 0;
        virtual void visitQuantifiedSet(const BType &type, const QStringList &bxmlTag, const std::vector<TypedVar> vars, const Pred &cond) = 0;
        virtual void visitRecordUpdate(const BType &type, const QStringList &bxmlTag, const Expr &rec, const std::string &label, const Expr &value) = 0;
        virtual void visitRecordAccess(const BType &type, const QStringList &bxmlTag, const Expr &rec, const std::string &label) = 0;
};

namespace std {
    template <>
        class hash<Expr> {
            public:
                size_t operator()(const Expr &e) const { return e.hash_combine(0); };
        };
}

#endif // EXPR_H
