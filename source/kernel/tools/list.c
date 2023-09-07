#include "tools/list.h"

/**
 * @brief 链表初始化
 */
void list_init(list_t* list)
{
    list->first = list->last = (list_node_t*)0;
    list->count = 0;
}

/**
 * @brief 头插法
 */
void list_insert_first(list_t* list, list_node_t* node)
{
    node->next = list->first;
    node->pre = (list_node_t*)0;
    if (list_is_empty(list)) {
        list->last = list->first = node;
    } else {
        list->first->pre = node;
        list->first = node;
    }
    list->count++;
}
/**
 * @brief 尾插法
 */
void list_insert_last(list_t* list, list_node_t* node)
{
    node->next = (list_node_t*)0;
    node->pre = list->last;

    if (list_is_empty(list)) {
        list->first = list->last = node;
    } else {
        list->last->next = node;
        list->last = node;
    }
    list->count++;
}

/**
 * @brief 删除头部节点
 */
list_node_t* list_remove_first(list_t* list)
{
    if (list_is_empty(list))
        return (list_node_t*)0;

    list_node_t* remove_node = list->first;
    list->first = remove_node->next;
    if (list->first == (list_node_t*)0) {
        list->last = (list_node_t*)0;
    } else {
        remove_node->next->pre = (list_node_t*)0;
    }
    remove_node->next = remove_node->pre = (list_node_t*)0;
    list->count--;
    return remove_node;
}
/**
 * @brief 删除指定节点
 */
list_node_t* list_remove(list_t* list, list_node_t* remove_node)
{
    if (remove_node == list->first) {
        list->first = remove_node->next;
    }
    if (remove_node == list->last) {
        list->last = remove_node->pre;
    }
    if (remove_node->pre) {
        remove_node->pre->next = remove_node->next;
    }
    if (remove_node->next) {
        remove_node->next->pre = remove_node->pre;
    }
    remove_node->pre = remove_node->next = (list_node_t*)0;
    --list->count;
    return remove_node;
}
