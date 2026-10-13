#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int recountCapVec(int cap, int cnt)
{
    if (cap == 0) return MIN_VEC_CAP;

    int required_cap = cnt;

    if (cap >= required_cap) {
        return cap;
    }

    int new_cap;
    for (
        new_cap = cap; 
        new_cap < required_cap; 
        new_cap = new_cap << 1) {}
    
    return new_cap;
}

void initVec(vector_t* vec, int elem_sz, int cnt, deleteFunc_t deleteFunc, instanceFunc_t instanceFunc)
{
    if (elem_sz < 1) elem_sz = 1;
    vec->elem_sz = elem_sz;

    if (cnt < 0) cnt = 0;
    vec->cnt = cnt;

    if (vec->cnt < MIN_VEC_CAP)
        vec->cap = MIN_VEC_CAP;
    else
        vec->cap = recountCapVec(MIN_VEC_CAP, vec->cnt);

    vec->data = calloc(vec->cap, vec->elem_sz);
    vec->deleteFunc = deleteFunc;
    vec->instanceFunc = instanceFunc;
}

void deleteVec_static(vector_t* vec)
{
    if (vec == NULL) return;

    if (vec->deleteFunc) {
        int i;
        for (i = 0; i < vec->cnt; i++)
            vec->deleteFunc(getElemPtrVec(vec, i));
    }
}

vector_t* newVec(
    int elem_sz, 
    int cnt, 
    deleteFunc_t deleteFunc, 
    instanceFunc_t instanceFunc
)
{
    vector_t* vec = calloc(1, sizeof(vector_t));

    initVec(
            vec,
            elem_sz,
            cnt,
            deleteFunc,
            instanceFunc
            );

    return vec;
}

void deleteVec(vector_t** _vec)
{
    vector_t* vec = *_vec;
    deleteVec_static(vec);
    free(vec);
    *_vec = NULL;
}

void* getElemPtrVec(vector_t* vec, int idx)
{
    if (vec == NULL || idx < 0 || idx >= vec->cnt)
        return NULL;

    return &vec->data[idx*(vec->elem_sz)];
}

void expandVec(vector_t* vec, void* pvalue)
{
    int new_cap = recountCapVec(vec->cap, vec->cnt + 1);
    // printf("new_cap %d\n", new_cap);

    if (new_cap > vec->cap) {
        VEC_STORED_TYPE* data = calloc(new_cap, vec->elem_sz);
        memmove(data, vec->data, vec->elem_sz*vec->cnt);
        free(vec->data);
        vec->data = data;
        vec->cap = new_cap;
    }

    memmove (
            &vec->data[vec->cnt*vec->elem_sz],
            pvalue, 
            vec->elem_sz
            );

    vec->cnt++;
}


void printVec(vector_t* vec, char* specificator)
{
    int specLen = strlen(specificator);
    char* specStr = calloc(1, specLen+1);
    snprintf(specStr, specLen+1, "%s", specificator);

    VEC_STORED_TYPE* pTmpElem = calloc(1, vec->elem_sz);
    VEC_STORED_TYPE* pElem;

    int i;
    for (i = 0; i < vec->cnt; i++){
        pElem = getElemPtrVec(vec, i);
        memmove(pTmpElem, pElem, vec->elem_sz);
        printf(specStr, *(int*)pTmpElem);
    }

    free(specStr);
    free(pTmpElem);
    
}

vector_t* instanceVec(vector_t* vec)
{
    vector_t* instance = newVec (
                                vec->elem_sz, 
                                vec->cnt, 
                                vec->deleteFunc,
                                vec->instanceFunc
                                );
    
    if (vec->instanceFunc) {

        int i;
        int elem_sz = vec->elem_sz;
        int cnt = vec->cnt;
        instanceFunc_t instanceFunc = vec->instanceFunc;

        VEC_STORED_TYPE* pElemInstace = calloc(1, elem_sz);

        for (i = 0; i < cnt; i++) {
            instanceFunc(getElemPtrVec(vec, i), pElemInstace);
            memmove(getElemPtrVec(instance, i), pElemInstace, elem_sz);
        }

        free(pElemInstace);
    }
    else {
        memmove(instance->data, vec->data, vec->cnt * vec->elem_sz);
    }
    
    return instance;
}

