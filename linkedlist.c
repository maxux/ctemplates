/* linkedlist - small code for generic linked list indexed by string
 * Author: Daniel Maxime (root@maxux.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

list_t * list_init(void (*destruct)(void *)) {
	list_t *list;
	
	if(!(list = (list_t*) malloc(sizeof(list_t))))
		return NULL;
	
	/* Preparing environment */
	list->destruct  = destruct;
	list->nodes     = NULL;
	list->length    = 0;
	
	return list;
}

void list_free(list_t *list) {
	list_node_t *node = list->nodes, *temp;
	
	/* Free resident data */
	while(node) {
		if(list->destruct)
			list->destruct(node->data);
			
		free(node->name);
		
		temp = node;
		node = node->next;
		
		free(temp);
	}
	
	free(list);
}

void * list_append(list_t *list, char *name, void *data) {
	list_node_t *node;
	
	if(!(node = (list_node_t*) malloc(sizeof(list_node_t))))
		return NULL;
	
	/* Copy key name */
	node->name = strdup(name);
	node->data = data;
	
	/* Linking data */
	node->next  = list->nodes;	
	list->nodes = node;
	
	/* Updating list */
	list->length++;
	
	return data;
}

void * list_search(list_t *list, char *name) {
	list_node_t *node = list->nodes;
	
	/* Linear search */
	while(node && strcmp(node->name, name))
		node = node->next;
	
	return (node) ? node->data : NULL;
}

int list_remove(list_t *list, char *name) {
	list_node_t *node = list->nodes;
	list_node_t *prev = list->nodes;
	
	/* Searching node */
	while(node && strcmp(node->name, name)) {
		prev = node;
		node = node->next;
	}
	
	if(!node)
		return 1;
	
	/* Syncing list */
	if(node == list->nodes)
		list->nodes = node->next;
		
	else prev->next = node->next;
	
	list->length--;
	
	/* Call user destructor */
	if(list->destruct)
		list->destruct(node->data);
	
	free(node->name);	
	free(node);
	
	return 0;
}
