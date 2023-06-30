
#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP
#include "linked_list.h"

//node

template<typename T>
node<T>::node(T* address, node* prior) {
	this->address = address;
	this->prior = prior;
	this->next = nullptr;
}

template<typename T>
void node<T>::attach_next(node* next) {
	this->next = next;
	next->prior = this;
}

template<typename T>
void node<T>::attach_prior(node* prior) {
	this->prior = prior;
	prior->next = this;
}

template<typename T>
linked_list<T>::linked_list() {
	head = nullptr;
	tail = nullptr;
	size = 0;
}

//linked list
template<typename T>
void linked_list<T>::push(T* ptr) {
	if (tail == nullptr) {
		tail = (node<T>*)malloc(sizeof(node<T>));
		*tail = node<T>(ptr);
		head = tail;
	}
	else {
		node<T>* new_tail = (node<T>*)malloc(sizeof(node<T>));
		*new_tail = node<T>(ptr);
		tail->attach_next(new_tail);
		tail = new_tail;
	}

}
template<typename T>
T* linked_list<T>::pop() {
	if (tail == nullptr) {
		return nullptr;
	}
	if (tail->prior == nullptr) {
		T* ptr = tail->address;
		free(tail);
		tail = nullptr;
		head = nullptr;
		return ptr;
	}

	node<T>* temp = tail->prior;
	T* ptr = tail->address;
	free(tail);
	tail = temp;
	return ptr;
}
template<typename T>
void linked_list<T>::destroy(const char* action) {
	if (tail == nullptr) {
		return;
	}
	T* ptr = this->pop();
	if (action = "t") {
		free(ptr);
	}
	destroy(action);
}
template<typename T>
void linked_list<T>::reverse() {
	node<T>* previous = tail;
	node<T>* current = tail->prior;
	tail->prior = nullptr;

	while (current != nullptr) {
		node* next = current->prior;
		previous->next = current;
		current->prior = previous;
		current->next = nullptr;
		previous = current;
		current = next;
	}

	head = tail;
	tail = current;
	return;
}
template<typename T>
void linked_list<T>::rotate_back() {
	node<T>* new_head = head->next;
	new_head->prior = nullptr;
	head->next = nullptr;
	tail->next = head;
	tail = head;
	head = new_head;
}

//int main() {
//	linked_list list;
//
//	int a = 1;
//	int b = 2;
//	int c = 3;
//	int d = 4;
//
//	list.push(&a);
//	list.push(&b);
//	list.push(&c);
//	list.push(&d);
//
//	list.rotate_back();
//
//	node* current = list.head;
//	while (current != nullptr) {
//		int x = *(int*)current->address;
//		printf("number : ");
//		printf("%d", x);
//		printf("\n");
//		current = current->next;
//	}
//
//	return 0;
//}

#endif