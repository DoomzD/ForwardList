#include <iostream>

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

    void print() const {
        Node* tempNode = head_;
        while (tempNode != nullptr) {
            std::cout << tempNode->data << " ";
            tempNode = tempNode->next;
        }
    }

    void reverse() {
        Node* prev = nullptr;
        Node* cur = head_;
        Node* next = nullptr;

        while (cur != nullptr) {
            next = cur->next;
            cur->next = prev;
            prev = cur;
            head_ = cur;
            cur = next;
        }
    }

    List merge(List lhs, List rhs) {
        List mergedList;
        Node* lhsNode = lhs.head_;
        Node* rhsNode = rhs.head_;

        while (lhsNode != nullptr || rhsNode != nullptr) {
            if (lhsNode == nullptr || (rhsNode != nullptr && rhsNode->data < lhsNode->data)) {
                Node *nextNode = rhsNode->next;
                mergedList.push_front(rhsNode);
                rhsNode = nextNode;
            } else {
                Node *nextNode = lhsNode->next;
                mergedList.push_front(lhsNode);
                lhsNode = nextNode;
            }
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

        *this = mergedList;
    }

    ~List() {
        if (isAllocated_) {
            while (head_ != nullptr) {
                Node* tmp = head_->next;
                delete head_;
                head_ = tmp;
            }
        }
    }

private:
    void push_front(Node* node) {
        node->next = head_;
        head_ = node;
        ++sz_;
    }

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
