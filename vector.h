#pragma once

#define MIN_VEC_CAP (16)

typedef unsigned char byte_t;
typedef void (*deleteFunc_t)(void*);

// example of func declaration in var.h :
// void instancePoly_wrapper(VAR_STORED_TYPE* _pElem, VAR_STORED_TYPE* _pInstance) {}
typedef void (*instanceFunc_t)(void*, void*);

// compares 2 elems supplied by pointers
typedef void* (*compareFunc_t)(void*, void*);

// sorted elems are stored in the begin of the vector.
// sortFunc checks wether the 1st arg elem is aspirant
// to be moved into sorted part of the vector.
// if the elem is a such one, sortFunc retuns 1.
typedef int (*sortFunc_t)(void*, void*);



#define VEC_STORED_TYPE byte_t
// if u change VEC_STORED_TYPE on sth that
// sizeof(sth) differs from 1, u get crash

typedef struct _vector_t {
    VEC_STORED_TYPE*    data;
    int                 cap;
    int                 cnt;
    int                 elem_sz;
    deleteFunc_t        deleteFunc;
    instanceFunc_t      instanceFunc;
} vector_t;

vector_t* newVec(
                    int elem_sz, 
                    int cnt, 
                    deleteFunc_t deleteFunc, 
                    instanceFunc_t instanceFunc
                );
void initVec    (
                    vector_t* vec, 
                    int elem_sz, 
                    int cnt, 
                    deleteFunc_t deleteFunc, 
                    instanceFunc_t instanceFunc
                );
void deleteVec(vector_t** _vec);
void deleteVec_static(vector_t* vec);
void expandVec(vector_t* vec, void* pValue);
void* getElemPtrVec(vector_t* vec, int idx);
void printVec(vector_t* vec, char* specificator);

vector_t* instanceVec(vector_t* vec);
void instanceVec_static(vector_t* dst_vec, vector_t* src_vec);
void moveVec(vector_t* dst_vec, vector_t* src_vec);

void* findElemVec(vector_t* vec, compareFunc_t cmpFunc, void* pRefElem);
void replaceElemVec(vector_t* vec, void* pElem, void* pNewElem);

void deleteElemVec(vector_t* vec, void* pDeletedElem);

void sortVec(vector_t* vec, sortFunc_t sortFunc);
void swapVec(vector_t* vec, int idx1, int idx2);
