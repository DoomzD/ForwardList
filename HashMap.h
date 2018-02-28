#include <iostream>
#include <list>
#include <vector>
#include <stdexcept>

using namespace std;

using std::pair;
using std::list;
using std::vector;

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
    using value_type = pair<const KeyType, ValueType>;
    using hash_table = list<value_type>;
    using iterator = typename list<value_type>::iterator;
    using const_iterator = typename list<value_type>::const_iterator;

    HashMap(const Hash& hasher = Hash())
            : hasher_(hasher), size_(0), buckets_(2) {
        rebuild(buckets_);
    }

    template<typename ForwardIterator>
    HashMap(ForwardIterator begin, ForwardIterator end, const Hash& hasher = Hash())
            : hasher_(hasher), size_(0), buckets_(2) {
        while (begin != end) {
            insert_(*begin++);
        }
    }

    using InitializerIterator = typename std::initializer_list<value_type>::iterator;
    HashMap(const std::initializer_list<value_type>& init_list, const Hash& hasher = Hash())
            : hasher_(hasher), size_(0), buckets_(2) {
        InitializerIterator begin = init_list.begin();
        InitializerIterator end = init_list.end();

        while (begin != end) {
            insert_(*begin++);
        }
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

        if (size_ > buckets_ * 2 / 3) {
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

        if (size_ < buckets_ / 4) {
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
        size_t pos = hasher_(key) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (used_[pos]) return ind_[pos]->second;

        pair<KeyType, ValueType> temp;
        temp.first = key;
        value_type newElem(temp);
        table_.emplace_back(newElem);
        ind_[pos] = --table_.end();
        used_[pos] = true;

        ++size_;
        if (size_ > buckets_ * 2 / 3) {
            rebuild(buckets_ * 2);
            pos = hasher_(key) % buckets_;
            while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
                pos = (pos + 1 == buckets_ ? 0 : pos + 1);
            }
        }

        return ind_[pos]->second;
    }

    const ValueType& at(const KeyType& key) const {
        size_t pos = hasher_(key) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == key)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        if (used_[pos]) return ind_[pos]->second;
        throw std::out_of_range("no such element");
    }

    void clear() {
        table_.clear();
        size_ = 0;
        buckets_ = 2;
        rebuild(buckets_);
    }

    HashMap& operator = (const HashMap& other) {
        if (this == &other) return *this;
        table_.clear();
        buckets_ = other.buckets_;
        size_ = other.size_;
        rebuild(buckets_);

        auto it = other.begin();
        while (it != other.end()) {
            table_.emplace_back(*it++);
            size_t pos = hasher_(table_.back().first) % buckets_;
            while (used_[pos]) {
                pos = (pos + 1 == buckets_ ? 0 : pos + 1);
            }
            ind_[pos] = --table_.end();
            used_[pos] = true;
        }

        return *this;
    }

    hash_table& operator = (const hash_table& other) {
        this->clear();
        auto it = other.begin();
        while (it != other.end()) {
            table_.emplace_back(*it++);
        }

        return table_;
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
//        cout << size_ << ":" << buckets_ << '\n';
        hash_table temp = table_;
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

    void insert_(value_type elem) {
        size_t pos = hasher_(elem.first) % buckets_;
        while ((used_[pos] && !(ind_[pos]->first == elem.first)) || erased_[pos]) {
            pos = (pos + 1 == buckets_ ? 0 : pos + 1);
        }

        used_[pos] = true;
        erased_[pos] = false;

        table_.emplace_back(elem);
        ind_[pos] = --table_.end();
        ++size_;

        if (size_ > buckets_ * 2 / 3) {
            rebuild(buckets_ * 2);
        }
    }

    Hash hasher_;
    hash_table table_;
    vector<iterator> ind_ = {table_.end(), table_.end()};
    vector<bool> used_ = {false, false};
    vector<bool> erased_ = {false, false};
    size_t size_;
    unsigned int buckets_;
};
