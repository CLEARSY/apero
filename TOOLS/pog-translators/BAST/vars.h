/** vars.h

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
#ifndef VARS_H
#define VARS_H

#include <string>
#include <set>
#include <cassert>
#include "btype.h"

int mkPrefix(const std::string &s);

struct VarName {
    enum class Kind { 
        NoSuffix, // Ident without suffix (ex: toto)
        WithSuffix, // Ident with a suffix (ex: toto$1). The suffix is >= 1.
        FreshId, // FreshId (bound by a LetFreshIdent tag in the poxml)
        Tmp // Temporary Ident. Guarranted to be different from any other ident when created.
    };
    // Constructors
    static VarName makeVarWithoutSuffix(const std::string &p){ return VarName(p,-1); };
    static VarName makeVar(const std::string &p, int s){ assert(s>0); return VarName(p,s); };
    static VarName makeFreshId(const std::string &p){ return VarName(p,-2); };
    static VarName makeTmp(const std::string &p);
    // Accessors
    const std::string &prefix() const;
    int suffix() const { return _suffix; };
    Kind kind() const {
        assert(_suffix != 0);
        if(_suffix == -1) return Kind::NoSuffix;
        if(_suffix == -2) return Kind::FreshId;
        if(_suffix > 0) return Kind::WithSuffix;
        /* _suffix < -2 */
        return Kind::Tmp;
    };
    std::string show() const { // for debug
        assert(_suffix != 0);
        if(_suffix == -1) return prefix();
        if(_suffix == -2) return "_freshId_" + prefix();
        if(_suffix > 0) return prefix() + "$" + std::to_string(_suffix);
        /* _suffix < -2 */
        return "_tmpId_" + prefix() + "$" + std::to_string(-_suffix);
    }

        // Comparisons
    static int compare(const VarName &v1, const VarName& v2);
    static int vec_compare(const std::vector<VarName> &v1, const std::vector<VarName>& v2);
    inline bool operator==(const VarName& other) const { return compare(*this,other) == 0; }
    inline bool operator!=(const VarName& other) const { return compare(*this,other) != 0; }
    inline bool operator< (const VarName& other) const { return compare(*this,other) <  0; }
    inline bool operator> (const VarName& other) const { return compare(*this,other) >  0; }
    inline bool operator<=(const VarName& other) const { return compare(*this,other) <= 0; }
    inline bool operator>=(const VarName& other) const { return compare(*this,other) >= 0; }
    // Hashing
    size_t hash_combine(size_t seed) const ;
    // Misc
    static VarName getFreshVar(const std::string &prefix, const std::set<VarName> &set);
    static VarName getFreshVar(const VarName &v, const std::set<VarName> &set);
  private:
    // Members
    int _prefix;
    int _suffix; // -1 means no suffix
    //             -2 is used to managed the Fresh_Id tag
    //             0 should not be used to avoid confusion with dollar0
    //             > 0 variable with suffix (ex toto$1)
    //             < -2 temporary fresh variable
    // Constructor
    VarName(const std::string &p,int s):
        _prefix{mkPrefix(p)},
        _suffix{s}{ };
};

struct TypedVar {
    VarName name;
    BType type;
    TypedVar(const VarName &name, const BType &type):
        name{name},
        type{type}{ };

    static int compare(const TypedVar &v1, const TypedVar& v2);
    static int vec_compare(const std::vector<TypedVar> &v1, const std::vector<TypedVar>& v2);
    inline bool operator==(const TypedVar& other) const { return compare(*this,other) == 0; }
    inline bool operator!=(const TypedVar& other) const { return compare(*this,other) != 0; }
    inline bool operator< (const TypedVar& other) const { return compare(*this,other) <  0; }
    inline bool operator> (const TypedVar& other) const { return compare(*this,other) >  0; }
    inline bool operator<=(const TypedVar& other) const { return compare(*this,other) <= 0; }
    inline bool operator>=(const TypedVar& other) const { return compare(*this,other) >= 0; }

    size_t hash_combine(size_t seed) const ;
};

class Context {
    public:
        bool push(const std::vector<TypedVar> &vars1, const std::vector<TypedVar> &vars2){
            if(vars1.size() == vars2.size()){
                for(int i=0;i<vars1.size();i++){
                    if(vars1[i].name.prefix() != vars2[i].name.prefix())
                        return false;
                    if(vars1[i].type != vars2[i].type)
                        return false;
                }
                vec1.push_back(vars1);
                vec2.push_back(vars2);
                return true;
            } else {
                return false;
            }
        };
        static void get_pair(
                const std::vector<std::vector<TypedVar>> vec, const VarName &v,
                bool &found, std::pair<int,int> &pos)
        {
            for(int i=0;i<vec.size();i++){
                for(int j=0;j<vec[i].size();j++){
                    if(v == vec[i][j].name){
                        found = true;
                        pos = {i,j};
                        return;
                    }
                }
            }
            found = false;
        }
        bool equals(const VarName &v1, const VarName &v2){
            bool v1_found, v2_found;
            std::pair<int,int> p1, p2;
            get_pair(vec1,v1,v1_found,p1);
            get_pair(vec2,v2,v2_found,p2);
            if(v1_found && v2_found){
                return p1 == p2;
            } else if (!v1_found && !v2_found){
                return v1 == v2;
            } else {
                return false;
            }
        }
        void pop(){
            vec1.pop_back();
            vec2.pop_back();
        };
    private:
        std::vector<std::vector<TypedVar>> vec1;
        std::vector<std::vector<TypedVar>> vec2;
};

#endif // VARS_H
