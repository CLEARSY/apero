/** gpredReader.cpp

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
#include "substReader.h"
#include "gpredReader.h"
#include "gpred.h"
#include <map>

namespace Xml {

    const std::map<std::string, GPred::Kind> ptags = {
        {"Binary_Pred", GPred::Kind::Implication},
        {"Exp_Comparison", GPred::Kind::ExprComparison},
        {"Quantified_Pred", GPred::Kind::Forall},
        {"Unary_Pred", GPred::Kind::Negation},
        {"Nary_Pred", GPred::Kind::Conjunction},
        {"Tag", GPred::Kind::TaggedPred},
        {"Sub_Calculus", GPred::Kind::Sub},
        {"Not", GPred::Kind::NotSubNot},
        {"Let_Fresh_Id", GPred::Kind::LetFreshId},
    };

    GPred readGPredicate(const QDomElement &dom, const std::vector<BType> &typeInfos){
        if (dom.isNull())
            throw GPredReaderException("Null dom element.");

        QString tagName = dom.tagName();

        auto it = ptags.find(tagName.toStdString());
        if(it == ptags.end())
            throw GPredReaderException("Unexpected tag '" + tagName.toStdString() + "'.");

        switch(it->second){
            case GPred::Kind::NotSubNot:
                {
                    QDomElement child = dom.firstChildElement();
                    if(child.isNull() or child.tagName() != "Sub_Calculus")
                        throw GPredReaderException("Sub_Calculus element expected.");
                    QDomElement sub = child.firstChildElement();
                    QDomElement _not = sub.nextSiblingElement();
                    if(_not.isNull() or _not.tagName() != "Not")
                        throw GPredReaderException("Not element expected.");
                    QDomElement prd = _not.firstChildElement();
                    return GPred::makeNotSubNot
                        (readSubstitution(sub,typeInfos),readPredicate(prd,typeInfos));
                }

            case GPred::Kind::Sub:
                {
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    bool overflow = (dom.attribute("overflow") == "true");
                    return GPred::makeSub
                        (readSubstitution(fst,typeInfos),readGPredicate(snd,typeInfos),overflow);
                }
            case GPred::Kind::Implication:
            case GPred::Kind::Equivalence:
                {
                    QString op = dom.attribute("op");
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    if(op == "=>"){
                        return GPred::makeImplication(readGPredicate(fst,typeInfos),readGPredicate(snd,typeInfos));
                    } else if (op == "<=>"){
                        return GPred::makeEquivalence(readGPredicate(fst,typeInfos),readGPredicate(snd,typeInfos));
                    } else {
                        throw GPredReaderException
                            ("Unknown binary predicate operator '" + op.toStdString() + "'.");
                    }
                }
            case GPred::Kind::ExprComparison:
                {
                    QString op = dom.attribute("op");
                    auto it = comparisonOp.find(op.toStdString());
                    QDomElement fst = dom.firstChildElement();
                    QDomElement snd = fst.nextSiblingElement();
                    if(it != comparisonOp.end())
                        return GPred::makeExprComparison
                            (it->second,readExpression(fst,typeInfos),readExpression(snd,typeInfos));
                    if (op == "/:")
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Membership,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/<:")
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/<<:")
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Strict_Subset,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                    if (op == "/=")
                        return GPred::makeNegationPred (GPred::makeExprComparison
                                (Pred::ComparisonOp::Equality,
                                 readExpression(fst,typeInfos),
                                 readExpression(snd,typeInfos)));
                        throw GPredReaderException
                            ("Unknown comparison operator '" + op.toStdString() + "'.");

                }
            case GPred::Kind::Forall:
            case GPred::Kind::Exists:
                {
                    QString op = dom.attribute("type");
                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw GPredReaderException
                            ("The 'Quantified_Pred' element is missing some 'Variables' child.");
                    std::vector<TypedVar> vec;
                    for(    QDomElement ce = vars.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    if(op == "!"){
                        return GPred::makeForall(vec,
                                readGPredicate(dom.firstChildElement("Body").firstChildElement(),typeInfos));
                    } else if (op == "#"){
                        return GPred::makeExists(vec,
                                readGPredicate(dom.firstChildElement("Body").firstChildElement(),typeInfos));
                    } else {
                        throw GPredReaderException
                            ("Unknown type of quantified predicate '" + op.toStdString() + "'.");
                    }
                }
            case GPred::Kind::Negation:
                {
                    QString op = dom.attribute("op");
                    if(op != "not")
                        throw GPredReaderException
                            ("Unknown unary predicate operator '" + op.toStdString() + "'.");

                    return GPred::makeNegationPred(
                            readGPredicate(dom.firstChildElement(),typeInfos));
                }
            case GPred::Kind::Conjunction:
            case GPred::Kind::Disjunction:
                {
                    QString op = dom.attribute("op");
                    std::vector<GPred> vec;
                    QDomElement ce = dom.firstChildElement();
                    while (!ce.isNull()) {
                        vec.push_back(readGPredicate(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }
                    if(op == "&"){
                        return GPred::makeConjunction(std::move(vec));
                    } else if(op == "or"){
                        return GPred::makeDisjunction(std::move(vec));
                    } else {
                        throw GPredReaderException
                            ("Unknown n-ary predicate operator '" + op.toStdString() + "'.");
                    }
                }
            case GPred::Kind::TaggedPred:
                {
                    auto elt = dom.firstChildElement();
                    return GPred::makeTaggedPred(dom.attribute("goalTag").toStdString(),readGPredicate(elt,typeInfos));
                }
            case GPred::Kind::LetFreshId:
                {
                    auto elt = dom.firstChildElement();
                    return GPred::makeLetFreshId(dom.attribute("name").toStdString(),readGPredicate(elt,typeInfos));
                }
        };
        assert(false); // unreachable
    };
}
