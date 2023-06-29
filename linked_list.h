#pragma once
#include <stdlib.h>
#include <stdio.h>

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template<typename T>
struct node {
	T* address;
	node* prior;
	node* next;

	node() = default;
	node(T* address, node* prior = nullptr);

	void attach_next(node* next);
	void attach_prior(node* prior);

};

/*Linked list. Head is the first inserted element, tail is the last.
Elements are stored in nodes as void pointers. Removing elements from the
list does NOT deallocate the memory pointed to by each node.
*/
template<typename T>
struct linked_list {
	node<T>* head;
	node<T>* tail;
	int size;

	linked_list();

	void push(T* ptr);
	T* pop();

	/*set action to "t" to free memory pointed to by each node.*/
	void destroy(const char* action = NULL);
	void reverse();
	void rotate_back();
};

#include "linked_list.hpp"

#endif