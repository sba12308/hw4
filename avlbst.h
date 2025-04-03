#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item);
    virtual void remove(const Key& key);
protected:
    //add base class version
    using BinarySearchTree<Key, Value>::nodeSwap;
    //add specialized version
    virtual void nodeSwap(AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    //helper functions
    void rotateLeft(AVLNode<Key,Value>* node);
    void rotateRight(AVLNode<Key,Value>* node);
    void insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node);
    void removeFix(AVLNode<Key,Value>* node, int8_t diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    //if tree is empty
    if (this->empty())
    {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
        return;
    }
    
    AVLNode<Key,Value>* current = static_cast<AVLNode<Key,Value>*>(this->root_);
    AVLNode<Key,Value>* parent = nullptr;
    
    //find insertion point
    while (current != nullptr)
    {
        if (new_item.first == current->getKey())
        {
            current->setValue(new_item.second);
            return;
        }
        parent = current;
        if (new_item.first < current->getKey())
        {
            current = current->getLeft();
        }
        else
        {
            current = current->getRight();
        }
    }
    
    //create new node
    AVLNode<Key,Value>* newNode = new AVLNode<Key,Value>(
        new_item.first, new_item.second, parent);
    
    //link it to parent
    if (new_item.first < parent->getKey())
    {
        parent->setLeft(newNode);
        parent->updateBalance(-1);
    }
    else
    {
        parent->setRight(newNode);
        parent->updateBalance(1);
    }
    
    //fix AVL property if needed
    if (parent->getBalance() != 0)
    {
        insertFix(parent, newNode);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    AVLNode<Key,Value>* node = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
    if (!node) return;
    
    // Case 1: Node has two children
    if (node->getLeft() != nullptr && node->getRight() != nullptr) {
        AVLNode<Key,Value>* pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
    }
    
    // Now node has at most one child
    AVLNode<Key,Value>* parent = node->getParent();
    AVLNode<Key,Value>* child = node->getLeft() ? 
                                node->getLeft() : node->getRight();
    
    int8_t diff = 0;
    if (parent) {
        if (parent->getLeft() == node) diff = 1;
        else diff = -1;
    }
    
    // Update root if needed
    if (node == this->root_) {
        this->root_ = child;
    }
    else {
        if (parent->getLeft() == node) parent->setLeft(child);
        else parent->setRight(child);
    }
    
    // Update child's parent pointer
    if (child) child->setParent(parent);
    
    // Fix AVL property
    if (parent) removeFix(parent, diff);
    
    delete node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key,Value>* node)
{
    if (!node || !node->getRight()) return;
    
    AVLNode<Key,Value>* rightChild = node->getRight();
    AVLNode<Key,Value>* parent = node->getParent();
    
    //update parent's child pointer
    if (parent)
    {
        if (parent->getLeft() == node) parent->setLeft(rightChild);
        else parent->setRight(rightChild);
    }
    else
    {
        this->root_ = rightChild;
    }
    
    //perform rotation
    node->setRight(rightChild->getLeft());
    if (rightChild->getLeft()) rightChild->getLeft()->setParent(node);
    rightChild->setLeft(node);
    rightChild->setParent(parent);
    node->setParent(rightChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key,Value>* node)
{
    if (!node || !node->getLeft()) return;
    
    AVLNode<Key,Value>* leftChild = node->getLeft();
    AVLNode<Key,Value>* parent = node->getParent();
    
    //update parent's child pointer
    if (parent)
    {
        if (parent->getLeft() == node) parent->setLeft(leftChild);
        else parent->setRight(leftChild);
    }
    else {
        this->root_ = leftChild;
    }
    
    //perform rotation
    node->setLeft(leftChild->getRight());
    if (leftChild->getRight()) leftChild->getRight()->setParent(node);
    leftChild->setRight(node);
    leftChild->setParent(parent);
    node->setParent(leftChild);
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key,Value>* parent, AVLNode<Key,Value>* node)
{
    if (!parent || !node) return;
    
    AVLNode<Key,Value>* grandparent = parent->getParent();
    if (!grandparent) return;
    
    //if parent is left child
    if (grandparent->getLeft() == parent)
    {
        grandparent->updateBalance(-1);
        if (grandparent->getBalance() == 0) return;
        
        if (grandparent->getBalance() == -1)
        {
            insertFix(grandparent, parent);
            return;
        }
        
        if (grandparent->getBalance() == -2)
        {
            //zig-zig case
            if (parent->getLeft() == node)
            {
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            //zig-zag case
            else
            {
                rotateLeft(parent);
                rotateRight(grandparent);
                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    node->setBalance(0);
                }
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
                else
                {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
    //if parent is right child
    else
    {
        grandparent->updateBalance(1);
        if (grandparent->getBalance() == 0) return;
        
        if (grandparent->getBalance() == 1)
        {
            insertFix(grandparent, parent);
            return;
        }
        
        if (grandparent->getBalance() == 2)
        {
            // Zig-zig case
            if (parent->getRight() == node)
            {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            }
            //zig-zag case
            else
            {
                rotateRight(parent);
                rotateLeft(grandparent);
                if (node->getBalance() == 1)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    node->setBalance(0);
                }
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
                else
                {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key,Value>* node, int8_t diff)
{
    if (!node) return;
    
    AVLNode<Key,Value>* parent = node->getParent();
    int8_t ndiff = 0;
    if (parent && parent->getLeft() == node) ndiff = 1;
    else if (parent) ndiff = -1;
    
    //update balance
    node->updateBalance(diff);
    
    //case 1: balance is zero - tree is balanced
    if (node->getBalance() == 0)
    {
        removeFix(parent, ndiff);
        return;
    }
    //case 2: balance is +/- 1 - subtree height unchanged
    else if (node->getBalance() == 1 || node->getBalance() == -1)
    {
        return;
    }
    
    //case 3: balance is +/- 2 - rotation needed
    AVLNode<Key,Value>* child = nullptr;
    if (node->getBalance() == -2)
    {
        child = node->getLeft();
        //zig-zig case
        if (child->getBalance() <= 0)
        {
            rotateRight(node);
            node->setBalance(-child->getBalance());
            child->setBalance(0);
            removeFix(parent, ndiff);
        }
        //zig-zag case
        else
        {
            AVLNode<Key,Value>* grandchild = child->getRight();
            rotateLeft(child);
            rotateRight(node);
            if (grandchild->getBalance() == -1)
            {
                node->setBalance(1);
                child->setBalance(0);
            }
            else if (grandchild->getBalance() == 0)
            {
                node->setBalance(0);
                child->setBalance(0);
            }
            else
            {
                node->setBalance(0);
                child->setBalance(-1);
            }
            grandchild->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
    else if (node->getBalance() == 2)
    {
        child = node->getRight();
        //zig-zig case
        if (child->getBalance() >= 0)
        {
            rotateLeft(node);
            node->setBalance(-child->getBalance());
            child->setBalance(0);
            removeFix(parent, ndiff);
        }
        //zig-zag case
        else
        {
            AVLNode<Key,Value>* grandchild = child->getLeft();
            rotateRight(child);
            rotateLeft(node);
            if (grandchild->getBalance() == 1)
            {
                node->setBalance(-1);
                child->setBalance(0);
            }
            else if (grandchild->getBalance() == 0)
            {
                node->setBalance(0);
                child->setBalance(0);
            }
            else
            {
                node->setBalance(0);
                child->setBalance(1);
            }
            grandchild->setBalance(0);
            removeFix(parent, ndiff);
        }
    }
}

#endif