void instanceVec_static(vector_t* dst_vec, vector_t* src_vec)
{
    initVec (
            dst_vec,
            src_vec->elem_sz, 
            src_vec->cnt, 
            src_vec->deleteFunc,
            src_vec->instanceFunc
            );
    
    if (src_vec->instanceFunc) {

        int i;
        int elem_sz = src_vec->elem_sz;
        int cnt = src_vec->cnt;
        instanceFunc_t instanceFunc = src_vec->instanceFunc;

        VEC_STORED_TYPE* pElemInstace = calloc(1, elem_sz);

        for (i = 0; i < cnt; i++) {
            instanceFunc(getElemPtrVec(src_vec, i), pElemInstace);
            memmove(getElemPtrVec(dst_vec, i), pElemInstace, elem_sz);
        }

        free(pElemInstace);
    }
    else {
        memmove(dst_vec->data, src_vec->data, src_vec->cnt * src_vec->elem_sz);
    }
}

// both vectors are still available on dst_vec and src_vec ptrs
void moveVec(vector_t* dst_vec, vector_t* src_vec)
{
    deleteVec_static(dst_vec);
    memmove(dst_vec, src_vec, sizeof(vector_t));
    initVec(
            src_vec,
            src_vec->elem_sz, 
            0, 
            src_vec->deleteFunc,
            src_vec->instanceFunc
            );
}

// returns ptr on the found elem.
// returns NULL if none is found.
// pRefElem is a ptr introduced to cmpFunc as 2nd arg.
// cmpFunc gets 2 ptrs:
//      ptr1 - ptr on a value from vector
//      ptr2 - pRefElem
void* findElemVec(vector_t* vec, compareFunc_t cmpFunc, void* pRefElem)
{
    void* found;
    int i;
    for (i = 0; i < vec->cnt; i++) {
        found = cmpFunc(getElemPtrVec(vec, i), pRefElem);
        if (found) return found;
    }

    return NULL;
}

// pElem - addr of replaced elem
// pNewElem - addr of new elem
void replaceElemVec(vector_t* vec, void* pElem, void* pNewElem)
{
    // if you replaces monom in poly
    // deleteFunc does:
    //      1. frees memory pointed by *pElem
    //      2. writes NULL on addr pElem
    vec->deleteFunc(pElem);
    // printf("replaceElemVec: %p\n", *(void**)pElem);
    memmove(pElem, pNewElem, vec->elem_sz);
}

// pDeletedElem - ptr on value representing elem to delete
void deleteElemVec(vector_t* vec, void* pDeletedElem)
{
    if (vec->cnt == 0) return;

    void* pElem = NULL;
    
    int idx;
    for (idx = 0; idx < vec->cnt; idx++) {
        pElem = getElemPtrVec(vec, idx);
        if (!memcmp(pElem, pDeletedElem, vec->elem_sz))
            break;
    }

    if (vec->deleteFunc) vec->deleteFunc(pElem);

    int k;
    for (k = idx + 1; k < vec->cnt; k++)
        memmove(getElemPtrVec(vec, k-1), getElemPtrVec(vec, k), vec->elem_sz);
    
    memset(getElemPtrVec(vec, vec->cnt-1), 0, vec->elem_sz);  
    vec->cnt--; 
}


void swapVec(vector_t* vec, int idx1, int idx2)
{
    VEC_STORED_TYPE* pTmpElem = calloc(1, vec->elem_sz);
    VEC_STORED_TYPE* pElem1 = getElemPtrVec(vec, idx1);
    VEC_STORED_TYPE* pElem2 = getElemPtrVec(vec, idx2);
    int elem_sz = vec->elem_sz;
    memmove(pTmpElem, pElem1, elem_sz);
    memmove(pElem1, pElem2, elem_sz);
    memmove(pElem2, pTmpElem, elem_sz);
    free(pTmpElem);
}


void sortVec(vector_t* vec, sortFunc_t sortFunc)
{
    if (!vec || !sortFunc) return;

    int good_idx;
    int cnt = vec->cnt;
    VEC_STORED_TYPE *pElem_good, *pElem_k;
    int i, k;
    for (i = 0; i < cnt; i++) {
        good_idx = i;
        for (k = i + 1; k < cnt; k++) {
            pElem_good = getElemPtrVec(vec, good_idx);
            pElem_k = getElemPtrVec(vec, k);
            if (sortFunc(pElem_k, pElem_good)) {
                good_idx = k;
            }
        }
        if (good_idx != i) {
            swapVec(vec, good_idx, i);
        }
    }   
}
