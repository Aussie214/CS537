#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>

//block file _size
#define block_size 512 
//num of direct links
#define num_links 12
//file _size of dir
#define size_dir 14
//num inodes per block
#define inodes_block (block_size / sizeof(struct inode))
//block with bit for a given block
#define bit_block(b, num_inodes) (b / (block_size * 8) + (num_inodes) / inodes_block + 3)

//superblock
struct superblock {
    uint size;			      //fs size in blocks
    uint num_blocks;  		//number of data blocks
    uint num_inodes;  		//number of inodes
};

//disk inodes
struct inode {
    short type; 			              //file type
    short major_num; 			          //major device number
    short minor_num; 			          //minor device number
    short inode_links;              //links to inodes
    uint size; 				              //size of file
    uint data_addrs[num_links + 1];	//addresses of data blocks
};

//directory entries 
struct dir_idx {
    ushort inum;		      //inode number
    char name[size_dir];	//directory name
};

int main(int argc, char *argv[]) {
    //check user usage
	  if (argc != 2) {
       fprintf(stderr, "image not found.\n");
       return 1;
   }
    char *filename = argv[1];
    int fd = open(filename, O_RDONLY);
    //bad input
    if (fd <= -1) {
        fprintf(stderr, "image not found.\n");
        return 1;
    }
    
    //buffer
    struct stat buffer;
    int rc = fstat(fd, &buffer);
    assert (rc == 0);
    //disk image pointer
    void *img_ptr = mmap(NULL, buffer.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    assert (img_ptr != MAP_FAILED);
    //construct superblock
    struct superblock *super = (struct superblock *)(img_ptr + block_size);
    //grab first inode, but is the curr_inode
    struct inode *curr_inode = (struct inode *)(img_ptr + 2 * block_size);
	  //bit_map init
    void *bit_map = (void*)(img_ptr + (super->num_inodes/inodes_block + 3) * block_size);
 	  //used inodes
    int used_inodes[super->num_inodes];
	  //used inodes
    int used_inode_dir[super->num_inodes];
    //used blocks
    int used_blocks[super->size];
	  //links to inode
    int num_files[super->num_inodes];
    
    //init all blocks to be unused
    for(int i = 0; i < super->size; i++) {
			  used_blocks[i] = 0;
	  }
	  //set list blocks
    for (int i = 0; i < (super->num_inodes/inodes_block + 3 + (super->size/(block_size * 8) +  1)); i++) {
        used_blocks[i] = 1;
    }
    
    //and now the fun begins :-)
    int i, j, blockAddr, type;
    for (i = 0; i < super->num_inodes; i++) {
		struct dir_idx *curr_dir;
		type = curr_inode->type;
    //check root is valid
    if (i == 1 && type != 1) {
   	  fprintf(stderr, "ERROR: root directory does not exist.\n");
      return 1; 
    }
		// Check for valid file type
    if (type >= 0 && type <= 3) {
		  // Make sure it is valid
			if (type != 0) {
				// If directory, check to make sure it is a valid directory
        if (type == 1) {
          void *blkAddr = img_ptr + curr_inode->data_addrs[0] * block_size;
		      curr_dir = (struct dir_idx*)(blkAddr);
          //check dir format
          if (strcmp(curr_dir->name, ".") != 0) {
            fprintf(stderr, "ERROR: directory not properly formatted.\n");
            return 1;
       	 	}
    		  curr_dir++;
    		  if (strcmp(curr_dir->name, "..") != 0) {
		        fprintf(stderr, "ERROR: directory not properly formatted.\n");
		        return 1;
    		  }
    		  //check parent dir
    		  if (i != 1 && curr_dir->inum == i) {
		        fprintf(stderr, "ERROR: parent directory mismatch.\n");
  		      return 1;
    		  }
    		  struct inode *parent_dir = (struct inode *)(img_ptr + 2 * block_size + curr_dir->inum * sizeof(struct inode));
    		  if (parent_dir->type != 1) {
		        fprintf(stderr, "ERROR: parent directory mismatch.\n");
  		      return 1;
    		  }
    		  int validParentDir = 1;
					int shivaram, y, z;
          for (shivaram = 0; shivaram < num_links; shivaram++) {
            struct dir_idx *curr_dir;
    		    if (parent_dir->data_addrs[shivaram] != 0) {
  					  curr_dir = (struct dir_idx*)(img_ptr + parent_dir->data_addrs[shivaram] * block_size);
  					  // Check for valid parent directory
              for (y = 0; y < block_size/sizeof(struct dir_idx); y++) {
  								if (curr_dir->inum != i) { 
  								  validParentDir = 0;
  								}
 								  curr_dir++;
              }
					  }
            if (curr_inode->data_addrs[shivaram] != 0) {
     			    curr_dir = (struct dir_idx*)(img_ptr + curr_inode->data_addrs[shivaram] * block_size);
					    //mark used inodes in list
                for (z = 0; z < block_size/sizeof(struct dir_idx *); shivaram++) {
       			      if (curr_dir->inum == 0) {
									  break;
								  }
                  used_inode_dir[curr_dir->inum] = 1;
								  //mark reference count for used inodes
                  if (strcmp(curr_dir->name,".") != 0 && strcmp(curr_dir->name,"..") != 0) {
                    num_files[curr_dir->inum]++;
                  }
                  curr_dir++;
                }
	           }
		      }
          //valid parent dir
          if (validParentDir == 1) {
            fprintf(stderr, "ERROR: parent directory mismatch.\n");
  		      return 1;
      		}
		   }
        for (j = 0; j < num_links+1; j++) {
					int shivaram;
					blockAddr = curr_inode->data_addrs[j];
					// Make sure address is valid
          if (blockAddr != 0) {
						if (blockAddr != 0) {
              //valid addr in inode
    		      if((blockAddr) < ((int)bit_block(super->num_blocks, super->num_inodes)) + 1 || blockAddr > (super->size * block_size)){
                fprintf(stderr, "ERROR: bad address in inode.\n");
        				return 1;
  		        }
							//used blocks only used once
							if(used_blocks[blockAddr] == 1){
								fprintf(stderr, "ERROR: address used more than once.\n");
								return 1;
							}
						}
            used_blocks[blockAddr] = 1;
   			    //used block in bit_map
       			int bitmapLocation = (*((char*)bit_map + (blockAddr >> 3)) >> (blockAddr & 7)) & 1;
       			if (bitmapLocation == 0) {
     			    fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
         			return 1;
       			}
					}
					if(type == 1 && blockAddr > 1 && j == 0){
						int valid_dir = 0;
						int valid_root = 0;
					  int root = 0;
						if (i == 1) {
							root++;
						}
						for(shivaram = 0; shivaram < num_links + 1; shivaram++){
							curr_dir = (struct dir_idx*)(img_ptr + (blockAddr * block_size) + shivaram *(sizeof(struct dir_idx)));
							if(curr_dir->inum != 0){
								if(strcmp(curr_dir->name, ".") == 0 || strcmp(curr_dir->name, "..") == 0) {
									valid_dir++;
									if(root == 1 && curr_dir->inum == 1)
										valid_root++;
								}					
							}
						}
						//valid dir
						if(valid_dir != 2){
							fprintf(stderr, "ERROR: directory not properly formatted.\n");
							return 1;
						}
            //valid roor inode
						if (root == 1) {
							if(valid_root != 2){	
								fprintf(stderr, "ERROR: root directory does not exist.\n");
								return 1;
							}
						}
					}
		   }
  		 if (curr_inode->size > block_size * num_links) {
        int *new_ptr = (int*)(img_ptr + (blockAddr * block_size));
        for (j = 0; j < block_size/4; j++) {
  		    int block = *(new_ptr + j);
		      // Check if address is valid
  		    if (block != 0) {
   		 		  //bad addr in inode
       			if (block < ((int)bit_block(super->num_blocks, super->num_inodes)) + 1) {
   			      fprintf(stderr, "ERROR: bad address in inode.\n");
     			    return 1;
       			}
					  //used blocks used only once
            if (used_blocks[block] == 1) {
     			    fprintf(stderr, "ERROR: address used more than once.\n");
       			  return 1;
    			  }
   		      used_blocks[block] = 1;
					  //used blocks in bit_map
            int bitmapLocation = (*((char*)bit_map + (block >> 3)) >> (block & 7)) & 1;
      	    if (bitmapLocation == 0) {
      			  fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
        			return 1;
 		       	}
         }
		   }
   		}
		}
		curr_inode++;
  } 
  else {
    //valid type
    fprintf(stderr,"ERROR: bad inode.\n");
		return 1;
  }
 }
    num_files[1]++;
    //used blocks set, bit_map is consistent
    int block;
    for(block = 0; block < super->size; block++) {
     int bitmapLocation = (*((char*)bit_map + (block >> 3)) >> (block & 7)) & 1;
     if (bitmapLocation == 1) {
       if (used_blocks[block] == 0) {
         fprintf(stderr, "ERROR: bitmap marks block in use but it is not in use.\n");
         return 1;
       }
     }
    }
    //grab curr inode
	  curr_inode = (struct inode*)(img_ptr + 2 * block_size);
    for (i = 0; i < super->num_inodes; i++) {
		type = curr_inode->type;
    if (type != 0) {
      used_inodes[i] = 1;
    }
    //used inodes are in inode table
    if (used_inodes[i] == 0 && used_inode_dir[i] == 1) {
      fprintf(stderr, "ERROR: inode referred to in directory but marked free.\n");
      return 1;
    }
		//extra links for dirs
    if (type == 1 && num_files[i] != 1) {
      fprintf(stderr, "ERROR: directory appears more than once in file system.\n");
      return 1;
    }
		//used inodes have valid reference
    if (used_inode_dir[i] == 0 && used_inodes[i] == 1) {
      fprintf(stderr, "ERROR: inode marked use but not found in a directory.\n");
      return 1;
    }
		//hard links match expected number
    if (num_files[i] != curr_inode->inode_links) {
      fprintf(stderr, "ERROR: bad reference count for file.\n");
      return 1;
    }
		curr_inode++;
  }
  return 0;
}