#include <stdio.h>

typedef struct node {
	int val;
	struct node* next;
} node;

/* FIXME: this function is buggy. */
int ll_equal(const node* a, const node* b) {
	while (a != NULL && b != NULL) {
		if (a->val != b->val)
			return 0;
		a = a->next;
		b = b->next;
	}
	/* lists are equal if a and b are both null */
	return a == b;
}

int main(int argc, char** argv) {
	int i;
	node nodes[10];     //Array of 10 nodes

	for (i=0; i<10; i++) {
		nodes[i].val = 0;
		nodes[i].next = NULL;
	}

	nodes[0].next = &nodes[1];
	nodes[1].next = &nodes[2];
	nodes[2].next = &nodes[3];

	printf("equal test 1 result = %d\n", ll_equal(&nodes[0], &nodes[0]));
    
    //Top one works because you're literally comparing it to itself
    
	printf("equal test 2 result = %d\n", ll_equal(&nodes[0], &nodes[2]));
    
    //Second one can move one more step along nodes but encounters a node that points to empty more quickly than A does, so need a second null check

	return 0;
}

