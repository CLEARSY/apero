/** exprWriter.cpp

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
#include "exprDesc.h"

namespace Xml {
    unsigned int getTypRef(std::map<BType,unsigned int> &typeInfos, const BType &ty){
        auto it = typeInfos.find(ty);
        if(it == typeInfos.end()){
            unsigned int i = typeInfos.size();
            typeInfos.insert({ty,i});
            return i;
        } else {
            return it->second;
        }
    }

    void writeExprAttributes(
            const BType &type,
            const QStringList &bxmlTag,
            QXmlStreamWriter &stream,
            std::map<BType,unsigned int> &typeInfos)
    {
        stream.writeAttribute("typref",QString::number(getTypRef(typeInfos,type)));
        if(!bxmlTag.empty()){
            stream.writeAttribute("tag",bxmlTag.join(","));
        }
    }

    void writeTypedVar(QXmlStreamWriter &stream, std::map<BType,unsigned int> &typeInfos, const TypedVar &v){
        stream.writeStartElement("Id");
        stream.writeAttribute("value",QString::fromStdString(v.name.prefix()));
        switch(v.name.kind()){
            case VarName::Kind::NoSuffix:
                break;
            case VarName::Kind::WithSuffix:
                stream.writeAttribute("suffix",QString::number(v.name.suffix()));
                break;
            case VarName::Kind::FreshId:
                assert(false);
                break;
            case VarName::Kind::Tmp:
                assert(false);
                break;
        }
        stream.writeAttribute("typref",QString::number(getTypRef(typeInfos,v.type)));
        stream.writeEndElement(); // Id
    }

    class ExprWriterVisitor : public Expr::Visitor {
        public:
            void visitConstant(const BType &type, const QStringList &bxmlTag,Expr::Visitor::EConstant c){
                switch (c){
                    case Expr::Visitor::EConstant::MaxInt:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","MAXINT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::MinInt:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","MININT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::INTEGER:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","INTEGER");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NATURAL:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","NATURAL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NATURAL1:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","NATURAL1");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::INT:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","INT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NAT:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","NAT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::NAT1:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","NAT1");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::STRING:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","STRING");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::BOOL:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","BOOL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::REAL:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","REAL");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::FLOAT:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","FLOAT");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::TRUE:
                        {
                            stream.writeStartElement("Boolean_Literal");
                            stream.writeAttribute("value","TRUE");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Boolean_Literal
                            break;
                        }
                    case Expr::Visitor::EConstant::FALSE:
                        {
                            stream.writeStartElement("Boolean_Literal");
                            stream.writeAttribute("value","FALSE");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Boolean_Literal
                            break;
                        }
                    case Expr::Visitor::EConstant::EmptySet:
                        {
                            stream.writeStartElement("EmptySet");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // EmptySet
                            break;
                        }
                    case Expr::Visitor::EConstant::Successor:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","succ");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                    case Expr::Visitor::EConstant::Predecessor:
                        {
                            stream.writeStartElement("Id");
                            stream.writeAttribute("value","pred");
                            writeExprAttributes(type,bxmlTag,stream,typeInfos);
                            stream.writeEndElement(); // Id
                            break;
                        }
                }
            }
            void visitIntegerLiteral(const BType &type, const QStringList &bxmlTag,const std::string &i){
                stream.writeStartElement("Integer_Literal");
                stream.writeAttribute("value",QString::fromStdString(i));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.writeEndElement(); // Integer_Literal
            }
            void visitStringLiteral(const BType &type, const QStringList &bxmlTag,const std::string &b){
                stream.writeStartElement("STRING_Literal");
                stream.writeAttribute("value",QString::fromStdString(b));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.writeEndElement(); // STRING_Literal
            }
            void visitRealLiteral(const BType &type, const QStringList &bxmlTag,const Expr::Decimal &d){
                stream.writeStartElement("Real_Literal");
                stream.writeAttribute("value",QString::fromStdString(d.integerPart) + "." + QString::fromStdString(d.fractionalPart));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.writeEndElement(); // Real_Literal
            }
            void visitIdent(const BType &type, const QStringList &bxmlTag, const VarName &v){
                stream.writeStartElement("Id");
                stream.writeAttribute("value",QString::fromStdString(v.prefix()));
                switch(v.kind()){
                    case VarName::Kind::NoSuffix:
                        break;
                    case VarName::Kind::WithSuffix:
                        stream.writeAttribute("suffix",QString::number(v.suffix()));
                        break;
                    case VarName::Kind::FreshId:
                        assert(false);
                        break;
                    case VarName::Kind::Tmp:
                        assert(false);
                        break;
                }
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                stream.writeEndElement(); // Id
            }
            void visitUnaryExpression(const BType &type, const QStringList &bxmlTag,Expr::UnaryOp op,const Expr &e){
                stream.writeStartElement("Unary_Exp");
                stream.writeAttribute("op",QString::fromStdString(Expr::to_string(op)));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                e.accept(*this);
                stream.writeEndElement(); // Unary_Exp
            }
            void visitBinaryExpression(const BType &type, const QStringList &bxmlTag,Expr::BinaryOp op, const Expr &lhs, const Expr &rhs){
                stream.writeStartElement("Binary_Exp");
                stream.writeAttribute("op",QString::fromStdString(Expr::to_string(op)));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                lhs.accept(*this);
                rhs.accept(*this);
                stream.writeEndElement(); // Binary_Exp
            }
            void visitTernaryExpression(const BType &type, const QStringList &bxmlTag,Expr::TernaryOp op, const Expr &fst, const Expr &snd, const Expr &thd){
                stream.writeStartElement("Ternary_Exp");
                stream.writeAttribute("op",QString::fromStdString(Expr::to_string(op)));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                fst.accept(*this);
                snd.accept(*this);
                thd.accept(*this);
                stream.writeEndElement(); // Ternary_Exp
            }
            void visitNaryExpression(const BType &type, const QStringList &bxmlTag,Expr::NaryOp op, const std::vector<Expr> &vec){
                stream.writeStartElement("Nary_Exp");
                stream.writeAttribute("op",QString::fromStdString(Expr::to_string(op)));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &e : vec)
                    e.accept(*this);
                stream.writeEndElement(); // Nary_Exp
            }
            void visitBooleanExpression(const BType &type, const QStringList &bxmlTag,const Pred &p){
                stream.writeStartElement("Boolean_Exp");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                writePredicate(stream,typeInfos,p);
                stream.writeEndElement(); // Boolean_Exp
            }
            void visitRecord(const BType &type, const QStringList &bxmlTag,const std::vector<std::pair<std::string,Expr>> &fds){
                stream.writeStartElement("Record");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &pair : fds){
                    stream.writeStartElement("Record_Item");
                    stream.writeAttribute("label",QString::fromStdString(pair.first));
                    pair.second.accept(*this);
                    stream.writeEndElement(); // Record_Item
                };
                stream.writeEndElement(); // Record
            }
            void visitStruct(const BType &type, const QStringList &bxmlTag,const std::vector<std::pair<std::string,Expr>> &fds){
                stream.writeStartElement("Struct");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                for(auto &pair : fds){
                    stream.writeStartElement("Record_Item");
                    stream.writeAttribute("label",QString::fromStdString(pair.first));
                    pair.second.accept(*this);
                    stream.writeEndElement(); // Record_Item
                };
                stream.writeEndElement(); // Struct
            }
            void visitQuantifiedExpr(const BType &type, const QStringList &bxmlTag,Expr::QuantifiedOp op,const std::vector<TypedVar> vars,const Pred &cond, const Expr &body){
                stream.writeStartElement("Quantified_Exp");
                stream.writeAttribute("type",QString::fromStdString(Expr::to_string(op)));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);

                stream.writeStartElement("Variables");
                for(auto &id : vars)
                    writeTypedVar(stream,typeInfos,id);
                stream.writeEndElement(); // Variables

                stream.writeStartElement("Pred");
                writePredicate(stream,typeInfos,cond);
                stream.writeEndElement(); // Pred

                stream.writeStartElement("Body");
                body.accept(*this);
                stream.writeEndElement(); // Body

                stream.writeEndElement(); // Quantified_Exp
            }
            void visitQuantifiedSet(const BType &type, const QStringList &bxmlTag,const std::vector<TypedVar> vars, const Pred &cond){
                stream.writeStartElement("Quantified_Set");
                writeExprAttributes(type,bxmlTag,stream,typeInfos);

                stream.writeStartElement("Variables");
                for(auto &id : vars)
                    writeTypedVar(stream,typeInfos,id);
                stream.writeEndElement(); // Variables

                stream.writeStartElement("Body");
                writePredicate(stream,typeInfos,cond);
                stream.writeEndElement(); // Body

                stream.writeEndElement(); // Quantified_Set
            };

            void visitRecordUpdate(const BType &type, const QStringList &bxmlTag, const Expr &rec, const std::string &label, const Expr &value){
                stream.writeStartElement("Record_Update");
                stream.writeAttribute("label",QString::fromStdString(label));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                rec.accept(*this);
                value.accept(*this);
                stream.writeEndElement(); // Record_Update
            }

            void visitRecordAccess(const BType &type, const QStringList &bxmlTag, const Expr &rec, const std::string &label){
                stream.writeStartElement("Record_Field_Access");
                stream.writeAttribute("label",QString::fromStdString(label));
                writeExprAttributes(type,bxmlTag,stream,typeInfos);
                rec.accept(*this);
                stream.writeEndElement(); // Record_Field_Access
            }

            ExprWriterVisitor(QXmlStreamWriter &s, std::map<BType,unsigned int> &typeInfos):
                stream{s},
                typeInfos{typeInfos}
            {};
        private:
            QXmlStreamWriter &stream;
            std::map<BType,unsigned int> &typeInfos;
    };

    void writeExpression(QXmlStreamWriter &stream, std::map<BType,unsigned int> &typeInfos, const Expr &p){
        ExprWriterVisitor v(stream,typeInfos);
        p.accept(v);
    }
}
