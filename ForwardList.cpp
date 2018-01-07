#include <iostream>
#include <cassert>

const int BIG_INT = (1ll << 30) - 1;

struct Node {
    int data = 0;
    Node* next = nullptr;

    explicit Node(int value, Node* newNode) : data(value), next(newNode) {}
};

class List {
public:
    List() : head_(nullptr), sz_(0), isAllocated_(0) {}

    void push_front(int x) {
        isAllocated_ = true;
        Node* newNode = new Node(x, head_);
        head_ = newNode;
        ++sz_;
    }

    void push_front(Node* newNode) {
        newNode->next = head_;
        head_ = newNode;
        ++sz_;
    }

    int size() {
        return sz_;
    }

    void print() const {
        Node* tempNode = head_;
        while (tempNode != nullptr) {
            std::cout << tempNode->data << " ";
            tempNode = tempNode->next;
        }
        std::cout << std::endl;
    }

    void reverse() {
        Node* prevNode = nullptr;
        Node* curNode = head_;
        Node* nextNode = nullptr;

        while (curNode != nullptr) {
            nextNode = curNode->next;
            curNode->next = prevNode;
            prevNode = curNode;
            head_ = curNode;
            curNode = nextNode;
        }
    }

    List merge(List &lhs, List &rhs) {
        List mergedList;
        Node* lhsNode = lhs.head_;
        Node* rhsNode = rhs.head_;

        /// Как здесь уменьшить количество копирований?

        while (lhsNode != nullptr && rhsNode != nullptr) {
            if (lhsNode->data < rhsNode->data) {
                Node* nextNode = lhsNode->next;
                mergedList.push_front(lhsNode);
                lhsNode = nextNode;
            } else {
                Node* nextNode = rhsNode->next;
                mergedList.push_front(rhsNode);
                rhsNode = nextNode;
            }
        }
        while (lhsNode != nullptr) {
            Node* nextNode = lhsNode->next;
            mergedList.push_front(lhsNode);
            lhsNode = nextNode;
        }
        while (rhsNode != nullptr) {
            Node* nextNode = rhsNode->next;
            mergedList.push_front(rhsNode);
            rhsNode = nextNode;
        }

        return mergedList;
    }

    void sort() {
        if (sz_ < 2) return;

        List lhs;
        List rhs;

        Node* head = head_;

        int middle = sz_ / 2;

        Node* curNode = head_;
        while (lhs.sz_ < middle) {
            Node* nextNode = curNode->next;
            lhs.push_front(curNode);
            curNode = nextNode;

        }
        while (rhs.sz_ + lhs.sz_ < sz_) {
            Node* nextNode = curNode->next;
            rhs.push_front(curNode);
            curNode = nextNode;
        }

        rhs.sort();
        lhs.sort();

        List mergedList = merge(lhs, rhs);
        mergedList.reverse();

        head_ = mergedList.head_;
        Node* thisCurNode = head_;
        Node* mergedCurNode = mergedList.head_;

        while (thisCurNode != nullptr) {
            thisCurNode->data = mergedCurNode->data;

            thisCurNode = thisCurNode->next;
            mergedCurNode = mergedCurNode->next;
        }
    }

    ~List() {
        while (head_ != nullptr) {
            if (isAllocated_) {
                Node* tmp = head_->next;
                delete head_;
                head_ = tmp;
            } else {
                break;
            }
        }
    }

private:
    Node* head_;
    int sz_;
    bool isAllocated_;
};

int main() {
    int n;
    std::cin >> n;

    List list;
    for (int i = 0; i < n; ++i) {
        int num;
        std::cin >> num;
        list.push_front(num);
    }

    list.sort();

    list.print();
}
