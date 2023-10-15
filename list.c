#include <stdio.h>
#include "list.h"
#include <assert.h>

// holds the data for the pListHeadsPool
static List listHeadsPool;
// holds the data for the pListNodesPool
static List listNodesPool;
// Holds a doubly-linked-list of List datatypes (treated as a stack)
static List * pListHeadsPool;
// Holds a doubly-linked-list of Node datatypes (treated as a stack)
static List * pListNodesPool;
// The pool of heads
static List headsPool[LIST_MAX_NUM_HEADS]; 
// The pool of nodes
static Node nodesPool[LIST_MAX_NUM_NODES + LIST_MAX_NUM_HEADS]; // Need extra 10 nodes for list holding list
static bool headsCreated = false;
static bool nodesCreated = false;

    // HELPER FUNCTIONS

// Pops an empty node off of the stack of nodes and returns it
// If all nodes are exhausted from pool, return NULL
Node * create_Node() {
    if (pListNodesPool->currentPtrStatus == LIST_OOB_START 
        && pListNodesPool->size == 0) { 
        return NULL;
    } 
    if (pListHeadsPool->pCurrentItemNode == NULL) {
        printf("An error occured in create_Node: currentItemNode was NULL!\n"); 
    }
    Node * newNode = pListNodesPool->pCurrentItemNode;
    pListNodesPool->pLast = pListNodesPool->pCurrentItemNode->pPrevious;
    pListNodesPool->pCurrentItemNode = pListNodesPool->pCurrentItemNode->pPrevious;
    pListNodesPool->pCurrentItemNode->pNext = NULL;

    newNode->pNext = NULL;
    newNode->pPrevious = NULL;
    pListNodesPool->size--;
    if (pListNodesPool->size == 0) { // List is empty -> no more nodes
        pListNodesPool->currentPtrStatus = LIST_OOB_START;
    }

    return newNode;    
}

// Theoretically, this function should never have to check if the list is "full"
// since no extra nodes are allocated except those that are generated once at startup.
// - Pushes an empty node on top of the nodes stack to be used again
// Returns -1 if failed, 0 if successful
// DOES NOT DEALLOCATE ANY MEMORY OF THE ITEMS
int push_Node(Node * pNode) {
    pNode->pPrevious = NULL;
    pNode->pNext = NULL;

    if (pListNodesPool->currentPtrStatus == LIST_OOB_START && pListNodesPool->size == 0) {
        pListNodesPool->pHead = pNode;
        // pListNodesPool->currentPtrStatus = NULL;
    } else {
        assert(pListNodesPool->pCurrentItemNode->pNext == NULL);
        // Make last and current node the newest node added
        pListNodesPool->pCurrentItemNode->pNext = pNode;
        pNode->pPrevious = pListNodesPool->pCurrentItemNode;
        pListNodesPool->pCurrentItemNode = pListNodesPool->pCurrentItemNode->pNext;
        pListNodesPool->pLast = pListNodesPool->pCurrentItemNode;
    }
    pListNodesPool->size++;
    return 0;
}

// Pushes a list onto the stack of lists and sets current ptr to it
// Returns -1 if failed, 0 if successful
int push_Head(List * pList) { 
    pList->pHead = NULL;
    pList->pLast = NULL;
    pList->pCurrentItemNode = NULL;
    pList->currentPtrStatus = LIST_OOB_START;
    pList->size = 0;
    if (pListHeadsPool->currentPtrStatus == LIST_OOB_START && pListHeadsPool->size == 0) {
        pListHeadsPool->pHead->pItem = pList;
        pListHeadsPool->pCurrentItemNode = pListHeadsPool->pHead;
    } else { // General case:
        pListHeadsPool->pCurrentItemNode->pNext->pItem = pList;
        pListHeadsPool->pCurrentItemNode = pListHeadsPool->pCurrentItemNode->pNext;
    }
    pListHeadsPool->size++;
    return 0;
}

List* initializeHeadsPool(List * pList, int size) {
    listHeadsPool.size = 0;

    for (unsigned int i = 0; i < size; i++) {
        // Take a node (from index 0 - 9) from the nodes pool and insert a head into there
        nodesPool[i].pItem = &headsPool[i]; 
        if (listHeadsPool.pCurrentItemNode == NULL) {
            // If no currentItem, then list is empty, so head points to first node
            listHeadsPool.pHead = &nodesPool[i];
        } else {
            nodesPool[i].pPrevious = listHeadsPool.pCurrentItemNode; // new.previous -> current
            listHeadsPool.pCurrentItemNode->pNext = &nodesPool[i]; // current.next -> new
        }
        
        // Update the current pointer to the new node
        listHeadsPool.pCurrentItemNode = &nodesPool[i]; 
        // Updated the last pointer to the new node
        listHeadsPool.pLast = &nodesPool[i];
        listHeadsPool.size++;
        // listHeadsPool.currentPtrStatus = NULL; // It is within the normal bounds
    }
    return &listHeadsPool;
}

