#ifndef __BUDDY_H__
#define __BUDDY_H__
#include <vector>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
	可分配的最大内存，和保存完全二叉树的数组
class buddy
{
	unsigned size;
	vector<unsigned> longest;
};
*/

class buddy;

/* 
	size是整块的内存大小，然后通过数组的形式来构造完全二叉树，
    并且每个节点记录了可分配的内存大小
	*/
class buddy* buddy_new( int size );

/* 释放这个buddy管理内存 */
void buddy_destroy( class buddy* self );

/* 分配内存 ,返回的是这块内存相对于首位值得偏移量 */
int buddy_alloc(class buddy* self, int size);

/* 释放内存中得块 */
void buddy_free(class buddy* self, int offset);

/* 根据此偏移量，来计算此申请的大小 并返回 */
int buddy_size(class buddy* self, int offset);

/* 
	将分配的状态打印出来
   通过'_ '来表示未分配的块，用 ' * '来表示已分配的块 */
void buddy_dump(class buddy* self);

/*   测试  */
void buddy_text(class buddy* self);

#endif//__BUDDY_H__
