#ifndef __BUDDY_H__
#define __BUDDY_H__
#include <vector>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
	�ɷ��������ڴ棬�ͱ�����ȫ������������
class buddy
{
	unsigned size;
	vector<unsigned> longest;
};
*/

class buddy;

/* 
	size��������ڴ��С��Ȼ��ͨ���������ʽ��������ȫ��������
    ����ÿ���ڵ��¼�˿ɷ�����ڴ��С
	*/
class buddy* buddy_new( int size );

/* �ͷ����buddy�����ڴ� */
void buddy_destroy( class buddy* self );

/* �����ڴ� ,���ص�������ڴ��������λֵ��ƫ���� */
int buddy_alloc(class buddy* self, int size);

/* �ͷ��ڴ��еÿ� */
void buddy_free(class buddy* self, int offset);

/* ���ݴ�ƫ�����������������Ĵ�С ������ */
int buddy_size(class buddy* self, int offset);

/* 
	�������״̬��ӡ����
   ͨ��'_ '����ʾδ����Ŀ飬�� ' * '����ʾ�ѷ���Ŀ� */
void buddy_dump(class buddy* self);

/*   ����  */
void buddy_text(class buddy* self);

#endif//__BUDDY_H__