List* initializeNodesPool(List * pList, int size) {
    listNodesPool.size = 0;

    unsigned int start = LIST_MAX_NUM_HEADS; // Index 10 to start pulling from 
    for (unsigned int i = start; i < start + size; i++) {
        if (listNodesPool.pCurrentItemNode == NULL) {
            // If no currentItem, then list is empty, so head points to first node
            listNodesPool.pHead = &nodesPool[i];
        } else {
            nodesPool[i].pPrevious = listNodesPool.pCurrentItemNode; // new.previous -> current
            listNodesPool.pCurrentItemNode->pNext = &nodesPool[i]; // current.next -> new
        }
        
        // Update the current pointer to the new node
        listNodesPool.pCurrentItemNode = &nodesPool[i]; 
        // Updated the last pointer to the new node
        listNodesPool.pLast = &nodesPool[i];
        listNodesPool.size++;
        // listNodesPool.currentPtrStatus = NULL; // It is within the normal bounds
    }
    return &listNodesPool;
}

    // Implementations of functions from List.h
    
void initPools() {
    if (headsCreated == false) {
        pListHeadsPool = initializeHeadsPool(pListHeadsPool, LIST_MAX_NUM_HEADS);
        assert(pListHeadsPool != NULL);
        headsCreated = true;
    }
    if (nodesCreated == false) {
        pListNodesPool = initializeNodesPool(pListNodesPool, LIST_MAX_NUM_NODES);
        assert(pListNodesPool != NULL);
        nodesCreated = true;
    }
}

List* List_create() {
    initPools();
    
    if (pListHeadsPool != NULL 
        && pListHeadsPool->pCurrentItemNode != NULL) {
        List * newList = (List*)pListHeadsPool->pCurrentItemNode->pItem; 
        assert(newList != NULL);
        
        listHeadsPool.size--;
        if (pListHeadsPool->pCurrentItemNode != pListHeadsPool->pHead) {
            // Move one space back and set pItem to NULL to free up a space
            pListHeadsPool->pCurrentItemNode->pItem = NULL;
            pListHeadsPool->pCurrentItemNode = pListHeadsPool->pCurrentItemNode->pPrevious;
        } else {
            // List is all used, cannot take any more
            pListHeadsPool->currentPtrStatus = LIST_OOB_START;
            pListHeadsPool->pCurrentItemNode = NULL;
        }

        // Initialize new list
        newList->pCurrentItemNode = NULL;
        newList->pHead = NULL;
        newList->size = 0;
        newList->pLast = NULL;
        newList->currentPtrStatus = LIST_OOB_START;
        return newList;
    }
    if (listHeadsPool.currentPtrStatus == LIST_OOB_END) {
        printf("Error in list_create: LIST_OOB_END in heads pool!\n");
    }
    // if error occured return NULL
    return NULL;
}

int List_count(List* pList) {
    assert(pList != NULL);
    return pList->size;
}

// Returns a pointer to the first item in pList and makes the first item the current item.
// Returns NULL and sets current item to NULL if list is empty.
void* List_first(List* pList) {
    pList->pCurrentItemNode = (pList->pHead != NULL 
        && pList->size > 0) ? pList->pHead : NULL;
    return pList->pCurrentItemNode->pItem;
}

void* List_last(List* pList) {
    pList->pCurrentItemNode = (pList->pLast != NULL 
        && pList->size != 0) ? pList->pLast : NULL;
    return pList->pCurrentItemNode->pItem;
}

void* List_next(List* pList) {
    if (pList->currentPtrStatus == LIST_OOB_END && pList->pCurrentItemNode == NULL) {
        return NULL;
    }

    if (pList->pCurrentItemNode == pList->pLast) { // Already at beyond end
        pList->pCurrentItemNode = NULL;
        pList->currentPtrStatus = LIST_OOB_END;
        return pList->pCurrentItemNode;
    } else if (pList->currentPtrStatus == LIST_OOB_START && pList->pCurrentItemNode == NULL) { // Before start of list
        if (pList->size > 0) {
            pList->pCurrentItemNode = pList->pHead;
        } else {
            pList->currentPtrStatus = LIST_OOB_END;
        }
    } else if (pList->pCurrentItemNode->pNext == NULL) { // Reached beyond end now
        pList->currentPtrStatus = LIST_OOB_END;
    }  else {
        pList->pCurrentItemNode = pList->pCurrentItemNode->pNext;
    }
    return pList->pCurrentItemNode->pItem;
}

