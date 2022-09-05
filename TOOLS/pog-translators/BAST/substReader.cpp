/** substReader.cpp

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
#include "substReader.h"
#include "exprReader.h"
#include "predReader.h"
#include <map>

namespace Xml {
    const std::map<std::string, Subst::SKind> ptags = {
        {"Bloc_Sub", Subst::SKind::Block},
        {"Skip", Subst::SKind::Skip},
        {"Assert_Sub", Subst::SKind::Assert},
        {"PRE_Sub", Subst::SKind::Assert},
        {"If_Sub", Subst::SKind::IfThenElse},
        {"Simple_Assignement_Sub", Subst::SKind::SimpleAssignment},
        {"Select", Subst::SKind::Select},
        {"Case_Sub", Subst::SKind::Case},
        {"ANY_Sub", Subst::SKind::Any},
        {"Operation_Call", Subst::SKind::OperationCall},
        {"While", Subst::SKind::While},
        {"Witness", Subst::SKind::Witness},
    };

    Subst readSubstitution(const QDomElement &dom, const std::vector<BType> &typeInfos){
        if (dom.isNull())
            throw SubstReaderException("Null dom element.");

        QString tagName = dom.tagName();
        Subst::SKind kind;
        if(tagName == "Nary_Sub") {
            QString op = dom.attribute("op");
            if(op == "||")
                kind = Subst::SKind::Parallel;
            else if(op == ";")
                kind = Subst::SKind::Sequence;
            else if(op == "CHOICE")
                kind = Subst::SKind::Choice;
            else
                throw SubstReaderException("Unknown nary substitution operator '"+op.toStdString()+"'.");
        }
        else
        {
            auto it = ptags.find(tagName.toStdString());
            if(it == ptags.end())
                throw SubstReaderException("Unexpected tag '" + tagName.toStdString() + "'.");
            kind = it->second;
        };

        switch(kind){
            case Subst::SKind::Block:
                return Subst::makeBlock(readSubstitution(dom.firstChildElement(),typeInfos));
            case Subst::SKind::Skip:
                return Subst::makeSkip();
            case Subst::SKind::Assert:
                {
                    QDomElement guard;
                    if(tagName == "PRE_Sub"){
                        guard = dom.firstChildElement("Precondition");
                        if(guard.isNull())
                            throw SubstReaderException("Missing child 'Precondition' in PRE_Sub element.");
                    } else {
                        guard = dom.firstChildElement("Guard");
                        if(guard.isNull())
                            throw SubstReaderException("Missing child 'Guard' in Assert_Sub element.");
                    }
                    QDomElement body = dom.firstChildElement("Body");
                    if(body.isNull())
                        throw SubstReaderException("Missing child 'Body' in Assert_Sub or PRE_Sub element.");
                    return Subst::makeAssert(
                            readPredicate(guard.firstChildElement(),typeInfos),
                            readSubstitution(body.firstChildElement(),typeInfos)
                            );
                }
            case Subst::SKind::IfThen:
            case Subst::SKind::IfThenElse:
                {
                    QDomElement condition = dom.firstChildElement("Condition");
                    if(condition.isNull())
                        throw SubstReaderException("Missing child 'Condition' in 'If_Sub' element.");
                    QDomElement then = dom.firstChildElement("Then");
                    if(then.isNull())
                        throw SubstReaderException("Missing child 'Then' in 'If_Sub' element.");
                    QDomElement els = dom.firstChildElement("Else");
                    if(els.isNull())
                        return Subst::makeIfThen(
                                readPredicate(condition.firstChildElement(),typeInfos),
                                readSubstitution(then.firstChildElement(),typeInfos)
                                );
                    else
                        return Subst::makeIfThenElse(
                                readPredicate(condition.firstChildElement(),typeInfos),
                                readSubstitution(then.firstChildElement(),typeInfos),
                                readSubstitution(els.firstChildElement(),typeInfos)
                                );
                }
            case Subst::SKind::SimpleAssignment:
                {
                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw SubstReaderException("Missing child 'Variables' in 'Simple_Assignement_Sub' element.");
                    std::vector<TypedVar> vec;
                    for(    QDomElement ce = vars.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    QDomElement values = dom.firstChildElement("Values");
                    if(values.isNull())
                        throw SubstReaderException("Missing child 'Values' in 'Simple_Assignement_Sub' element.");
                    std::vector<Expr> vec2;
                    for(    QDomElement ce = values.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        vec2.push_back(readExpression(ce,typeInfos));
                    }
                    return Subst::makeSimpleAssignment(vec,std::move(vec2));
                }
            case Subst::SKind::Select:
            case Subst::SKind::SelectElse:
                {
                    QDomElement clauses = dom.firstChildElement("When_Clauses");
                    if(clauses.isNull())
                        throw SubstReaderException("Missing child 'When_Clauses' in 'Select' element.");
                    std::vector<std::pair<Pred,Subst>> vec;
                    for(    QDomElement ce = clauses.firstChildElement("When");
                            !ce.isNull();
                            ce = ce.nextSiblingElement("When") )
                    {
                        QDomElement cond = ce.firstChildElement("Condition");
                        if(cond.isNull())
                            throw SubstReaderException("Missing child 'Condition' in 'When' element.");
                        QDomElement then = ce.firstChildElement("Then");
                        if(then.isNull())
                            throw SubstReaderException("Missing child 'Then' in 'When' element.");
                        vec.push_back( {
                                readPredicate(cond.firstChildElement(),typeInfos),
                                readSubstitution(then.firstChildElement(),typeInfos) });
                    }
                    QDomElement els = dom.firstChildElement("Else");
                    if(els.isNull())
                        return Subst::makeSelect(std::move(vec));
                    else
                        return Subst::makeSelectElse(std::move(vec),
                                readSubstitution(els.firstChildElement(),typeInfos) );
                }
            case Subst::SKind::Case:
            case Subst::SKind::CaseElse:
                {
                    QDomElement value = dom.firstChildElement("Value");
                    if(value.isNull())
                        throw SubstReaderException("Missing child 'Value' in 'Case_Sub' element.");
                    QDomElement choices = dom.firstChildElement("Choices");
                    if(choices.isNull())
                        throw SubstReaderException("Missing child 'Choices' in 'Case_Sub' element.");
                    std::vector<Subst::CaseChoice> vec;
                    for(    QDomElement ce = choices.firstChildElement("Choice");
                            !ce.isNull();
                            ce = ce.nextSiblingElement("Choice") )
                    {
                        QDomElement v = ce.firstChildElement("Value");
                        Subst::CaseChoice ch;
                        for(    QDomElement v = ce.firstChildElement("Value");
                                !v.isNull();
                                v = v.nextSiblingElement("Value") )
                        {
                            ch.values.push_back(readExpression(v.firstChildElement(),typeInfos));
                        }
                        if(ch.values.empty())
                            throw SubstReaderException("Missing child 'Value' in 'Choice' element.");
                        QDomElement then = ce.firstChildElement("Then");
                        if(then.isNull())
                            throw SubstReaderException("Missing child 'Then' in 'Choice' element.");
                        ch.body = readSubstitution(then.firstChildElement(),typeInfos);
                        vec.push_back(std::move(ch));
                    }
                    QDomElement els = dom.firstChildElement("Else");
                    if(els.isNull())
                        return Subst::makeCase(readExpression(value.firstChildElement(),typeInfos),std::move(vec));
                    else {
                        return Subst::makeCaseElse(readExpression(value.firstChildElement(),typeInfos),
                                std::move(vec),
                                readSubstitution(els.firstChildElement(),typeInfos) );
                    }
                }
            case Subst::SKind::Any:
                {
                    QDomElement vars = dom.firstChildElement("Variables");
                    if(vars.isNull())
                        throw SubstReaderException("Missing child 'Variables' in 'ANY_Sub' element.");
                    std::vector<TypedVar> vec;
                    for(    QDomElement ce = vars.firstChildElement();
                            !ce.isNull();
                            ce = ce.nextSiblingElement() )
                    {
                        vec.push_back(VarNameFromId(ce,typeInfos));
                    }
                    QDomElement pred = dom.firstChildElement("Pred");
                    if(pred.isNull())
                        throw SubstReaderException("Missing child 'Pred' in 'ANY_Sub' element.");
                    QDomElement then = dom.firstChildElement("Then");
                    if(then.isNull())
                        throw SubstReaderException("Missing child 'Then' in 'ANY_Sub' element.");
                    QDomElement fc = then.firstChildElement();
                    return Subst::makeAny(
                            vec,
                            readPredicate(pred.firstChildElement(),typeInfos),
                            readSubstitution(fc,typeInfos) );
                }
            case Subst::SKind::Witness:
                {
                    QDomElement wt = dom.firstChildElement("Witnesses");
                    if(wt.isNull())
                        throw SubstReaderException("Missing child 'Witnesses' in 'Witness' element.");
                    std::map<std::string,Expr> witnesses;
                    QDomElement wt_child = wt.firstChildElement();
                    if(wt_child.isNull())
                        throw SubstReaderException("Missing child in 'Witnesses' element.");
                    if(wt_child.tagName() == "Nary_Pred"){
                        if(wt_child.attribute("op") != "&")
                            throw SubstReaderException("Expected Nary_Pred with attribute op '&'.");
                        for(    QDomElement ce = wt_child.firstChildElement("Exp_Comparison");
                                !ce.isNull();
                                ce = ce.nextSiblingElement("Exp_Comparison") )
                        {
                            if(ce.attribute("op") != "=")
                                throw SubstReaderException("Expected Exp_Comparison with attribute op '='.");
                            QDomElement id = ce.firstChildElement();
                            if(id.tagName() != "Id")
                                throw SubstReaderException("Id element expected.");
                            if(!id.hasAttribute("value"))
                                throw SubstReaderException("value attribute expected.");
                            QDomElement expr = id.nextSiblingElement();
                            std::pair<std::string,Expr> pair = {id.attribute("value").toStdString(), readExpression(expr,typeInfos)};
                            witnesses.insert(std::move(pair));
                        }
                    }
                    else if(wt_child.tagName() == "Exp_Comparison"){
                        if(wt_child.attribute("op") != "=")
                            throw SubstReaderException("Expected Exp_Comparison with attribute op '='.");
                        QDomElement id = wt_child.firstChildElement();
                        if(id.tagName() != "Id")
                            throw SubstReaderException("Id element expected.");
                        if(!id.hasAttribute("value"))
                            throw SubstReaderException("value attribute expected.");
                        QDomElement expr = id.nextSiblingElement();
                        std::pair<std::string,Expr> pair = {id.attribute("value").toStdString(), readExpression(expr,typeInfos)};
                        witnesses.insert(std::move(pair));
                    } else {
                        throw SubstReaderException("Nary_Pred or Exp_Comparison element expected.");
                    }
                    QDomElement body = dom.firstChildElement("Body");
                    if(body.isNull())
                        throw SubstReaderException("Missing child 'Body' in 'Witness' element.");
                    return Subst::makeWitness(
                            std::move(witnesses),
                            readSubstitution(body.firstChildElement(),typeInfos) );
                }
            case Subst::SKind::OperationCall:
                {
                    QDomElement name = dom.firstChildElement("Name");
                    if(name.isNull())
                        throw SubstReaderException("Missing child 'Name' in 'Operation_Call' element.");
                    QDomElement id = name.firstChildElement("Id");
                    if(id.isNull())
                        throw SubstReaderException("Missing child 'Id' in 'Name' element.");
                    if(!id.hasAttribute("value"))
                        throw SubstReaderException("Missing attribute 'value' in 'Id' element.");

                    // Inputs (Effective)
                    std::vector<Expr> v_input;
                    QDomElement input = dom.firstChildElement("Input_Parameters");
                    if(!input.isNull()){
                        for(    QDomElement ce = input.firstChildElement();
                                !ce.isNull();
                                ce = ce.nextSiblingElement() )
                        {
                            v_input.push_back(readExpression(ce,typeInfos));
                        }
                    }
                    // Outputs (Effective)
                    std::vector<TypedVar> v_output;
                    QDomElement output = dom.firstChildElement("Output_Parameters");
                    if(!output.isNull()){
                        for(    QDomElement ce = output.firstChildElement();
                                !ce.isNull();
                                ce = ce.nextSiblingElement() )
                        {
                            v_output.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }
                    QDomElement operation = dom.firstChildElement("Operation");
                    if(operation.isNull())
                        throw SubstReaderException("Missing child 'Operation' in 'Operation_Call' element.");
                    if(!operation.hasAttribute("name"))
                        throw SubstReaderException("Missing attribute 'name' in 'Operation' element.");

                    // Outputs (Formal)
                    std::vector<TypedVar> op_outputs;
                    QDomElement op_out_params = operation.firstChildElement("Output_Parameters");
                    if(!op_out_params.isNull()){
                        for(    QDomElement ce = op_out_params.firstChildElement("Id");
                                !ce.isNull();
                                ce = ce.nextSiblingElement("Id") )
                        {
                            op_outputs.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }

                    // Inputs (Formal)
                    std::vector<TypedVar> op_inputs;
                    QDomElement op_in_params = operation.firstChildElement("Input_Parameters");
                    if(!op_in_params.isNull()){
                        for(    QDomElement ce = op_in_params.firstChildElement("Id");
                                !ce.isNull();
                                ce = ce.nextSiblingElement("Id") )
                        {
                            op_inputs.push_back(VarNameFromId(ce,typeInfos));
                        }
                    }

                    if(v_input.size() != op_inputs.size())
                        throw SubstReaderException("Wrong number of input parameters in call to operation "
                                + id.attribute("value").toStdString()
                                + " (Formal: " + std::to_string(op_inputs.size()) + ", "
                                + "Effective: " +std::to_string(v_input.size()) + ")");

                    if(v_output.size() != op_outputs.size())
                        throw SubstReaderException("Wrong number of output parameters in call to operation "
                                + id.attribute("value").toStdString()
                                + " (Formal: " + std::to_string(op_outputs.size()) + ", "
                                + "Effective: " +std::to_string(v_output.size()) + ")");

                    // Precondition
                    QDomElement op_pre = operation.firstChildElement("Precondition");
                    Pred pre = op_pre.isNull()?
                        Pred::makeTrue() :
                        readPredicate(op_pre.firstChildElement(),typeInfos);

                    // Body
                    QDomElement op_body = operation.firstChildElement("Body");
                    if(op_body.isNull())
                        throw SubstReaderException("Missing body in call to operation " + id.attribute("value").toStdString());

                    return Subst::makeOpCall(
                            id.attribute("value").toStdString(),
                            std::move(v_input),
                            v_output,
                            op_inputs,
                            op_outputs,
                            std::move(pre),
                            readSubstitution(op_body.firstChildElement(),typeInfos));
                }
            case Subst::SKind::While:
                {
                    QDomElement cond = dom.firstChildElement("Condition");
                    if(cond.isNull())
                        throw SubstReaderException("Missing child 'Condition' in 'While' element.");
                    QDomElement body = dom.firstChildElement("Body");
                    if(body.isNull())
                        throw SubstReaderException("Missing child 'Body' in 'While' element.");
                    QDomElement inv = dom.firstChildElement("Invariant");
                    if(inv.isNull())
                        throw SubstReaderException("Missing child 'Invariant' in 'While' element.");
                    QDomElement var = dom.firstChildElement("Variant");
                    if(var.isNull())
                        throw SubstReaderException("Missing child 'Variant' in 'While' element.");
                    return Subst::makeWhile(
                            readPredicate(cond.firstChildElement(),typeInfos),
                            readSubstitution(body.firstChildElement(),typeInfos),
                            readPredicate(inv.firstChildElement(),typeInfos),
                            readExpression(var.firstChildElement(),typeInfos) );
                }
            case Subst::SKind::Sequence:
                {
                    std::vector<Subst> vec;
                    QDomElement ce = dom.firstChildElement();
                    while (!ce.isNull()) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }
                    return Subst::makeSequence(std::move(vec));
                }
            case Subst::SKind::Parallel:
                {
                    std::vector<Subst> vec;
                    QDomElement ce = dom.firstChildElement();
                    while (!ce.isNull()) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }
                    return Subst::makeParallel(std::move(vec));
                }
            case Subst::SKind::Choice:
                {
                    std::vector<Subst> vec;
                    QDomElement ce = dom.firstChildElement();
                    while (!ce.isNull()) {
                        vec.push_back(readSubstitution(ce,typeInfos));
                        ce = ce.nextSiblingElement();
                    }
                    return Subst::makeChoice(std::move(vec));
                }
        };
        assert(false); // unreachable
    };
}
