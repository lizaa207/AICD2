#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <utility>

int get_random(std::mt19937& generator, int lower, int upper) {
	if (lower > upper) {
		throw std::invalid_argument("Lower bound is greater than upper bound");
	}
	std::uniform_int_distribution<int> distribution(lower, upper);
	return distribution(generator);
}

double get_random(std::mt19937& generator, double lower, double upper) {
	if (lower > upper) {
		throw std::invalid_argument("Lower bound is greater than upper bound");
	}
	std::uniform_real_distribution<double> distribution(lower, upper);
	return distribution(generator);
}

template <typename T>
class LinkedList {
public:
	LinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

	LinkedList(std::size_t count, const T& lower, const T& upper,
		unsigned int seed)
		: LinkedList() {
		std::mt19937 generator(seed);
		for (std::size_t i = 0; i < count; ++i) {
			push_tail(get_random(generator, lower, upper));
		}
	}

	LinkedList(const LinkedList& other) : LinkedList() {
		for (Node* node = other.head_; node != nullptr; node = node->next) {
			push_tail(node->value);
		}
	}

	LinkedList& operator=(const LinkedList& other) {
		if (this != &other) {
			LinkedList copy(other);
			swap(copy);
		}
		return *this;
	}

	~LinkedList() { clear(); }

	std::size_t size() const { return size_; }
	bool empty() const { return size_ == 0; }

	void push_tail(const T& value) {
		Node* node = new Node(value);
		if (tail_ != nullptr) {
			tail_->next = node;
		}
		else {
			head_ = node;
		}
		tail_ = node;
		++size_;
	}

	void push_head(const T& value) {
		Node* node = new Node(value);
		node->next = head_;
		head_ = node;
		if (tail_ == nullptr) {
			tail_ = node;
		}
		++size_;
	}

	void push_tail(const LinkedList& other) {
		LinkedList copy(other);
		if (copy.empty()) {
			return;
		}
		if (tail_ != nullptr) {
			tail_->next = copy.head_;
		}
		else {
			head_ = copy.head_;
		}
		tail_ = copy.tail_;
		size_ += copy.size_;
		copy.release();
	}

	void push_head(const LinkedList& other) {
		LinkedList copy(other);
		if (copy.empty()) {
			return;
		}
		copy.tail_->next = head_;
		head_ = copy.head_;
		if (tail_ == nullptr) {
			tail_ = copy.tail_;
		}
		size_ += copy.size_;
		copy.release();
	}

	T pop_heads() {
		if (empty()) {
			throw std::out_of_range("List is empty");
		}
		Node* old_head = head_;
		T value = old_head->value;
		head_ = old_head->next;
		if (head_ == nullptr) {
			tail_ = nullptr;
		}
		delete old_head;
		--size_;
		return value;
	}

	T pop_tails() {
		if (empty()) {
			throw std::out_of_range("List is empty");
		}
		if (size_ == 1) {
			return pop_heads();
		}
		Node* previous = head_;
		while (previous->next != tail_) {
			previous = previous->next;
		}
		T value = tail_->value;
		delete tail_;
		tail_ = previous;
		tail_->next = nullptr;
		--size_;
		return value;
	}

	void delete_node(T value) {
		Node* previous = nullptr;
		Node* current = head_;
		while (current != nullptr) {
			Node* next = current->next;
			if (current->value == value) {
				if (previous != nullptr) {
					previous->next = next;
				}
				else {
					head_ = next;
				}
				if (current == tail_) {
					tail_ = previous;
				}
				delete current;
				--size_;
			}
			else {
				previous = current;
			}
			current = next;
		}
	}

	T& operator[](std::size_t index) { return node_at(index)->value; }

	const T& operator[](std::size_t index) const {
		return node_at(index)->value;
	}

	friend std::ostream& operator<<(std::ostream& out, const LinkedList& list) {
		out << '[';
		for (Node* node = list.head_; node != nullptr; node = node->next) {
			if (node != list.head_) {
				out << ", ";
			}
			out << node->value;
		}
		out << ']';
		return out;
	}

private:
	struct Node {
		T value;
		Node* next;

		explicit Node(const T& node_value) : value(node_value), next(nullptr) {}
	};

	Node* node_at(std::size_t index) const {
		if (index >= size_) {
			throw std::out_of_range("List index out of range");
		}
		Node* node = head_;
		for (std::size_t i = 0; i < index; ++i) {
			node = node->next;
		}
		return node;
	}

	void clear() {
		while (head_ != nullptr) {
			Node* next = head_->next;
			delete head_;
			head_ = next;
		}
		tail_ = nullptr;
		size_ = 0;
	}

	void release() {
		head_ = nullptr;
		tail_ = nullptr;
		size_ = 0;
	}

	void swap(LinkedList& other) {
		std::swap(head_, other.head_);
		std::swap(tail_, other.tail_);
		std::swap(size_, other.size_);
	}

	Node* head_;
	Node* tail_;
	std::size_t size_;
};


template <typename T>
void reverse_list(LinkedList<T>& list) {
	LinkedList<T> reversed;
	while (!list.empty()) {
		reversed.push_head(list.pop_heads());
	}
	list = reversed;
}

template <typename T>
void demonstrate_list(const char* name, const T& lower, const T& upper,
	unsigned int seed) {
	std::cout << "===== " << name << " =====\n";

	LinkedList<T> empty_list;
	LinkedList<T> a(6, lower, upper, seed);
	LinkedList<T> b(3, lower, upper, seed + 1);
	std::cout << "Empty: " << empty_list << "\nA: " << a << "\nB: " << b << '\n';

	LinkedList<T> copy(a);
	copy[0] = upper;
	std::cout << "Copy of A with changed [0]: " << copy << "\nA is the same: " << a
		<< '\n';
	copy = b;
	std::cout << "Copy after assignment of B: " << copy << '\n';

	a.push_tail(upper);
	a.push_head(upper);
	std::cout << "A after push_tail and push_head of upper: " << a << '\n';
	a.push_tail(b);
	std::cout << "A after push_tail(B): " << a << '\n';
	a.push_head(b);
	std::cout << "A after push_head(B): " << a << '\n';
	std::cout << "Size of A: " << a.size() << ", A[1] = " << a[1] << '\n';

	std::cout << "pop_heads: " << a.pop_heads() << ", pop_tails: " << a.pop_tails()
		<< '\n';
	std::cout << "A after pops: " << a << '\n';

	a.delete_node(upper);
	std::cout << "A after delete_node(upper): " << a << '\n';

	try {
		std::cout << a[100] << '\n';
	}
	catch (const std::out_of_range& error) {
		std::cout << "Error: " << error.what() << '\n';
	}
	try {
		empty_list.pop_heads();
	}
	catch (const std::out_of_range& error) {
		std::cout << "Error: " << error.what() << '\n';
	}
	try {
		LinkedList<T> wrong(3, upper, lower, seed);
		std::cout << wrong << '\n';
	}
	catch (const std::invalid_argument& error) {
		std::cout << "Error: " << error.what() << '\n';
	}

	std::cout << "A before reverse: " << a << '\n';
	reverse_list(a);
	std::cout << "A after reverse:  " << a << '\n';
	reverse_list(empty_list);
	std::cout << "Empty list after reverse: " << empty_list << "\n\n";
}

int main() {
	std::random_device device;
	const unsigned int seed = device();
	std::cout << "Seed: " << seed << "\n\n";

	demonstrate_list<int>("int", 1, 9, seed);
	demonstrate_list<double>("double", -5.0, 5.0, seed);
	return 0;
}
