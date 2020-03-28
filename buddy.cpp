
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



//��ڵ�
#define LEFT_LEAF(index) (((index) << 1) + 1)
//�ҽڵ�
#define RIGHT_LEAF(index) (((index) << 1) + 2)
//���ڵ�
#define PARENT(index) (((index) - 1) >> 1)
//�ж�һ�����Ƿ���2����
#define IS_POWER_OF_2(x) (!((x)&((x)-1)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ALLOC malloc
#define FREE free

//��Ϊ��2���ݴΣ��������λΪ1������λΪ0
//�Ӷ�ͨ��������λ�ó�1��Ȼ���1���ͱ��2���ݴ�
//ͨ�����λ��1������һλ����˸�λ���1���Ӷ�����ʹ�ôθ�λ�����ΪΪ1
// |= 2 �����ǽ�ǰ4λ���1
// |= 4 ��ǰ8λ
// |= 8 ��ǰ16λ
// |= 16��ǰ32λ
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

	//���������ڴ���4
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//�������ȫ����������������ʵ�ֵ�
	//�����size��2*size-1
	for (i = 0; i < 2 * size - 1; ++i)
	{
		if (IS_POWER_OF_2(i + 1))
			node_size /= 2;
		self->longest[i] = node_size;   //ÿ����ȫ�������еĽڵ�洢�����ڴ�Ĵ�С
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
	//�����ʱ���ڴ����ʣ����ڴ��С����������Ĵ�С���ͷ���
	if (self->longest[index] < size)
		return -1;

	//�Ӹ��ڵ㿪ʼ�����ң�
	//���������ڴ���4
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//��0�������ӽڵ㿪ʼ������
	//ֱ���ҵ�����˴��ڴ�����Ľڵ㣬��������2�ģ�Ӧ���Ǵ�3����ڵ㷵�ص�
	for (node_size = self->size; node_size != size; node_size /= 2)
	{
		if (self->longest[LEFT_LEAF(index)] >= size)
			index = LEFT_LEAF(index);
		else
			index = RIGHT_LEAF(index);

	}

	//���ڴ汻ռ�ã��Ӷ���Ϊ0
	self->longest[index] = 0;
	offset = (index + 1) * node_size - self->size;	//�������ƫ������������

	//�޸��丸�ڵ������ڴ�,ʱ�临�ӶȲ�����logn
	while (index)
	{
		index = PARENT(index);
		self->longest[index] =
			MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
	}

	//���offset��ָ����ڷ�����������ڴ��λ�ƣ������Ǵ��������λ����4��ʼ����ôoffest����4
	return offset;
}

void buddy_free(class buddy* self, int offset)
{
	unsigned node_size, index = 0;
	unsigned left_longest, right_longest;

	//����ƫ�����ǿ϶�С���������ڴ��
	assert(self && offset >= 0 && offset < self->size);

	node_size = 1;
	//����ƫ�ƣ����㴦����
	index = offset + self->size - 1;
	//ͨ�����ݣ����ڴ���лָ�
	for (; self->longest[index]; index = PARENT(index))
	{
		node_size *= 2;
		if (index == 0)
			return;
	}

	self->longest[index] = node_size;

	//  �ϲ������ӽڵ�,��������ǻ��,���Ҷ���2���ݴ�,��Ϊ��ͨ����ȫ������ʵ�ֵ�
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
	//���ص��Ǹ�ƫ�ƴ��ѷ����Ĵ�С
	//          8          0
	//                   /   \
	//          4       1     2 
	//                 / \   / \
	//          2     3   4 5   6
	//��������5���Ѿ������ȥ�ˣ���ô��5���ӽڵ㿪ʼ
	//��11����Ȼ��node_sizeΪ2��Ȼ�󵽴︸�ڵ㣬��5������ѷ���Ŀ飬 
	//��Ȼ��ʱֵΪ0�����������ص�����������5�������Ĵ�С
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

	if (self->size > 64)	//�������Ʒ����������64
	{
		printf("buddy_dump: (struct buddy*)self is too big to dump");
		return;
	}

	memset(canvas, '_', sizeof(canvas));
	node_size = self->size * 2;

	for (i = 0; i < 2 * self->size - 1; ++i)
	{
		if (IS_POWER_OF_2(i + 1))
			node_size /= 2;		//�൱����������һ��

		//�ѷ����
		if (self->longest[i] == 0)
		{
			if (i >= self->size - 1)
			{
				canvas[i - self->size + 1] = '*';
			}//������ڵ��ѷ��䣬��ô�����е��ӽڵ�ȫ����� '*'
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