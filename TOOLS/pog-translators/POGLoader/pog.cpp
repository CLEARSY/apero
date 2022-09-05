/** pog.cpp

   \copyright Copyright © CLEARSY 2022
   \license This file is part of POGLoader.

   POGLoader is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

    POGLoader is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with POGLoader. If not, see <https://www.gnu.org/licenses/>.
*/
#include <iostream>
#include <QFile>
#include "exprDesc.h"
#include "predDesc.h"
#include "exprReader.h"
#include "predReader.h"
#include "exprWriter.h"
#include "predWriter.h"
#include "gpredReader.h"
#include "substReader.h"
#include "pog.h"

bool isConj(const Pred &p){
    return (p.getTag() == Pred::PKind::Conjunction);
}

BType readType(const QDomElement &dom){
    if(dom.isNull())
        throw pog::PogException("Null dom element.");

    QString tag = dom.tagName();
    if(tag == "Id"){
        QString value = dom.attribute("value");
        if(value == "INTEGER"){
            return BType::INT;
        } else if(value == "FLOAT"){
            return BType::FLOAT;
        } else if(value == "REAL"){
            return BType::REAL;
        } else if(value == "STRING"){
            return BType::STRING;
        } else if(value == "BOOL"){
            return BType::BOOL;
        } else {
            if(dom.hasAttribute("suffix"))
                throw pog::PogException("Abstract or Concrete Set with suffix."); // this constraint could be removed
            return BType::INT;
        }
    } else if(tag == "Unary_Exp"){
        assert(dom.attribute("op") == "POW");
        return BType::POW(readType(dom.firstChildElement()));
    } else if(tag == "Binary_Exp"){
        assert(dom.attribute("op") == "*");
        QDomElement fst = dom.firstChildElement();
        return BType::PROD(
                readType(fst),
                readType(fst.nextSiblingElement()));
    } else if(tag == "Struct"){
        std::vector<std::pair<std::string,BType>> fields;
        for(QDomElement item = dom.firstChildElement("Record_Item");
                !item.isNull();
                item = item.nextSiblingElement("Record_Item"))
        {
            fields.push_back( {
                    item.attribute("label").toStdString(),
                    readType(item.firstChildElement()) });
        }
        return BType::STRUCT(fields);
    } else {
        throw pog::PogException("Unexpected Tag");
    }
    assert(false); // unreachable
}

void readTypeInfos(const QDomElement &dom,std::vector<BType> &typeInfos){
    assert(typeInfos.size() == 0);
    if(!dom.isNull()){
        int cpt=0;
        for(QDomElement typ = dom.firstChildElement("Type");
                !typ.isNull();
                typ = typ.nextSiblingElement())
        {
            bool ok = false;
            int typref = typ.attribute("id").toInt(&ok);
            if(!ok)
                throw pog::PogException("Integer expected");
            if(typref != cpt)
                throw pog::PogException("Unexpected typref. Expecting '" + std::to_string(cpt)
                        + "'. Found '"+ std::to_string(typref) + "'.");
            typeInfos.push_back(readType(typ.firstChildElement()));
            cpt++;
        }
    }
}

pog::Set readSet(const std::vector<BType> &typeInfos, const QDomElement &dom){
    std::vector<TypedVar> vec;
    QDomElement elt = dom.firstChildElement("Enumerated_Values").firstChildElement("Id");
    while(!elt.isNull()){
        vec.push_back(Xml::VarNameFromId(elt,typeInfos));
        elt = elt.nextSiblingElement("Id");
    }
    QDomElement id = dom.firstChildElement("Id");
    if(id.isNull())
        throw pog::PogException ("Missing 'Id' element.");
    return pog::Set(Xml::VarNameFromId(id,typeInfos), vec);
}

