/** exprReader.cpp

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
#include "exprReader.h"
#include "predReader.h"
#include "exprDesc.h"
#include <map>
#include <utility>

namespace Xml {
    TypedVar VarNameFromId(const QDomElement &id, const std::vector<BType> &typeInfos){
        if(id.tagName() == "Id"){
            QString prefix = id.attribute("value","");
            if(prefix == "")
                throw ExprReaderException("value attribute is empty.",id.lineNumber());
            QString typref_s = id.attribute("typref","");
            if(typref_s == "")
                throw ExprReaderException("value attribute is empty.",id.lineNumber());
            bool ok = false;
            unsigned int typref = typref_s.toInt(&ok);
            if(!ok)
                throw ExprReaderException("typref attribute is not an integer.",id.lineNumber());

            if(!id.hasAttribute("suffix")){
                return {VarName::makeVarWithoutSuffix(prefix.toStdString()),typeInfos[typref]};
            } else {
                bool ok;
                int i = id.attribute("suffix").toInt(&ok);
                if(!ok)
                    throw ExprReaderException("suffix attribute must be a integer.",id.lineNumber());
                else if(i == 0)
                    return {VarName::makeVarWithoutSuffix(prefix.toStdString()),typeInfos[typref]}; // xx$0 may occur in while invariant, the suffix '0' could be removed in the ibxml step
                else
                    return {VarName::makeVar(prefix.toStdString(),i),typeInfos[typref]};
            }
        } else if(id.tagName() == "Fresh_Id"){
            QString prefix = id.attribute("ref","");
            if(prefix == "")
                throw ExprReaderException("ref attribute is empty.",id.lineNumber());
            QString typref_s = id.attribute("typref","");
            if(typref_s == "")
                throw ExprReaderException("value attribute is empty.",id.lineNumber());
            bool ok = false;
            unsigned int typref = typref_s.toInt(&ok);
            if(!ok)
                throw ExprReaderException("typref attribute is not an integer.",id.lineNumber());
            return {VarName::makeFreshId(prefix.toStdString()),typeInfos[typref]};
        } else {
            throw ExprReaderException("Id element expected.",id.lineNumber());
        }
        assert(false); // unreachable
    };

    const std::map<std::string, Expr::EKind> etags = {
        {"Binary_Exp", Expr::EKind::BinaryExpr},
        {"Nary_Exp", Expr::EKind::NaryExpr},
        {"Boolean_Literal",Expr::EKind::TRUE},
        {"Boolean_Exp", Expr::EKind::BooleanExpr},
        {"EmptySet", Expr::EKind::EmptySet},
        {"EmptySeq", Expr::EKind::EmptySet},
        {"Id", Expr::EKind::Id},
        {"Fresh_Id", Expr::EKind::Id},
        {"Integer_Literal", Expr::EKind::IntegerLiteral},
        {"Quantified_Exp", Expr::EKind::QuantifiedExpr},
        {"Quantified_Set", Expr::EKind::QuantifiedSet},
        {"String_Literal", Expr::EKind::StringLiteral},
        {"Unary_Exp",  Expr::EKind::UnaryExpr},
        {"Struct", Expr::EKind::Struct},
        {"Record", Expr::EKind::Record},
        {"Real_Literal", Expr::EKind::RealLiteral},
        {"STRING_Literal", Expr::EKind::StringLiteral},
        {"Ternary_Exp",  Expr::EKind::TernaryExpr},
        {"Record_Field_Access",Expr::EKind::Record_Field_Access},
        {"Record_Update",Expr::EKind::Record_Field_Update},
    };
    const std::map<std::string, Expr::UnaryOp> unaryExpOp = {
        {"card", Expr::UnaryOp::Cardinality},
        {"dom", Expr::UnaryOp::Domain},
        {"ran", Expr::UnaryOp::Range},
        {"POW", Expr::UnaryOp::Subsets},
        {"POW1",Expr::UnaryOp::Non_Empty_Subsets},
        {"FIN", Expr::UnaryOp::Finite_Subsets},
        {"FIN1",Expr::UnaryOp::Non_Empty_Finite_Subsets},
        {"union", Expr::UnaryOp::Union},
        {"inter", Expr::UnaryOp::Intersection},
        {"seq", Expr::UnaryOp::Sequences},
        {"seq1", Expr::UnaryOp::Non_Empty_Sequences},
        {"iseq", Expr::UnaryOp::Injective_Sequences},
        {"iseq1", Expr::UnaryOp::Non_Empty_Injective_Sequences},
        {"-i",Expr::UnaryOp::IMinus},
        {"-r",Expr::UnaryOp::RMinus},
        {"~", Expr::UnaryOp::Inverse},
        {"size", Expr::UnaryOp::Size},
        {"perm", Expr::UnaryOp::Permutations},
        {"first", Expr::UnaryOp::First},
        {"last", Expr::UnaryOp::Last},
        {"id", Expr::UnaryOp::Identity},
        {"closure", Expr::UnaryOp::Closure},
        {"closure1", Expr::UnaryOp::Transitive_Closure},
        {"tail", Expr::UnaryOp::Tail},
        {"front", Expr::UnaryOp::Front},
        {"rev", Expr::UnaryOp::Reverse},
        {"conc", Expr::UnaryOp::Concatenation},
        {"rel", Expr::UnaryOp::Rel},
        {"fnc", Expr::UnaryOp::Fnc},
        {"real", Expr::UnaryOp::Real},
        {"floor", Expr::UnaryOp::Floor},
        {"ceiling", Expr::UnaryOp::Ceiling},
        {"imin", Expr::UnaryOp::IMinimum},
        {"imax", Expr::UnaryOp::IMaximum},
        {"rmin", Expr::UnaryOp::RMinimum},
        {"rmax", Expr::UnaryOp::RMaximum},
        {"tree", Expr::UnaryOp::Tree},
        {"btree", Expr::UnaryOp::Btree},
        {"top", Expr::UnaryOp::Top},
        {"sons", Expr::UnaryOp::Sons},
        {"prefix", Expr::UnaryOp::Prefix},
        {"postfix", Expr::UnaryOp::Postfix},
        {"sizet", Expr::UnaryOp::Sizet},
        {"mirror", Expr::UnaryOp::Mirror},
        {"left", Expr::UnaryOp::Left},
        {"right", Expr::UnaryOp::Right},
        {"infix", Expr::UnaryOp::Infix},
        {"bin", Expr::UnaryOp::Bin}
    };

    const std::map<std::string, Expr::BinaryOp> binaryExpOp = {
        {",",Expr::BinaryOp::Mapplet},
        {"*i", Expr::BinaryOp::IMultiplication},
        {"*f", Expr::BinaryOp::FMultiplication},
        {"*r", Expr::BinaryOp::RMultiplication},
        {"*s", Expr::BinaryOp::Cartesian_Product},
        {"**i", Expr::BinaryOp::IExponentiation},
        {"**r", Expr::BinaryOp::RExponentiation},
        {"+i", Expr::BinaryOp::IAddition},
        {"+r", Expr::BinaryOp::RAddition},
        {"+f", Expr::BinaryOp::FAddition},
        {"+->", Expr::BinaryOp::Partial_Functions},
        {"+->>", Expr::BinaryOp::Partial_Surjections},
        {"-i", Expr::BinaryOp::ISubtraction},
        {"-r", Expr::BinaryOp::RSubtraction},
        {"-f", Expr::BinaryOp::FSubtraction},
        {"-s", Expr::BinaryOp::Set_Difference},
        {"-->", Expr::BinaryOp::Total_Functions},
        {"-->>", Expr::BinaryOp::Total_Surjections},
        {"->", Expr::BinaryOp::Head_Insertion},
        {"..", Expr::BinaryOp::Interval},
        {"/i", Expr::BinaryOp::IDivision},
        {"/r", Expr::BinaryOp::RDivision},
        {"/f", Expr::BinaryOp::FDivision},
        {"/\\", Expr::BinaryOp::Intersection},
        {"/|\\", Expr::BinaryOp::Head_Restriction},
        {";",Expr::BinaryOp::Composition},
        {"<+", Expr::BinaryOp::Surcharge},
        {"<->", Expr::BinaryOp::Relations},
        {"<-", Expr::BinaryOp::Tail_Insertion},
        {"<<|", Expr::BinaryOp::Domain_Subtraction},
        {"<|", Expr::BinaryOp::Domain_Restriction},
        {">+>", Expr::BinaryOp::Partial_Injections},
        {">->", Expr::BinaryOp::Total_Injections},
        {">+>>", Expr::BinaryOp::Partial_Bijections},
        {">->>", Expr::BinaryOp::Total_Bijections},
        {"><", Expr::BinaryOp::Direct_Product},
        {"||",Expr::BinaryOp::Parallel_Product},
        {"\\/",Expr::BinaryOp::Union},
        {"\\|/",Expr::BinaryOp::Tail_Restriction},
        {"^",Expr::BinaryOp::Concatenation},
        {"mod",Expr::BinaryOp::Modulo},
        {"|->",Expr::BinaryOp::Mapplet},
        {"|>",Expr::BinaryOp::Range_Restriction},
        {"|>>",Expr::BinaryOp::Range_Subtraction},
        {"[", Expr::BinaryOp::Image},
        {"(",Expr::BinaryOp::Application},
        {"prj1", Expr::BinaryOp::First_Projection},
        {"prj2", Expr::BinaryOp::Second_Projection},
        {"iterate", Expr::BinaryOp::Iteration},
        {"const", Expr::BinaryOp::Const},
        {"rank", Expr::BinaryOp::Rank},
        {"father", Expr::BinaryOp::Father},
        {"subtree", Expr::BinaryOp::Subtree},
        {"arity", Expr::BinaryOp::Arity}
    };

    const std::map<std::string, Expr::TernaryOp> ternaryExpOp = {
        {"bin",Expr::TernaryOp::Bin},
        {"son",Expr::TernaryOp::Son},
    };

    const std::map<std::string, Expr::NaryOp> naryExpOp = {
        {"[",Expr::NaryOp::Sequence},
        {"{",Expr::NaryOp::Set}
    };

    const std::map<std::string, Expr::QuantifiedOp> quantifiedExprOp = {
        {"%",Expr::QuantifiedOp::Lambda},
        {"rSIGMA",Expr::QuantifiedOp::RSum},
        {"iSIGMA",Expr::QuantifiedOp::ISum},
        {"rPI",Expr::QuantifiedOp::RProduct},
        {"iPI",Expr::QuantifiedOp::IProduct},
        {"INTER",Expr::QuantifiedOp::Intersection},
        {"UNION",Expr::QuantifiedOp::Union},
        {"rPI",Expr::QuantifiedOp::RProduct},
        {"rSIGMA",Expr::QuantifiedOp::RSum}
    };

    const std::map<std::string, Expr::EKind> constantExpr = {
        {"MAXINT",Expr::EKind::MaxInt},
        {"MININT",Expr::EKind::MinInt},
        {"INTEGER",Expr::EKind::INTEGER},
        {"NATURAL",Expr::EKind::NATURAL},
        {"NATURAL1",Expr::EKind::NATURAL1},
        {"INT",Expr::EKind::INT},
        {"NAT",Expr::EKind::NAT},
        {"NAT1",Expr::EKind::NAT1},
        {"STRING",Expr::EKind::STRING},
        {"BOOL",Expr::EKind::BOOL},
        {"REAL",Expr::EKind::REAL},
        {"FLOAT",Expr::EKind::FLOAT},
        {"TRUE",Expr::EKind::TRUE},
        {"FALSE",Expr::EKind::FALSE},
        {"succ", Expr::EKind::Successor},
        {"pred", Expr::EKind::Predecessor}
    };

    Expr readExpression(const QDomElement &dom, const std::vector<BType> &typeInfos){
        if (dom.isNull())
            throw ExprReaderException("Null dom element.",-1);

        QString tagName = dom.tagName();
        if(!dom.hasAttribute("typref"))
            throw ExprReaderException("Missing typref attribute for '" + tagName.toStdString() + "'.",dom.lineNumber());
        BType type = typeInfos[dom.attribute("typref").toUInt()];
        QStringList bxmlTag;
        QString _bxmlTag = dom.attribute("tag");
        if(_bxmlTag != "")
            bxmlTag.push_back(_bxmlTag);

        auto it = etags.find(tagName.toStdString());
        if(it == etags.end())
            throw ExprReaderException("Unexpected tag '" + tagName.toStdString() + "'.",dom.lineNumber());

        switch(it->second){
            case Expr::EKind::BinaryExpr:
                {
                    QString op = dom.attribute("op");
                    auto it = binaryExpOp.find(op.toStdString());
                    if(it == binaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown binary expression operator '" + op.toStdString() + "'.",dom.lineNumber());
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    Expr lhs = readExpression(fst,typeInfos);
                    Expr rhs = readExpression(snd,typeInfos);
                    return Expr::makeBinaryExpr(it->second,std::move(lhs),std::move(rhs),type,bxmlTag);
                }
            case Expr::EKind::TernaryExpr:
                {
                    QString op = dom.attribute("op");
                    auto it = ternaryExpOp.find(op.toStdString());
                    if(it == ternaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown ternary expression operator '" + op.toStdString() + "'.",dom.lineNumber());
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    QDomElement thd = snd.nextSiblingElement();
                    Expr efst = readExpression(fst,typeInfos);
                    Expr esnd = readExpression(snd,typeInfos);
                    Expr ethd = readExpression(thd,typeInfos);
                    return Expr::makeTernaryExpr(it->second,std::move(efst),std::move(esnd),std::move(ethd),type,bxmlTag);
                }
            case Expr::EKind::NaryExpr:
                {
                    QString op = dom.attribute("op");
                    auto it = naryExpOp.find(op.toStdString());
                    if(it == naryExpOp.end())
                        throw ExprReaderException
                            ("Unknown n-ary expression operator '" + op.toStdString() + "'.",dom.lineNumber());
                    std::vector<Expr> lst;
                    QDomElement ce = dom.firstChildElement();
                    while (!ce.isNull()) {
                        lst.push_back(readExpression(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }
                    return Expr::makeNaryExpr(it->second,std::move(lst),type, bxmlTag);
                }
            case Expr::EKind::BooleanExpr:
                {
                    return Expr::makeBooleanExpr(
                            readPredicate(dom.firstChildElement(),typeInfos),
                            bxmlTag );
                }
            case Expr::EKind::EmptySet:
                {
                    return Expr::makeEmptySet(type, bxmlTag);
                }
            case Expr::EKind::Id:
                {
                    if(tagName.toStdString() == "Fresh_Id"){
                        TypedVar tv = VarNameFromId(dom,typeInfos);
                        return Expr::makeIdent(tv.name, tv.type, bxmlTag);
                    }

                    auto v = dom.attribute("value").toStdString();
                    auto it = constantExpr.find(v);

                    if(it == constantExpr.end()){
                        TypedVar tv = VarNameFromId(dom,typeInfos);
                        return Expr::makeIdent(tv.name, tv.type, bxmlTag);
                    }

                    switch (it->second){
                        case Expr::EKind::MaxInt:
                            return Expr::makeMaxInt(bxmlTag);
                        case Expr::EKind::MinInt:
                            return Expr::makeMinInt(bxmlTag);
                        case Expr::EKind::INTEGER:
                            return Expr::makeINTEGER(bxmlTag);
                        case Expr::EKind::NATURAL:
                            return Expr::makeNATURAL(bxmlTag);
                        case Expr::EKind::NATURAL1:
                            return Expr::makeNATURAL1(bxmlTag);
                        case Expr::EKind::INT:
                            return Expr::makeINT(bxmlTag);
                        case Expr::EKind::NAT:
                            return Expr::makeNAT(bxmlTag);
                        case Expr::EKind::NAT1:
                            return Expr::makeNAT1(bxmlTag);
                        case Expr::EKind::STRING:
                            return Expr::makeSTRING(bxmlTag);
                        case Expr::EKind::BOOL:
                            return Expr::makeBOOL(bxmlTag);
                        case Expr::EKind::REAL:
                            return Expr::makeREAL(bxmlTag);
                        case Expr::EKind::FLOAT:
                            return Expr::makeFLOAT(bxmlTag);
                        case Expr::EKind::TRUE:
                            return Expr::makeTRUE(bxmlTag);
                        case Expr::EKind::FALSE:
                            return Expr::makeFALSE(bxmlTag);
                        case Expr::EKind::Successor:
                            return Expr::makeSuccessor(type,bxmlTag);
                        case Expr::EKind::Predecessor:
                            return Expr::makePredecessor(type,bxmlTag);
                        default:
                            assert(false); // unreachable
                    };
                }
            case Expr::EKind::IntegerLiteral:
                {
                    return Expr::makeInteger(dom.attribute("value").toStdString(),bxmlTag);
                }
            case Expr::EKind::RealLiteral:
                {
                    QStringList lst = dom.attribute("value").split(".");
                    if(lst.size() == 1 || lst.size() == 2){
                        std::string integerPart = lst[0].toStdString();
                        if(lst.size() == 1){
                            return Expr::makeReal(Expr::Decimal(integerPart),bxmlTag);
                        } else /* lst.size() == 2 */ {
                            std::string decimalPart = lst[1].toStdString();
                            return Expr::makeReal(Expr::Decimal(integerPart,decimalPart),bxmlTag);
                        }
                    } else {
                        throw ExprReaderException("Incorrect decimal value ("+ dom.attribute("value").toStdString() + ").",dom.lineNumber());
                    }
                }
            case Expr::EKind::StringLiteral:
                {
                    return Expr::makeString(dom.attribute("value").toStdString(),bxmlTag);
                }
            case Expr::EKind::QuantifiedExpr:
                {
                    QString op = dom.attribute("type");
                    auto it = quantifiedExprOp.find(op.toStdString());
                    if(it == quantifiedExprOp.end())
                        throw ExprReaderException
                            ("Unknown type of quantified expression '" + op.toStdString() + "'.",dom.lineNumber());

                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw ExprReaderException
                            ("The 'Quantified_Exp' element is missing some 'Variables' child.",dom.lineNumber());
                    std::vector<TypedVar> ids;
                    for(    QDomElement ce = vars.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        ids.push_back(VarNameFromId(ce,typeInfos));
                    }
                    Pred pre = readPredicate(dom.firstChildElement("Pred").firstChildElement(),typeInfos);
                    Expr body = readExpression(dom.firstChildElement("Body").firstChildElement(),typeInfos);
                    return Expr::makeQuantifiedExpr(it->second,ids,std::move(pre),std::move(body),type,bxmlTag );
                }
            case Expr::EKind::QuantifiedSet:
                {
                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw ExprReaderException
                            ("The 'Quantified_Set' element is missing some 'Variables' child.",dom.lineNumber());
                    std::vector<TypedVar> ids;
                    for(    QDomElement ce = vars.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        ids.push_back(VarNameFromId(ce,typeInfos));
                    }
                    Pred body = readPredicate(dom.firstChildElement("Body").firstChildElement(),typeInfos);
                    return Expr::makeQuantifiedSet(ids,std::move(body),type,bxmlTag );
                }
            case Expr::EKind::UnaryExpr:
                {
                    QString op = dom.attribute("op");
                    auto it = unaryExpOp.find(op.toStdString());
                    if(it == unaryExpOp.end())
                        throw ExprReaderException
                            ("Unknown unary expression operator '" + op.toStdString() + "'.",dom.lineNumber());
                    Expr content = readExpression(dom.firstChildElement(),typeInfos);
                    return Expr::makeUnaryExpr(it->second,std::move(content),type,bxmlTag);
                }
            case Expr::EKind::Struct:
                {
                    std::vector<std::pair<std::string,Expr>> vec;
                    for(QDomElement recItem = dom.firstChildElement("Record_Item");
                            !recItem.isNull();
                            recItem = recItem.nextSiblingElement("Record_Item"))
                    {
                        if(!recItem.hasAttribute("label"))
                            throw ExprReaderException
                                ("The 'Record_Item' element is missing a 'label' attribute.",dom.lineNumber());
                        vec.push_back(std::make_pair(
                                    recItem.attribute("label").toStdString(),
                                    readExpression(recItem.firstChildElement(),typeInfos)));
                    };
                    std::sort(vec.begin(),vec.end(),RecordFieldCmp);
                    return Expr::makeStruct(std::move(vec),type, bxmlTag);
                }
            case Expr::EKind::Record:
                {
                    std::vector<std::pair<std::string,Expr>> vec;
                    for(QDomElement recItem = dom.firstChildElement("Record_Item");
                            !recItem.isNull();
                            recItem = recItem.nextSiblingElement("Record_Item"))
                    {
                        if(!recItem.hasAttribute("label"))
                            throw ExprReaderException
                                ("The 'Record_Item' element is missing a 'label' attribute.",dom.lineNumber());
                        vec.push_back(std::make_pair(
                                    recItem.attribute("label").toStdString(),
                                    readExpression(recItem.firstChildElement(),typeInfos)));
                    };
                    std::sort(vec.begin(),vec.end(),RecordFieldCmp);
                    return Expr::makeRecord(std::move(vec),type, bxmlTag);
                }
            case Expr::EKind::TRUE: // Boolean_Literal
                {
                    QString lt = dom.attribute("value","").toUpper();
                    if(lt == "TRUE") return Expr::makeTRUE(bxmlTag);
                    else if(lt == "FALSE") return Expr::makeFALSE(bxmlTag);
                    else
                        throw ExprReaderException("Unknown boolean literal '"
                                + lt.toStdString() + "'.",dom.lineNumber());
                }
            case Expr::EKind::Record_Field_Update:
                {
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    Expr rec = readExpression(fst,typeInfos);
                    std::string label = dom.attribute("label").toStdString();
                    Expr fval = readExpression(snd,typeInfos);
                    return Expr::makeRecordFieldUpdate(std::move(rec),label,std::move(fval),type,bxmlTag);
                }
            case Expr::EKind::Record_Field_Access:
                {
                    QDomElement fst = dom.firstChildElement();
                    Expr rec = readExpression(fst,typeInfos);
                    std::string label = dom.attribute("label").toStdString();
                    return Expr::makeRecordFieldAccess(std::move(rec),label,type,bxmlTag);
                }
            case Expr::EKind::MaxInt:
            case Expr::EKind::MinInt:
            case Expr::EKind::INTEGER:
            case Expr::EKind::NATURAL:
            case Expr::EKind::NATURAL1:
            case Expr::EKind::INT:
            case Expr::EKind::NAT:
            case Expr::EKind::NAT1:
            case Expr::EKind::STRING:
            case Expr::EKind::BOOL:
            case Expr::EKind::REAL:
            case Expr::EKind::FLOAT:
            case Expr::EKind::FALSE:
            case Expr::EKind::Successor:
            case Expr::EKind::Predecessor:
                {
                    assert(false); // unreachable
                }
        };
        assert(false); // unreachable
    };
}
