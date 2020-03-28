首先(Buddy)伙伴的定义：

	两个块大小相同
	两个块地址连续
	两个块必须是同一个大块中分离出来的

适用条件：
	
	  伙伴系统一般使用于大内存块的分配，并且是2的幂次

          ![image](https://github.com/Yonhoo/Buddy-memory-allocation-/blob/master/image/p12312931.jpg)

实现思路：
     
      
      伙伴算法的分配器的实现思路是，通过一个数组形式的完全二叉树来监控管理内存，二叉树的节点用于标记相应内存块的使用状态。

申请过程：

      比如以上面的二叉树为例，索引为1的节点的内存大小是4，此时分配出去了，那么此时更新父节点的内存大小，即是左右孩子的最大值，即此时0处表示能用内存是4(即右孩子还剩的内存)，高层节点对应大的块，低层节点对应小的块，在分配和释放中我们就通过这些节点的标记属性来进行块的分离合并。

释放过程：

    比如释放索引为1的节点的内存,加入索引为2的节点处是未分配的，那么通过回溯父节点，来查看右孩子是否是已分配的，来进行合并，然后将索引为0的节点处的大小改为8.

时间复杂度：

   由于这是一个完全二叉树，从而申请和释放的时间复杂度都在O(log N)内。
