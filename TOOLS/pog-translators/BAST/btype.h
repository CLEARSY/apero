/** btype.h

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
#ifndef BType_H
#define BType_H

#include <memory>
#include <algorithm>
#include <vector>
#include "hashutil.h"

class BType {
public:
    enum class Kind { INTEGER, BOOLEAN, FLOAT, REAL, STRING, ProductType, PowerType, Struct };
    Kind getKind() const { return kind; };

    class ProductType;
    class PowerType;
    class RecordType;

    const ProductType& toProductType() const;
    const PowerType& toPowerType() const;
    const RecordType& toRecordType() const;

    class Visitor {
        public:
        virtual void visitINTEGER() = 0;
        virtual void visitBOOLEAN() = 0;
        virtual void visitFLOAT() = 0;
        virtual void visitREAL() = 0;
        virtual void visitSTRING() = 0;
        virtual void visitProductType(const BType &lhs, const BType &rhs) = 0;
        virtual void visitPowerType(const BType &ty) = 0;
        virtual void visitRecordType(const std::vector<std::pair<std::string,BType>> &fields) = 0;
    };
    void accept(Visitor &v) const;

    static const BType INT;
    static const BType BOOL;
    static const BType FLOAT;
    static const BType REAL;
    static const BType STRING;

    static const BType POW_INT;
    static const BType POW_BOOL;
    static const BType POW_FLOAT;
    static const BType POW_STRING;
    static const BType POW_REAL;

    static BType PROD(const BType &lhs, const BType &rhs);
    static BType POW(const BType &content);
    static BType STRUCT(const std::vector<std::pair<std::string,BType>> &fields);

    BType():kind{Kind::INTEGER},ptr{nullptr}{}

    // Comparisons
    static int compare(const BType &v1, const BType& v2);
    static int vec_compare(const std::vector<BType> &v1, const std::vector<BType>& v2);
    inline bool operator==(const BType& other) const { return compare(*this,other) == 0; }
    inline bool operator!=(const BType& other) const { return compare(*this,other) != 0; }
    inline bool operator< (const BType& other) const { return compare(*this,other) <  0; }
    inline bool operator> (const BType& other) const { return compare(*this,other) >  0; }
    inline bool operator<=(const BType& other) const { return compare(*this,other) <= 0; }
    inline bool operator>=(const BType& other) const { return compare(*this,other) >= 0; }

    size_t hash_combine(size_t seed) const ;
    std::string to_string() const;
private:
    class AbstractBType {
        public:
            virtual void accept(Visitor &v) const = 0;
            virtual size_t hash_combine(size_t seed) const = 0;
    };
    Kind kind;
    std::shared_ptr<AbstractBType> ptr;

    BType(Kind kind, const std::shared_ptr<AbstractBType> &ptr):
        kind{kind},
        ptr{ptr}
    {};
};

class BType::ProductType : public AbstractBType {
    public:
        ProductType(const BType &lhs, const BType &rhs):lhs{lhs}, rhs{rhs}{};
        void accept(Visitor &v) const { v.visitProductType(lhs,rhs); }

        size_t hash_combine(size_t seed) const {
            return lhs.hash_combine(rhs.hash_combine(seed));
        }
        const BType lhs;
        const BType rhs;
};
class BType::PowerType : public AbstractBType {
    public:
        PowerType(const BType &content):content{content}{};

        size_t hash_combine(size_t seed) const {
            return content.hash_combine(seed);
        }
        void accept(Visitor &v) const { v.visitPowerType(content); }
        const BType content;
};
class BType::RecordType : public AbstractBType {
    public:
        RecordType(const std::vector<std::pair<std::string,BType>> &fields):
            fields{sort(fields)}{ };

        size_t hash_combine(size_t seed) const {
            size_t res = seed;
            for(auto &p : fields)
                res = hashUtil::hash_combine_string(
                        p.first,
                        p.second.hash_combine(res) );
            return res;
        }

        void accept(Visitor &v) const { v.visitRecordType(fields); }
        //
        const std::vector<std::pair<std::string,BType>> fields; // invariant: fields are sorted alphabetically
    private:
        std::vector<std::pair<std::string,BType>> sort(const std::vector<std::pair<std::string,BType>> &fields);
};

#endif