void* List_prev(List* pList) {
    if (pList->currentPtrStatus == LIST_OOB_START && pList->pCurrentItemNode == NULL) {
        return NULL;
    }

    if (pList->pCurrentItemNode == pList->pHead) { // Already at beginning of list
        pList->pCurrentItemNode = NULL;
        pList->currentPtrStatus = LIST_OOB_START;
        return pList->pCurrentItemNode;
    } else if (pList->currentPtrStatus == LIST_OOB_END && pList->pCurrentItemNode == NULL) {
        if (pList->size > 0) {
            pList->pCurrentItemNode = pList->pLast;
        } else {
            pList->currentPtrStatus = LIST_OOB_START;
        }
    } else if (pList->pCurrentItemNode->pPrevious == NULL) {
        pList->currentPtrStatus = LIST_OOB_START;
    }  else {
        pList->pCurrentItemNode = pList->pCurrentItemNode->pPrevious;
    }
    return pList->pCurrentItemNode->pItem;
}

// Current may be NULL!
void* List_curr(List* pList) {
    return pList->pCurrentItemNode->pItem;
}

int List_insert_after(List* pList, void* pItem) {
    Node * newNode = create_Node();
    if (newNode == NULL) {
        return -1;
    }
    newNode->pItem = pItem;

    if (pList->currentPtrStatus == LIST_OOB_START && pList->pCurrentItemNode == NULL) {
        if (pList->size > 0) {
            newNode->pNext = pList->pHead; 
            pList->pHead->pPrevious = newNode;
        } else {
            pList->pLast = newNode;
        }
        pList->pHead = newNode;
        pList->pCurrentItemNode = pList->pHead;
    } 
    else if (pList->currentPtrStatus == LIST_OOB_END && pList->pCurrentItemNode == NULL) {
        if (pList->size > 0) {
            newNode->pPrevious = pList->pLast;
            pList->pLast->pNext = newNode;
        } else {
            pList->pHead = newNode;
        }
        pList->pLast = newNode;
        pList->pCurrentItemNode = pList->pLast;
    } 
    else {   //General case:
    newNode->pNext = pList->pCurrentItemNode->pNext;
    newNode->pPrevious = pList->pCurrentItemNode;
    if (pList->pCurrentItemNode != pList->pLast) {
        pList->pCurrentItemNode->pNext->pPrevious = newNode;
    } else {
        pList->pLast = newNode;
    }
    pList->pCurrentItemNode->pNext = newNode;
    pList->pCurrentItemNode = newNode;
    }

    pList->size++;
    return 0;
}

int List_insert_before(List* pList, void* pItem) {
    Node * newNode = create_Node();
    if (newNode == NULL) {
        return -1;
    }
    newNode->pItem = pItem;

    if (pList->currentPtrStatus == LIST_OOB_START && pList->pCurrentItemNode == NULL) {
        if (pList->size > 0) {
            newNode->pNext = pList->pHead;
        } else {
            pList->pLast = newNode;
        }
        pList->pHead = newNode;
        pList->pCurrentItemNode = pList->pHead;
    } 
    else if (pList->currentPtrStatus == LIST_OOB_END && pList->pCurrentItemNode == NULL) {
        if (pList->size > 0) {
            newNode->pPrevious = pList->pLast;
        } else {
            pList->pHead = newNode;
        }
        pList->pLast = newNode;
        pList->pCurrentItemNode = pList->pLast;
    } 
    else {
        newNode->pPrevious = pList->pCurrentItemNode->pPrevious;
        newNode->pNext = pList->pCurrentItemNode;
        if (pList->pCurrentItemNode != pList->pHead) {
            pList->pCurrentItemNode->pPrevious->pNext = newNode;
        } else {
            pList->pHead = newNode;
        }
        pList->pCurrentItemNode->pPrevious = newNode;
        pList->pCurrentItemNode = newNode;
    }
    pList->size++;
    return 0;
}

int List_append(List* pList, void* pItem) {
    Node * newNode = create_Node();
    if (newNode == NULL) {
        return -1;
    }
    newNode->pItem = pItem;

    if (pList->size > 0) {
        newNode->pPrevious = pList->pLast;
        pList->pLast->pNext = newNode;
    } else {
        pList->pHead = newNode;
    }
    pList->pLast = newNode;
    pList->pCurrentItemNode = pList->pLast;
    pList->size++;

    return 0;
}

int List_prepend(List* pList, void* pItem) {
    Node * newNode = create_Node();
    if (newNode == NULL) {
        return -1;
    }
    newNode->pItem = pItem;

    if (pList->size > 0) {
        newNode->pNext = pList->pHead;
        pList->pHead->pPrevious = newNode;
    } else {
        pList->pLast = newNode;
    }
    pList->pHead = newNode;
    pList->pCurrentItemNode = pList->pHead;
    pList->size++;

    return 0;
}

