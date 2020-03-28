
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "buddy.h"


class buddy
{
public:
	buddy(int num) :size(num), longest(std::move(vector<unsigned>(2 * size - 1))) {};

	unsigned size;
	vector<unsigned> longest;
};



//左节点
#define LEFT_LEAF(index) (((index) << 1) + 1)
//右节点
#define RIGHT_LEAF(index) (((index) << 1) + 2)
//父节点
#define PARENT(index) (((index) - 1) >> 1)
//判断一个数是否是2的幂
#define IS_POWER_OF_2(x) (!((x)&((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ALLOC malloc
#define FREE free

//因为是2的幂次，就是最高位为1，其余位为0
//从而通过将所有位置成1，然后加1，就变成2的幂次
//通过最高位是1，右移一位，则此高位变成1，从而做或，使得次高位和最高为为1
// |= 2 ，则是将前4位变成1
// |= 4 ，前8位
// |= 8 ，前16位
// |= 16，前32位
static unsigned Fixsize(unsigned size)
{
	size |= size >> 1;
	size |= size >> 2;
	size |= size >> 4;
	size |= size >> 8;
	size |= size >> 16;
	return size + 1;
}

class buddy* buddy_new(int size)
{
	buddy* self;
	unsigned node_size;
	int i;

	if (size < 1 || !IS_POWER_OF_2(size))
		return NULL;

	self = new buddy(size);
	node_size = size * 2;

	//比如申请内存是4
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//这里的完全二叉树是用数组来实现的
	//这里的size是2*size-1
	for (i = 0; i < 2 * size - 1; ++i)
	{
		if (IS_POWER_OF_2(i + 1))
			node_size /= 2;
		self->longest[i] = node_size;   //每个完全二叉树中的节点存储的是内存的大小
	}
	return self;
}

void buddy_destroy(class buddy* self)
{
	FREE(self);
}

int buddy_alloc(class buddy* self, int size)
{
	unsigned index = 0;
	unsigned node_size;
	unsigned offset = 0;

	if (self == NULL)
		return -1;

	if (size <= 0)
		size = 1;
	else if (!IS_POWER_OF_2(size))
		size = Fixsize(size);
	//如果此时大内存块中剩余的内存大小不满足申请的大小，就返回
	if (self->longest[index] < size)
		return -1;

	//从根节点开始往下找，
	//比如申请内存是4
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//从0的左右子节点开始往下找
	//直到找到满足此次内存需求的节点，比如满足2的，应该是从3这个节点返回的
	for (node_size = self->size; node_size != size; node_size /= 2)
	{
		if (self->longest[LEFT_LEAF(index)] >= size)
			index = LEFT_LEAF(index);
		else
			index = RIGHT_LEAF(index);

	}

	//此内存被占用，从而置为0
	self->longest[index] = 0;
	offset = (index + 1) * node_size - self->size;	//这个计算偏移量，很神奇

	//修改其父节点的最大内存,时间复杂度不超过logn
	while (index)
	{
		index = PARENT(index);
		self->longest[index] =
			MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
	}

	//这个offset是指相对于分配的这整块内存的位移，比如是从这整块的位移量4开始，那么offest就是4
	return offset;
}

void buddy_free(class buddy* self, int offset)
{
	unsigned node_size, index = 0;
	unsigned left_longest, right_longest;

	//首先偏移量是肯定小于这整块内存的
	assert(self && offset >= 0 && offset < self->size);

	node_size = 1;
	//根据偏移，计算处索引
	index = offset + self->size - 1;
	//通过上溯，将内存进行恢复
	for (; self->longest[index]; index = PARENT(index))
	{
		node_size *= 2;
		if (index == 0)
			return;
	}

	self->longest[index] = node_size;

	//  合并左右子节点,这个左右是伙伴,并且都是2的幂次,因为是通过完全二叉树实现的
	while (index)
	{
		index = PARENT(index);
		node_size *= 2;

		left_longest = self->longest[LEFT_LEAF(index)];
		right_longest = self->longest[RIGHT_LEAF(index)];

		if (left_longest + right_longest == node_size)
			self->longest[index] = node_size;
		else
			self->longest[index] = MAX(left_longest, right_longest);
	}
}

int buddy_size(class buddy* self, int offset)
{
	unsigned node_size, index = 0;

	assert(self && offset >= 0 && offset < self->size);

	node_size = 1;
	//返回的是该偏移处已分配块的大小
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//比如索引5处已经分配出去了，那么从5的子节点开始
	//即11处，然后node_size为2，然后到达父节点，即5处这个已分配的块， 
	//显然此时值为0，跳出，返回的正好是索引5处这个块的大小
	for (index = offset + self->size - 1; self->longest[index]; index = PARENT(index))
		node_size *= 2;

	return node_size;
}

void buddy_dump(class buddy* self)
{
	char canvas[65];
	int i, j;
	unsigned node_size, offset;

	if (self == NULL)
	{
		printf("buddy_dump: (struct buddy*)self == NULL");
		return;
	}

	if (self->size > 64)	//这里限制分配的最大块是64
	{
		printf("buddy_dump: (struct buddy*)self is too big to dump");
		return;
	}

	memset(canvas, '_', sizeof(canvas));
	node_size = self->size * 2;

	for (i = 0; i < 2 * self->size - 1; ++i)
	{
		if (IS_POWER_OF_2(i + 1))
			node_size /= 2;		//相当于来到了下一层

		//已分配的
		if (self->longest[i] == 0)
		{
			if (i >= self->size - 1)
			{
				canvas[i - self->size + 1] = '*';
			}//即这个节点已分配，那么它所有的子节点全都变成 '*'
			else if (self->longest[LEFT_LEAF(i)] && self->longest[RIGHT_LEAF(i)])
			{
				offset = (i + 1) * node_size - self->size;

				for (j = offset; j < offset + node_size; ++j)
					canvas[j] = '*';
			}
		}
	}
	canvas[self->size] = '\0';
	puts(canvas);
}


void buddy_text(class buddy* self)
{
	string cmd;
	int arg;
	buddy_dump(self);
	for (;;) {
		cin >> cmd >> arg;
		if (strcmp(cmd.c_str(), "alloc") == 0)
		{
			printf("allocated@%d\n", buddy_alloc(self, arg));
			buddy_dump(self);
		}
		else if (strcmp(cmd.c_str(), "free") == 0)
		{
			buddy_free(self, arg);
			buddy_dump(self);
		}
		else if (strcmp(cmd.c_str(), "size") == 0)
		{
			printf("size: %d\n", buddy_size(self, arg));
			buddy_dump(self);
		}
		else
			buddy_dump(self);
	}
}