/*
 *	EXT2 super-block data in memory
 *  
 *	2015/03/30 Created by Shan Yizhou.
 *  All the code is derived from linux kernel.
 */

/*
 * second extended file system inode data in memory
 */
struct ext2_inode_info {
	__u32	i_data[15];
	__u32	i_flags;
	__u32	i_faddr;
	__u8	i_frag_no;
	__u8	i_frag_size;
	__u16	i_osync;
	__u32	i_file_acl;
	__u32	i_dir_acl;
	__u32	i_dtime;
	__u32	i_version;
	__u32	i_block_group;
	__u32	i_next_alloc_block;
	__u32	i_next_alloc_goal;
	__u32	i_prealloc_block;
	__u32	i_prealloc_count;
	int	i_new_inode:1;	/* Is a freshly allocated inode */
};
