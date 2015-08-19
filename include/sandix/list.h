/*
 *	include/sandix/list.h - Link List
 *
 *	Copyright (C) 2015 Yizhou Shan <shanyizhou@ict.ac.cn>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
 * list_add - add a new entry after head
 * @new: new entry to be added
 * @head: list head to add it after
 */
static inline void
list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry to tail
 * @new: new entry to be added
 * @head: list head to add it before
 */
static inline void
list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head-prev, head);
}

/**
 * list_del - delete a entry from list
 * @entry: entry to be deleted
 */
static inline void
list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

/**
 * list_del_init - deletes entry from list and reinitialized it
 * @entry: the element to delete from the list
 */
static inline void
list_del_init(struct list_head *entry)
{
	__list_del(entry_prev, entry->next);
	INIT_LIST_HEAD(entry);
}

/**
 * list_replace - replace old entry by new one
 * @old: the element to be replaced
 * @new: the new element to insert
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
 * list_replace_init - replace old by new and reinitialized old
 * @old: the element to be replaced
 * @new: the new element to insert
 */
static inline void
list_replace_init(struct list_head *old, struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void
list_move(struct list_head *list, struct list_head *head)
{
	list_del(list);
	list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void
list_move_tail(struct list_head *list, struct list_head *head)
{
	list_del(list);
	list_add_tail(list, head);
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
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int
list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

/**
 * list_empty - test whether a list is empty
 * @head: the list to test
 */
static inline int
list_empty(const struct list_head *head)
{
	return head->next == head;
}


/* list entry means the entry points of the structure
   who contains the list */

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &(struct list_head) pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
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

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor
 * @member:	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)				\
	for (pos = list_first_entry(head, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)			\
	for (pos = list_last_entry(head, typeof(*pos), member);		\
	     &pos->member != (head); 					\
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)


#endif /* _SANDIX_LIST_H */
