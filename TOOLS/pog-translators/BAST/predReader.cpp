/** predReader.cpp

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
#include <map>

namespace Xml {
    const std::map<std::string, Pred::PKind> ptags = {
        {"Binary_Pred", Pred::PKind::Implication},
        {"Exp_Comparison", Pred::PKind::ExprComparison},
        {"Quantified_Pred", Pred::PKind::Forall},
        {"Unary_Pred", Pred::PKind::Negation},
        {"Nary_Pred", Pred::PKind::Conjunction},
    };

    const std::map<std::string, Pred::ComparisonOp> comparisonOp = {
                {":", Pred::ComparisonOp::Membership},
        {"<:",Pred::ComparisonOp::Subset},
        {"<<:",Pred::ComparisonOp::Strict_Subset},
        {"=", Pred::ComparisonOp::Equality},
        {">=i",Pred::ComparisonOp::Ige},
        {">i",Pred::ComparisonOp::Igt},
        {"<i",Pred::ComparisonOp::Ilt},
        {"<=i",Pred::ComparisonOp::Ile},
        {">=r",Pred::ComparisonOp::Rge},
        {">r",Pred::ComparisonOp::Rgt},
        {"<r",Pred::ComparisonOp::Rlt},
        {"<=r",Pred::ComparisonOp::Rle},
        {">=f",Pred::ComparisonOp::Fge},
        {">f",Pred::ComparisonOp::Fgt},
        {"<f",Pred::ComparisonOp::Flt},
        {"<=f",Pred::ComparisonOp::Fle},
    };

    Pred readPredicate(const QDomElement &dom, const std::vector<BType> &typeInfos){
        if (dom.isNull())
            throw PredReaderException("Null dom element.");

        QString tagName = dom.tagName();

        if(tagName == "Tag")
            return readPredicate(dom.firstChildElement(),typeInfos);

        auto it = ptags.find(tagName.toStdString());
        if(it == ptags.end())
            throw PredReaderException("Unexpected tag '" + tagName.toStdString() + "'.");

        switch(it->second){
            case Pred::PKind::Implication:
            case Pred::PKind::Equivalence:
                {
                    QString op = dom.attribute("op");
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    if(op == "=>"){
                        return Pred::makeImplication(readPredicate(fst,typeInfos),readPredicate(snd,typeInfos));
                    } else if(op == "<=>"){
                        return Pred::makeEquivalence(readPredicate(fst,typeInfos),readPredicate(snd,typeInfos));
                    } else {
                        throw PredReaderException
                            ("Unknown binary predicate operator '" + op.toStdString() + "'.");
                    }
                }
            case Pred::PKind::ExprComparison:
                {
                    QString op = dom.attribute("op");
                    auto it = comparisonOp.find(op.toStdString());
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    if(it != comparisonOp.end())
                        return Pred::makeExprComparison
                            (it->second,readExpression(fst,typeInfos),readExpression(snd,typeInfos));
                    if (op == "/:")
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Membership,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/<:")
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/<<:")
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Strict_Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/=")
                        return Pred::makeNegation (Pred::makeExprComparison
                                (Pred::ComparisonOp::Equality,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    throw PredReaderException
                            ("Unknown comparison operator '" + op.toStdString() + "'.");
                }
            case Pred::PKind::Forall:
            case Pred::PKind::Exists:
                {
                    QString op = dom.attribute("type");
                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw PredReaderException
                            ("The 'Quantified_Pred' element is missing some 'Variables' child.");
                    std::vector<TypedVar> vec;
                    for(    QDomElement ce = vars.firstChildElement("Id");
                            !ce.isNull();
                            ce = ce.nextSiblingElement("Id") )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }

                    if(op == "!"){
                        return Pred::makeForall(vec,
                                readPredicate(dom.firstChildElement("Body").firstChildElement(),typeInfos));
                    } else if (op == "#"){
                        return Pred::makeExists(vec,
                                readPredicate(dom.firstChildElement("Body").firstChildElement(),typeInfos));
                    } else
                        throw PredReaderException
                            ("Unknown type of quantified predicate '" + op.toStdString() + "'.");
                }
            case Pred::PKind::Negation:
                {
                    QString op = dom.attribute("op");
                    if(op != "not")
                        throw PredReaderException
                            ("Unknown unary predicate operator '" + op.toStdString() + "'.");

                    return Pred::makeNegation(
                            readPredicate(dom.firstChildElement(),typeInfos));
                }
            case Pred::PKind::Conjunction:
            case Pred::PKind::Disjunction:
                {
                    QString op = dom.attribute("op");
                    QDomElement ce = dom.firstChildElement();
                    std::vector<Pred> vec;
                    while (!ce.isNull()) {
                        vec.push_back(readPredicate(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }

                    if(op == "&"){
                        return Pred::makeConjunction(std::move(vec));
                    } else if (op == "or"){
                        return Pred::makeDisjunction(std::move(vec));
                    } else {
                        throw PredReaderException
                            ("Unknown n-ary predicate operator '" + op.toStdString() + "'.");
                    }

                }
            case Pred::PKind::True:
            case Pred::PKind::False:
                assert(false); // unreachable
        };
        assert(false); // unreachable
    };
}
