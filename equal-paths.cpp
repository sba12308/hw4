#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm> // for max function
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

//get height of path to leaf
int getLeafHeight(Node* root) {
    //base case: if node is null, return 0
    if (root == nullptr) return 0;
    
    //base case: if node is a leaf, return 1
    if (root->left == nullptr && root->right == nullptr) return 1;
    
    //if left subtree exists, recurse left
    int leftHeight = (root->left) ? getLeafHeight(root->left) : 0;
    //if right subtree exists, recurse right
    int rightHeight = (root->right) ? getLeafHeight(root->right) : 0;
    
    //if we have both subtrees, both must be non-zero to count
    if (root->left && root->right)
    {
        return (leftHeight && rightHeight) ? max(leftHeight, rightHeight) + 1 : 0;
    }
    //if we have only one subtree, return height plus one
    return (leftHeight + rightHeight) + 1;
}

//check if all leaf paths have same height
bool checkEqualPaths(Node* root, int targetHeight, int currentHeight) {
    //base case: if node is null, return true (empty paths are equal)
    if (root == nullptr) return true;
    
    //if reach a leaf, check if path length equals target height
    if (root->left == nullptr && root->right == nullptr)
    {
        return currentHeight == targetHeight;
    }
    
    //recurse on both subtrees, incrementing current height
    return (root->left ? checkEqualPaths(root->left, targetHeight, currentHeight + 1) : true) &&
           (root->right ? checkEqualPaths(root->right, targetHeight, currentHeight + 1) : true);
}

bool equalPaths(Node* root)
{
    // Add your code below

    //special case: empty tree has equal paths
    if (root == nullptr) return true;
    
    //get height of first leaf path
    int targetHeight = getLeafHeight(root);
    
    //check if all leaf paths have same height
    return checkEqualPaths(root, targetHeight, 1);
}

