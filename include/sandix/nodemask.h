/*
 *	Copyright (C) 2016 Yizhou Shan <shan13@purdue.edu>
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

#ifndef _SANDIX_NODEMASK_H_
#define _SANDIX_NODEMASK_H_

#include <sandix/numa.h>
#include <sandix/types.h>
#include <sandix/bitops.h>
#include <sandix/kernel.h>

typedef struct nodemask {
	DEFINE_BITMAP(bits, MAX_NR_NODES);
} nodemask_t;

/**
 * NODE_STATES	-	Bitmasks that are kept for all the nodes
 *
 * NODE_POSSIBLE:	The node could become online at some point
 * NODE_ONLINE:		The node is online
 * NODE_NORMAL_MEMORY:	The node has regular memory
 * NODE_HIGH_MEMORY:	The node has regular or high memory
 * NODE_MEMORY:		The node has memory (regular, high, movable)
 * NODE_CPU:		The node has one or more CPUs
 */
enum NODE_STATES {
	NODE_POSSIBLE,
	NODE_ONLINE,
	NODE_NORMAL_MEMORY,
#ifdef CONFIG_HIGHMEM
	NODE_HIGH_MEMORY,
#else
	NODE_HIGH_MEMORY = NODE_NORMAL_MEMORY,
#endif
#ifdef CONFIG_MOVABLE_NODE
	NODE_MEMORY,
#else
	NODE_MEMORY = NODE_HIGH_MEMORY,
#endif
	NODE_CPU,

	__NR_NODE_STATES
};

extern struct nodemask node_states[__NR_NODE_STATES];

#if MAX_NR_NODES > 1

/*
 * TODO: Fix this after bitmap is added.
 */

extern int nr_node_ids;
extern int nr_online_nodes;

static inline void node_set_state(int node, enum NODE_STATES state)
{
	set_bit(node, &node_states[state]->bits);
}

static inline void node_clear_state(int node, enum NODE_STATES state)
{
	clear_bit(node, &node_states[state]->bits);
}

#else /* !MAX_NR_NODES > 1 */

static inline int node_state(int node, enum NODE_STATES state)
{
	return node == 0;
}

static inline void node_set_state(int node, enum NODE_STATES state)
{
}

static inline void node_clear_state(int node, enum NODE_STATES state)
{
}

static inline int num_node_state(enum NODE_STATES state)
{
	return 1;
}

#define for_each_node_state(node, __state) \
	for ( (node) = 0; (node) == 0; (node) = 1)

#define first_online_node		0
#define first_memory_node		0
#define next_online_node(nid)		(MAX_NR_NODES)
#define nr_node_ids			1
#define nr_online_nodes			1

#define node_set_online(node)		node_set_state((node), NODE_ONLINE)
#define node_set_offline(node)		node_clear_state((node), NODE_ONLINE)

#endif /* MAX_NR_NODES > 1 */

#define node_online_map			node_states[NODE_ONLINE]
#define node_possible_map		node_states[NODE_POSSIBLE]

#define num_online_nodes()		num_node_state(NODE_ONLINE)
#define num_possible_nodes()		num_node_state(NODE_POSSIBLE)
#define node_online(node)		node_state((node), NODE_ONLINE)
#define node_possible(node)		node_state((node), NODE_POSSIBLE)

#define for_each_node(node)		for_each_node_state(node, NODE_POSSIBLE)
#define for_each_online_node(node)	for_each_node_state(node, NODE_ONLINE)

#endif /* _SANDIX_NODEMASK_H_ */
