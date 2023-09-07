#ifndef __LIST_H__
#define __LIST_H__

#define offset_in_parent(parent_type, node_name) \
    ((uint32_t) & (((parent_type*)0)->node_name))
#define offset_to_parent(node, parent_type, node_name) \
    ((uint32_t)node - offset_in_parent(parent_type, node_name))
#define list_node_parent(node, parent_type, node_name) \
    ((parent_type*)(node ? offset_to_parent((node), parent_type, node_name) : 0))

/**
 * @brief 链表节点类型
 */
typedef struct _list_node_t {
    struct _list_node_t* pre;
    struct _list_node_t* next;
} list_node_t;

/**
 * @brief 头节点初始化
 */
static inline void list_node_init(list_node_t* node)
{
    node->next = node->pre = (list_node_t*)0;
}

/**
 * @brief 获取后继节点
 */
static inline list_node_t* list_node_next(list_node_t* node)
{
    return node->next;
}

/**
 * @brief 获取前驱节点
 */
static inline list_node_t* list_node_pre(list_node_t* node)
{
    return node->pre;
}

/**
 * @brief 链表结构
 */

typedef struct _list_t {
    list_node_t* first;
    list_node_t* last;
    int count;
} list_t;

void list_init(list_t* list);

/**
 * @brief 判空
 */
static inline int list_is_empty(list_t* list)
{
    return list->count == 0;
}

static inline int list_count(list_t* list)
{
    return list->count;
}

/**
 * @brief 获取链表头节点
 */
static inline list_node_t* list_first(list_t* list)
{
    return list->first;
}

/**
 * @brief 获取链表尾节点
 */
static inline list_node_t* list_last(list_t* list)
{
    return list->last;
}
void list_insert_first(list_t* list, list_node_t* node);
void list_insert_last(list_t* list, list_node_t* node);
list_node_t* list_remove_first(list_t* list);
list_node_t* list_remove(list_t* list, list_node_t* node);
#endif