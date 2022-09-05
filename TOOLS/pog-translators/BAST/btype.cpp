/** btype.cpp

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
#include <cassert>
#include <algorithm>
#include "btype.h"

void BType::accept(Visitor &v) const {
    switch(kind){
        case Kind::INTEGER:
            v.visitINTEGER();
            break;
        case Kind::BOOLEAN:
            v.visitBOOLEAN();
            break;
        case Kind::FLOAT:
            v.visitFLOAT();
            break;
        case Kind::REAL:
            v.visitREAL();
            break;
        case Kind::STRING:
            v.visitSTRING();
            break;
        case Kind::ProductType:
        case Kind::PowerType:
        case Kind::Struct:
            ptr->accept(v);
    }
};

const BType::ProductType& BType::toProductType() const{
  assert(kind == Kind::ProductType);
  return static_cast<ProductType&>(*ptr);
};

const BType::PowerType& BType::toPowerType() const{
  assert(kind == Kind::PowerType);
  return static_cast<PowerType&>(*ptr);
};

const BType::RecordType& BType::toRecordType() const{
  assert(kind == Kind::Struct);
  return static_cast<RecordType&>(*ptr);
};

const BType BType::INT = BType(Kind::INTEGER,nullptr);
const BType BType::BOOL = BType(Kind::BOOLEAN,nullptr);
const BType BType::FLOAT = BType(Kind::FLOAT,nullptr);
const BType BType::REAL = BType(Kind::REAL,nullptr);
const BType BType::STRING = BType(Kind::STRING,nullptr);

const BType BType::POW_INT = POW(INT);
const BType BType::POW_BOOL = POW(BOOL);
const BType BType::POW_FLOAT = POW(FLOAT);
const BType BType::POW_STRING = POW(STRING);
const BType BType::POW_REAL = POW(REAL);

BType BType::PROD(const BType &lhs,const BType &rhs){
    return BType(Kind::ProductType,std::make_shared<ProductType>(ProductType(lhs,rhs)));
};
BType BType::POW(const BType &content){
    return BType(Kind::PowerType,std::make_shared<PowerType>(PowerType(content)));
};
BType BType::STRUCT(const std::vector<std::pair<std::string,BType>> &fields){
    return BType(Kind::Struct,std::make_shared<RecordType>(RecordType(fields)));
}

int compare_field_vec(const std::vector<std::pair<std::string,BType>>& lhs, const std::vector<std::pair<std::string,BType>>& rhs){
    if(lhs.size() == rhs.size()){

        struct {
            bool operator()(const std::pair<std::string,BType> &a, const std::pair<std::string,BType> &b) const
            {
                return a.first < b.first;
            }
        } lessThan;

        std::vector<std::pair<std::string,BType>> sorted_lhs(lhs.size());
        partial_sort_copy(begin(lhs), end(lhs), begin(sorted_lhs), end(sorted_lhs),lessThan);

        std::vector<std::pair<std::string,BType>> sorted_rhs(rhs.size());
        partial_sort_copy(begin(rhs), end(rhs), begin(sorted_rhs), end(sorted_rhs),lessThan);

        int i = 0;
        while(i<lhs.size()){
            auto &p1 = sorted_lhs.at(i);
            auto &p2 = sorted_rhs.at(i);
            int res = p1.first.compare(p2.first);
            if(res != 0) return res;
            res = BType::compare(p1.second,p2.second);
            if(res != 0) return res;
            i++;
        }
        return 0;
    } else {
        return (lhs.size() - rhs.size());
    }
};

int BType::compare(const BType &ty1, const BType& ty2){
    if(ty1.kind == ty2.kind){
        switch(ty1.kind){
            case Kind::INTEGER:
            case Kind::BOOLEAN:
            case Kind::STRING:
            case Kind::FLOAT:
            case Kind::REAL:
                return 0;
            case Kind::PowerType:
                return compare(ty1.toPowerType().content,ty2.toPowerType().content);
            case Kind::ProductType:
                {
                    auto &pr1 = ty1.toProductType();
                    auto &pr2 = ty2.toProductType();
                    int res = compare(pr1.lhs,pr2.lhs);
                    if(res == 0)
                        return compare(pr1.rhs,pr2.rhs);
                    else
                        return res;
                }
            case Kind::Struct:
                return compare_field_vec(ty1.toRecordType().fields,ty2.toRecordType().fields);
        }
    }
    else if (ty1.kind < ty2.kind){
        return -1;
    } else {
        return 1;
    }
    assert(false); // unreachable
}

size_t BType::hash_combine(size_t seed) const {
    if(ptr != nullptr)
        seed = ptr->hash_combine(seed);
    return hashUtil::hash_combine_int(static_cast<int>(kind),seed);
}

struct {
    bool operator()(const std::pair<std::string,BType> &a, const std::pair<std::string,BType> &b)
    {
        return a.first < b.first;
    }
} BTypeRecordFieldCmp;

std::vector<std::pair<std::string,BType>> BType::RecordType::sort(const std::vector<std::pair<std::string,BType>> &fields){
    std::vector<std::pair<std::string,BType>> res { fields };
    std::sort(res.begin(), res.end(), BTypeRecordFieldCmp);
    return res;
}
std::string BType::to_string() const {
    switch(kind){
        case Kind::INTEGER:
            return "INT";
        case Kind::BOOLEAN:
            return "BOOL";
        case Kind::REAL:
            return "REAL";
        case Kind::FLOAT:
            return "FLOAT";
        case Kind::STRING:
            return "STRING";
        case Kind::PowerType:
            return "POW(" + toPowerType().content.to_string() + ")";
        case Kind::ProductType:
            return "PROD("
                + toProductType().lhs.to_string() + ", "
                + toProductType().rhs.to_string() + ")";
        case Kind::Struct:
            {
                std::string accu = "STRUCT(";
                for(auto &fd : toRecordType().fields)
                    accu += fd.first + ":" + fd.second.to_string() + ", ";
                return accu + ")";
            }
    }
    assert(false); // unreachable
}
