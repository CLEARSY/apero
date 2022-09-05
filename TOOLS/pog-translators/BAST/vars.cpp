/** vars.cpp

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
#include "vars.h"
#include <unordered_map>

std::unordered_map<std::string,int> stringToPrefix;
std::vector<std::string> prefixToString;

int mkPrefix(const std::string &s){
    auto it = stringToPrefix.find(s);
    if(it == stringToPrefix.end()){
        int res = prefixToString.size();
        prefixToString.push_back(s);
        stringToPrefix[s] = res;
        return res;
    } else {
        return it->second;
    }
}

const std::string &VarName::prefix() const { return prefixToString[_prefix]; };

static int varname_cpt = -2;
VarName VarName::makeTmp(const std::string &p){ varname_cpt--; return VarName(p,varname_cpt); };

size_t VarName::hash_combine(size_t seed) const {
    return hashUtil::hash_combine_int(_suffix,hashUtil::hash_combine_string(prefix(),seed));
};

size_t TypedVar::hash_combine(size_t seed) const {
    return name.hash_combine(type.hash_combine(seed));
};

int VarName::compare(const VarName &v1, const VarName& v2){
    int i = v1._prefix - v2._prefix;
    if(i != 0) return i;
    return v1._suffix - v2._suffix;
}

int VarName::vec_compare(const std::vector<VarName> &lhs, const std::vector<VarName>& rhs) {
    if(lhs.size() == rhs.size()){
        int i = 0;
        while(i<lhs.size()){
            int res = compare(lhs.at(i),rhs.at(i));
            if(res != 0) return res;
            i++;
        }
        return 0;
    } else {
        return (lhs.size() - rhs.size());
    }
    return 0;
}

VarName VarName::getFreshVar(const std::string &prefix, const std::set<VarName> &set){
    VarName v = VarName::makeVarWithoutSuffix(prefix);
    if(set.find(v) == set.end())
        return v;
    v = VarName::makeVar(prefix,1);
    while(set.find(v) != set.end())
        v._suffix++;
    return v;
}

VarName VarName::getFreshVar(const VarName &v, const std::set<VarName> &set){
    if(set.find(v) == set.end())
        return v;

    switch(v.kind()){
        case Kind::NoSuffix:
            {
                VarName nv = VarName::makeVar(v.prefix(),1);
                while(set.find(nv) != set.end())
                    nv._suffix++;
                return nv;
            }
        case Kind::WithSuffix:
            {
                VarName nv = VarName::makeVar(v.prefix(),v.suffix()+1);
                while(set.find(nv) != set.end())
                    nv._suffix++;
                return nv;
            }
        case Kind::FreshId:
            assert(false);
        case Kind::Tmp:
            return makeTmp(v.prefix());
    };
    assert(false); // unreachable
}

int TypedVar::compare(const TypedVar &v1, const TypedVar& v2){
    int i = VarName::compare(v1.name,v2.name);
    if(i != 0) return i;
    return BType::compare(v1.type,v2.type);
}

int TypedVar::vec_compare(const std::vector<TypedVar> &lhs, const std::vector<TypedVar>& rhs) {
    if(lhs.size() == rhs.size()){
        int i = 0;
        while(i<lhs.size()){
            int res = compare(lhs.at(i),rhs.at(i));
            if(res != 0) return res;
            i++;
        }
        return 0;
    } else {
        return (lhs.size() - rhs.size());
    }
    return 0;
}
