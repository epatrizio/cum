#ifndef VECTOR_H
#define VECTOR_H

#define MIN_CAPACITY 10

#define resizable_array(T)          \
    struct T##_resizable_array {    \
        T *elts;                    \
        size_t capacity;            \
        size_t size;                \
    };

#define vector_struct(T)    \
    typedef struct T##_resizable_array *T##_vector;

#define vector_create_sign(T)                           \
    T##_vector T##_vector_create(size_t);               \
    void T##_vector_destroy(T##_vector);                \
    size_t T##_vector_size(T##_vector);                 \
    bool T##_vector_is_empty(T##_vector);               \
    void T##_vector_resize(T##_vector, size_t);         \
    T T##_vector_get(const T##_vector, size_t);         \
    void T##_vector_set(T##_vector, size_t, const T);   \
    void T##_vector_add(T##_vector, size_t, const T);   \
    T T##_vector_remove(T##_vector, size_t);

#define vector_init(T)      \
    resizable_array(T)      \
    vector_struct(T)        \
    vector_create_sign(T)

#define vector_create_fct(T)                                    \
    T##_vector T##_vector_create(size_t capacity) {             \
        size_t cap = (capacity > 0) ? capacity : MIN_CAPACITY;  \
        T##_vector v = malloc(sizeof(T##_vector));              \
        v->elts = malloc(cap * sizeof(T));                      \
        v->capacity = cap;                                      \
        v->size = 0;                                            \
        return v;                                               \
    }

#define vector_destroy_fct(T)               \
    void T##_vector_destroy(T##_vector v)   \
    {                                       \
        free(v->elts);                      \
        free(v);                            \
    }

#define vector_size_fct(T)                  \
    size_t T##_vector_size(T##_vector v)    \
    {                                       \
        return v->size;                     \
    }

#define vector_is_empty_fct(T)              \
    bool T##_vector_is_empty(T##_vector v)  \
    {                                       \
        return (T##_vector_size(v) == 0);   \
    }

#define vector_resize_fct(T)                                                \
    void T##_vector_resize(T##_vector v, size_t size)                       \
    {                                                                       \
        if (size > v->capacity) {                                           \
            v->capacity = (size > v->capacity*2) ? size : v->capacity*2;    \
            v->elts = realloc(v->elts, v->capacity * sizeof(T));            \
        }                                                                   \
        v->size = size;                                                     \
    }

#define vector_get_fct(T)                               \
    T T##_vector_get(const T##_vector v, size_t index)  \
    {                                                   \
        assert(index < v->size);                        \
        return v->elts[index];                          \
    }

#define vector_set_fct(T)                                           \
    void T##_vector_set(T##_vector v, size_t index, const T elt)    \
    {                                                               \
        assert(index < v->size);                                    \
        v->elts[index] = elt;                                       \
    }

#define vector_add_fct(T)                                           \
    void T##_vector_add(T##_vector v, size_t index, const T elt)    \
    {                                                               \
        assert(index <= v->size);                                   \
        T##_vector_resize(v, v->size+1);                            \
        if (index < v->size-1) {                                    \
            for (unsigned int i = v->size-2 ; i >= index ; i--) {   \
                T##_vector_set(v, i+1, T##_vector_get(v, i));       \
            }                                                       \
        }                                                           \
        v->elts[index] = elt;                                       \
    }

#define vector_remove_fct(T)                                        \
    T T##_vector_remove(T##_vector v, size_t index)                 \
    {                                                               \
        assert(index < v->size);                                    \
        T elt = T##_vector_get(v, index);                           \
        if (index < v->size-1) {                                    \
            for (unsigned int i = index ; i < v->size-1 ; i++) {    \
                T##_vector_set(v, i, T##_vector_get(v, i+1));       \
            }                                                       \
        }                                                           \
        T##_vector_resize(v, v->size-1);                            \
        return elt;                                                 \
    }

#define vector_init_fct(T)  \
    vector_create_fct(T)    \
    vector_destroy_fct(T)   \
    vector_is_empty_fct(T)  \
    vector_size_fct(T)      \
    vector_resize_fct(T)    \
    vector_get_fct(T)       \
    vector_set_fct(T)       \
    vector_add_fct(T)       \
    vector_remove_fct(T)

vector_init(int)
vector_init(short)
vector_init(long)
vector_init(float)
vector_init(double)
vector_init(char)

#endif
