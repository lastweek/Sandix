#ifndef _SANDIX_LIST_H_
#define _SANDIX_LIST_H_

#include <sandix/types.h>
#include <sandix/kernel.h>
#include <sandix/const.h>

/*
 *	Simple doubly linked list implementation.
 *
 *	Some of the internal functions ("__xxx") are useful when
 *	manipulating whole lists rather than single entries, as
 *	sometimes we already know the next/prev entries and we can
 *	generate better code by using them directly rather than
 *	using the generic single-entry routines.
 */

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void
INIT_LIST_HEAD(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void
__list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
	prev->next = new;
	next->prev = new;
	new->next = next;
	new->prev = prev;
}

static inline void
__list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

/**
 *	list_add - add a new entry after head
 *	@new: new entry to be added
 *	@head: list head to add it after
 */
static inline void
list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 *	list_add_tail - add a new entry to tail
 *	@new: new entry to be added
 *	@head: list head to add it before
 */
static inline void
list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head-prev, head);
}

/**
 *	list_del - delete a entry from list
 *	@entry: entry to be deleted
 */
static inline void
list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 *	list_del_init - deletes entry from list and reinitialized it
 *	@entry: the element to delete from the list
 */
static inline void
list_del_init(struct list_head *entry)
{
	__list_del(entry_prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 *	list_replace - replace old entry by new one
 *	@old: the element to be replaced
 *	@new: the new element to insert
 */
static inline void
list_replace(struct list_head *old, struct list_head *new)
{
	old->prev->next = new;
	old->next->prev = new;
	new->prev = old->prev;
	new->next = old->next;
}

/**
 *	list_replace_init - replace old by new and reinitialized old
 *	@old: the element to be replaced
 *	@new: the new element to insert
 */
static inline void
list_replace_init(struct list_head *old, struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

/**
 * list_is_last - test whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int
list_is_last(const struct list_head *list, const struct list_head *head)
{
	return list->next == head;
}

/**
 *	list_empty - test whether a list is empty
 *	@head: the list to test
 */
static inline int
list_empty(const struct list_head *head)
{
	return head->next == head;
}


/**
 *	list_entry - get the struct for this entry
 *	@ptr: the &(struct list_head) pointer.
 *	@type: the type of the struct this is embedded in.
 *	@member: the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
		container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)


#endif /* _SANDIX_LIST_H */
