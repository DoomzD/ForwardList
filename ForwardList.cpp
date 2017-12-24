#include <iostream>

struct Node {
    int data = 0;
    Node * next = nullptr;
};

class List {
public:
    List() : head(nullptr), sz(0) {}

    void add(const int x) {
        auto * tmp = new Node;
        tmp->data = x;
        tmp->next = head;
        head = tmp;
        ++sz;
    }

    void show() const {
        Node * tmp = head;
        while (tmp != nullptr) {
            std::cout << tmp->data << " ";
            tmp = tmp->next;
        }
    }

    void merge(List &left, List &right) {
        List tmp;
        Node * leftNode = left.head;
        Node * rightNode = right.head;

        while (leftNode != nullptr && rightNode != nullptr) {
            if (leftNode->data < rightNode->data) {
                tmp.add(leftNode->data);
                leftNode = leftNode->next;
            } else {
                tmp.add(rightNode->data);
                rightNode = rightNode->next;
            }
        }
        while (leftNode != nullptr) {
            tmp.add(leftNode->data);
            leftNode = leftNode->next;
        }
        while (rightNode != nullptr) {
            tmp.add(rightNode->data);
            rightNode = rightNode->next;
        }

        Node * prev = nullptr;
        Node * tmpNode = tmp.head;
        while (1) {
            Node * next = this->head->next;
            this->head->next = prev;
            this->head->data = tmpNode->data;
            tmpNode = tmpNode->next;
            prev = this->head;
            if (next == nullptr) break;
            this->head = next;

        }
    }

    void sort() {
        if (sz < 2) return;

        List left;
        List right;

        int middle = sz / 2;

        Node * tmp = head;
        while (left.sz < middle) {
            left.add(tmp->data);
            tmp = tmp->next;

        }
        while (right.sz + left.sz < sz) {
            right.add(tmp->data);
            tmp = tmp->next;
        }

        right.sort();
        left.sort();

        merge(left, right);
    }

    ~List() {
        while (head != nullptr) {
            Node * tmp = head->next;
            delete head;
            head = tmp;
        }
    }

private:
    Node * head;
    int sz;
};

int main() {
    int n;
    std::cin >> n;

    List list;
    for (int i = 0; i < n; ++i) {
        int num;
        std::cin >> num;
        list.add(num);
    }

    list.sort();

    list.show();
}
