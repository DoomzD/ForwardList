#include <iostream>
#include <list>
#include <vector>
#include <stdexcept>

using namespace std;

using std::pair;
using std::list;
using std::vector;

const double MAX_LOAD_FACTOR = 2. / 3.;
const double MIN_LOAD_FACTOR = 1. / 4.;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using value_type = pair<const KeyType, ValueType>;
    using hash_table = list<value_type>;
    using iterator = typename list<value_type>::iterator;
    using const_iterator = typename list<value_type>::const_iterator;

    friend void swap(HashMap& lhs, HashMap& rhs) {
        using std::swap;

        swap(lhs.hasher_, rhs.hasher_);
        swap(lhs.table_, rhs.table_);
        swap(lhs.ind_, rhs.ind_);
        swap(lhs.used_, rhs.used_);
        swap(lhs.erased_, rhs.erased_);
        swap(lhs.size_, rhs.size_);
        swap(lhs.buckets_, rhs.buckets_);
    }

    HashMap(const Hash& hasher = Hash())
            : hasher_(hasher), size_(0), buckets_(1) {
        rebuild(buckets_);
    }

    HashMap(const HashMap<KeyType, ValueType, Hash>& other)
            : table_(other.table_) {
        hasher_ = other.hasher_;
        ind_ = other.ind_;
        used_ = other.used_;
        erased_ = other.erased_;
        size_ = other.size_;
        buckets_ = other.buckets_;
    }

    HashMap(HashMap<KeyType, ValueType, Hash>&& other)
            : HashMap() {
        swap(*this, other);
    }

    template<typename ForwardIterator>
    HashMap(ForwardIterator begin, ForwardIterator end, const Hash& hasher = Hash())
            : hasher_(hasher), size_(0), buckets_(1) {
        while (begin != end) {
            insert(*begin++);
        }
    }

    HashMap(const std::initializer_list<value_type>& init_list, const Hash& hasher = Hash())
            : HashMap(init_list.begin(), init_list.end(), hasher) {}

    HashMap& operator = (HashMap<KeyType, ValueType, Hash> other) {
        swap(*this, other);
        return *this;
    }

    HashMap& operator = (HashMap<KeyType, ValueType, Hash>&& other) {
        swap(*this, other);
        return *this;
    }

    void insert(value_type elem) {
        size_t pos = hasher_(elem.first) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == elem.first)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (used_[pos]) return;
        used_[pos] = true;
        erased_[pos] = false;

        table_.emplace_back(elem);
        ind_[pos] = --table_.end();
        ++size_;

        if (size_ > buckets_ * MAX_LOAD_FACTOR) {
            rebuild(buckets_ * 2);
        }
    }

    void erase(const KeyType& key) {
        size_t pos = hasher_(key) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (!used_[pos]) return;

        table_.erase(ind_[pos]);
        used_[pos] = false;
        erased_[pos] = true;
        --size_;

        if (size_ < buckets_ * MIN_LOAD_FACTOR) {
            rebuild(buckets_ / 2);
        }
    }

    iterator begin() {
        return table_.begin();
    }

    const_iterator begin() const {
        return table_.cbegin();
    }

    iterator end() {
        return table_.end();
    }

    const_iterator end() const {
        return table_.cend();
    }

    const_iterator find(const KeyType& key) const {
        size_t pos = hasher_(key) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (used_[pos]) return ind_[pos];
        return end();
    }

    iterator find(const KeyType& key) {
        size_t pos = hasher_(key) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (used_[pos]) return ind_[pos];
        return end();
    }

    ValueType& operator [] (const KeyType& key) {
        iterator found = find(key);
        if (found != end()) {
            return found->second;
        }

        insert({key, ValueType()});
        found = find(key);
        return found->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator found = find(key);
        if (found != end())
            return found->second;
        throw std::out_of_range("no such element");
    }

    void clear() {
        table_.clear();
        size_ = 0;
        buckets_ = 2;
        rebuild(buckets_);
    }

    size_t size() const {
        return size_;
    }

    bool empty() const {
        return size_ == 0;
    }

    Hash hash_function() const {
        return hasher_;
    }

private:
    void rebuild(unsigned int sz) {
        hash_table temp(table_);
        table_.clear();
        buckets_ = sz;
        vector<iterator> newInd(buckets_, table_.end());
        vector<bool> newUsed(buckets_, false);
        vector<bool> newErased(buckets_, false);

        for (auto value : temp) {
            table_.emplace_back(value);
            size_t pos = hasher_(value.first) % buckets_;
            while (newUsed[pos]) {
                pos = (pos + 1 == buckets_ ? 0 : pos + 1);
            }
            newInd[pos] = --table_.end();
            newUsed[pos] = true;
        }

        ind_ = newInd;
        used_ = newUsed;
        erased_ = newErased;
    }

    Hash hasher_;
    hash_table table_;
    vector<iterator> ind_ = {table_.end()};
    vector<bool> used_ = {false};
    vector<bool> erased_ = {false};
    size_t size_;
    unsigned int buckets_;
};
