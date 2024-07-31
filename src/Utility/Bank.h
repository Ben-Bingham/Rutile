#pragma once
#include <vector>

namespace Rutile {
    template<typename BankType, typename IndexType>
    class Bank {
    public:
        Bank() = default;

        IndexType Add(const BankType& obj) {
            m_BankObjects.push_back(obj);

            return (IndexType)m_BankObjects.size() - (IndexType)1;
        }

        BankType& operator[](IndexType i) {
            return m_BankObjects[i];
        }

        const BankType& operator[](IndexType i) const {
            return m_BankObjects[i];
        }

        size_t Size() {
            return m_BankObjects.size();
        }

    private:
        std::vector<BankType> m_BankObjects;
    };
}