void* List_remove(List* pList) {
    if ( (pList->currentPtrStatus == LIST_OOB_START ||
        pList->currentPtrStatus == LIST_OOB_END) && 
        pList->pCurrentItemNode == NULL) {
        return NULL;
    } 
    Node * nodeToRemove = pList->pCurrentItemNode;
    void * itemToRemove = pList->pCurrentItemNode->pItem;
    // make next item current one
    if (nodeToRemove->pNext == NULL) {
        pList->currentPtrStatus = LIST_OOB_END;
        pList->pCurrentItemNode = NULL;
    } else {
        pList->pCurrentItemNode = pList->pCurrentItemNode->pNext;
    }

    // take current item out of plist 
    if (nodeToRemove == pList->pHead && pList->pCurrentItemNode != NULL) { // head removed
        pList->pCurrentItemNode->pPrevious = NULL;
        pList->pHead = pList->pCurrentItemNode;
        nodeToRemove->pNext = NULL;
    } else if (nodeToRemove == pList->pLast && nodeToRemove->pPrevious != NULL) { // last removed 
        pList->pLast = pList->pLast->pPrevious;
        pList->pLast->pNext = NULL;
        pList->currentPtrStatus = LIST_OOB_END;
        nodeToRemove->pPrevious = NULL;
    } else if (pList->pCurrentItemNode == NULL && pList->size == 1) { // only 1 left
        pList->pHead = NULL;
        pList->pLast = NULL;
        pList->currentPtrStatus = LIST_OOB_END;
    } else {
        nodeToRemove->pNext->pPrevious = nodeToRemove->pPrevious;
        nodeToRemove->pPrevious->pNext = nodeToRemove->pNext;
    }

    // return its node to pool
    push_Node(nodeToRemove);
    pList->size--;
    return itemToRemove;
}

void* List_trim(List* pList) {
    if (pList->pHead == NULL) { // case: size == 0
        return NULL;
    }
    Node * nodeToRemove = pList->pLast;
    void * itemToRemove = pList->pLast->pItem;
    
    if (pList->size == 1) { // case size == 1
        pList->currentPtrStatus = LIST_OOB_START;
        pList->pCurrentItemNode = NULL;
        pList->pLast = NULL;
        pList->pHead = NULL;
    } else {
        pList->pLast = pList->pLast->pPrevious;
        pList->pLast->pNext = NULL;
    }
    
    nodeToRemove->pPrevious = NULL;
    push_Node(nodeToRemove);
    pList->size--;

    return itemToRemove;
}

void List_concat(List* pList1, List* pList2) {
    // Concatenate them
    if (pList2->pHead != NULL) {

        if (pList1->pHead == NULL) { // P1 is empty
            pList1->pHead = pList2->pHead;
        } else{
            pList1->pLast->pNext = pList2->pHead;
            if (pList2->pHead != NULL) {
                pList2->pHead->pPrevious = pList1->pLast;
            }
        }
        pList1->pLast = pList2->pLast;
        pList1->size += pList2->size;
    }
    
    // Clear out plist2
    push_Head(pList2);
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    if (pList->pHead == NULL) {
        push_Head(pList);
        return;
    }
    
    void * itemToFree = NULL;
    Node * nodeToFree = NULL;
    pList->pCurrentItemNode = pList->pLast;

    while (pList->size > 0 || pList->pCurrentItemNode != NULL) {

        // Free the allocated memory to the item in the node
        itemToFree = pList->pCurrentItemNode->pItem;
        if (itemToFree != NULL) {
            (*pItemFreeFn)(itemToFree);
        }
        nodeToFree = pList->pCurrentItemNode;

        pList->size--;
        
        pList->pCurrentItemNode = pList->pCurrentItemNode->pPrevious;
        pList->pLast = pList->pCurrentItemNode;
        
        push_Node(nodeToFree);
    }
    push_Head(pList);
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    if (pList->pHead == NULL || (pList->currentPtrStatus == LIST_OOB_END 
        && pList->pCurrentItemNode == NULL) ) {
        pList->pCurrentItemNode = NULL;
        pList->currentPtrStatus = LIST_OOB_END;
        return NULL;
    }
    if (pList->currentPtrStatus == LIST_OOB_START && pList->pCurrentItemNode == NULL) {
        pList->pCurrentItemNode = pList->pHead;
    }    

    int itemFound = 0;
    while (pList->pCurrentItemNode != NULL) {
        itemFound = pComparator(pList->pCurrentItemNode->pItem, pComparisonArg);
        if (itemFound) {

            return pList->pCurrentItemNode->pItem;
        } else {
            pList->pCurrentItemNode = pList->pCurrentItemNode->pNext;
        }
    }
    // Item was not found
    pList->pCurrentItemNode = NULL;
    pList->currentPtrStatus = LIST_OOB_END;
    return NULL;
}


