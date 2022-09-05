/** predWriter.cpp

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
#include "exprWriter.h"
#include "predWriter.h"

namespace Xml {

    class PredWriterVisitor : public Pred::Visitor {
        public:
            void visitImplication(const Pred &lhs, const Pred &rhs){
                stream.writeStartElement("Binary_Pred");
                stream.writeAttribute("op","=>");
                lhs.accept(*this);
                rhs.accept(*this);
                stream.writeEndElement(); // Binary_Pred
            };
            void visitEquivalence(const Pred &lhs, const Pred &rhs){
                stream.writeStartElement("Binary_Pred");
                stream.writeAttribute("op","<=>");
                lhs.accept(*this);
                rhs.accept(*this);
                stream.writeEndElement(); // Binary_Pred
            };
            void visitExprComparison(Pred::ComparisonOp op, const Expr &lhs, const Expr &rhs){
                stream.writeStartElement("Exp_Comparison");
                stream.writeAttribute("op",QString::fromStdString(Pred::to_string(op)));
                writeExpression(stream,typeInfos,lhs);
                writeExpression(stream,typeInfos,rhs);
                stream.writeEndElement(); // Exp_Comparison
            };
            void visitNegation(const Pred &p){
                stream.writeStartElement("Unary_Pred");
                stream.writeAttribute("op","not");
                p.accept(*this);
                stream.writeEndElement(); // Unary_Pred
            };
            void visitConjunction(const std::vector<Pred> &vec){
                stream.writeStartElement("Nary_Pred");
                stream.writeAttribute("op","&");
                for(auto &p : vec)
                    p.accept(*this);
                stream.writeEndElement(); // Nary_Pred
            };
            void visitDisjunction(const std::vector<Pred> &vec){
                stream.writeStartElement("Nary_Pred");
                stream.writeAttribute("op","or");
                for(auto &p : vec)
                    p.accept(*this);
                stream.writeEndElement(); // Nary_Pred
            };
            void visitForall(const std::vector<TypedVar> &vars, const Pred &p){
                stream.writeStartElement("Quantified_Pred");
                stream.writeAttribute("type","!");
                stream.writeStartElement("Variables");
                for(auto &v : vars)
                    writeTypedVar(stream,typeInfos,v);
                stream.writeEndElement(); // Variables
                stream.writeStartElement("Body");
                p.accept(*this);
                stream.writeEndElement(); // Body
                stream.writeEndElement(); // Quantified_Pred
            };
            void visitExists(const std::vector<TypedVar> &vars, const Pred &p){
                stream.writeStartElement("Quantified_Pred");
                stream.writeAttribute("type","#");
                stream.writeStartElement("Variables");
                for(auto &v : vars)
                    writeTypedVar(stream,typeInfos,v);
                stream.writeEndElement(); // Variables
                stream.writeStartElement("Body");
                p.accept(*this);
                stream.writeEndElement(); // Body
                stream.writeEndElement(); // Quantified_Pred
            };
            void visitTrue(){
                stream.writeStartElement("Nary_Pred");
                stream.writeAttribute("op","&");
                stream.writeEndElement(); // Nary_Pred
            };
            void visitFalse(){
                stream.writeStartElement("Nary_Pred");
                stream.writeAttribute("op","or");
                stream.writeEndElement(); // Nary_Pred
            };
            PredWriterVisitor(QXmlStreamWriter &s,std::map<BType,unsigned int> &typeInfos):
                stream{s},
                typeInfos{typeInfos}
            {};
        private:
            QXmlStreamWriter &stream;
            std::map<BType,unsigned int> &typeInfos;
    };

    void writePredicate(QXmlStreamWriter &stream, std::map<BType,unsigned int> &typeInfos, const Pred &p){
        PredWriterVisitor v(stream, typeInfos);
        p.accept(v);
    }
}