pog::Pog pog::read(const QDomDocument &pog){
    Pog res;
    QDomElement root = pog.firstChildElement("Proof_Obligations");
    if(root.isNull())
        throw PogException("Proof_Obligations element expected.");
    // TypeInfos
    std::vector<BType> typeInfos;
    readTypeInfos(root.firstChildElement("TypeInfos"),typeInfos);
    // Defines
    for(QDomElement e = root.firstChildElement("Define");
            !e.isNull();
            e = e.nextSiblingElement("Define"))
    {
        if(!e.hasAttribute("name"))
            throw PogException("Attribute name expected.");

        size_t hash;
        if(!e.hasAttribute("hash"))
            throw PogException("Missing hash attribute.");
        sscanf(e.attribute("hash").toStdString().c_str(), "%zu", &hash);

        auto def = Define(e.attribute("name").toStdString(),hash);
        for(QDomElement ch = e.firstChildElement();
                !ch.isNull();
                ch = ch.nextSiblingElement())
        {
            if(ch.tagName() == "Set"){
                Set s = readSet(typeInfos,ch);
                def.gsets.push_back(s);
            } else {
                Pred p = Xml::readPredicate(ch,typeInfos);
                assert(!isConj(p));
                def.ghyps.push_back(std::move(p));
            }
        }
        res.defines.push_back(std::move(def));
    }
    // Proof_Obligation
    for(QDomElement po = root.firstChildElement("Proof_Obligation");
            !po.isNull();
            po = po.nextSiblingElement("Proof_Obligation"))
    {
        // goalHash
        size_t goalHash;
        if(!po.hasAttribute("goalHash"))
            throw PogException("Missing goalHash attribute.");
        sscanf(po.attribute("goalHash").toStdString().c_str(), "%zu", &goalHash);
        // Tag
        QDomElement e = po.firstChildElement("Tag");
        if(e.isNull())
            throw PogException("Tag element expected.");
        std::string tag = e.text().toStdString();
        // Definitions
        std::vector<std::string> definitions;
        for(QDomElement e = po.firstChildElement("Definition");
                !e.isNull();
                e = e.nextSiblingElement("Definition"))
        {
            definitions.push_back(e.attribute("name").toStdString());
        }
        // Hypothesis
        std::vector<Pred> hyps;
        for(QDomElement e = po.firstChildElement("Hypothesis");
                !e.isNull();
                e = e.nextSiblingElement("Hypothesis"))
        {
            Pred p = Xml::readPredicate(e.firstChildElement(),typeInfos);
            assert(!isConj(p));
            hyps.push_back(std::move(p));
        }
        // Local Hypotheses
        std::vector<Pred> localHyps;
        for(QDomElement e = po.firstChildElement("Local_Hyp");
                !e.isNull();
                e = e.nextSiblingElement("Local_Hyp"))
        {
            Pred p = Xml::readPredicate(e.firstChildElement(),typeInfos);
            assert(!isConj(p));
            localHyps.push_back(std::move(p));
        }
        // Simple Goal
        std::vector<PO> simpleGoals;
        for(QDomElement e = po.firstChildElement("Simple_Goal");
                !e.isNull();
                e = e.nextSiblingElement("Simple_Goal"))
        {
            // Tag
            QDomElement tag = e.firstChildElement("Tag");
            std::string _tag = tag.text().toStdString();
            // Ref Hyps
            std::vector<int> _localHypRefs;
            for(QDomElement ch = e.firstChildElement("Ref_Hyp");
                    !ch.isNull();
                    ch = ch.nextSiblingElement("Ref_Hyp"))
            {
                _localHypRefs.push_back(ch.attribute("num").toInt());
            }
            // Goal
            QDomElement goal = e.firstChildElement("Goal");
            Pred _goal = Xml::readPredicate(goal.firstChildElement(),typeInfos);
            simpleGoals.push_back({_tag,_localHypRefs,std::move(_goal)});
        }
        res.pos.push_back(POGroup(tag,goalHash,definitions,std::move(hyps),std::move(localHyps),std::move(simpleGoals)));
    }
    return res;
}
