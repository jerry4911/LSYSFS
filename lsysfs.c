/**
 * Less Simple, Yet Stupid Filesystem.
 * 
 * Mohammed Q. Hussain - http://www.maastaar.net
 *
 * This is an example of using FUSE to build a simple filesystem. It is a part of a tutorial in MQH Blog with the title "Writing Less Simple, Yet Stupid Filesystem Using FUSE in C": http://maastaar.net/fuse/linux/filesystem/c/2019/09/28/writing-less-simple-yet-stupid-filesystem-using-FUSE-in-C/
 *
 * License: GNU GPL
 */
 
#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h> /* Definition of AT_* constants */
#include <sys/stat.h>

// ... //
struct file{

	char name[256];
	time_t atime;
	time_t mtime;
	int parent_dir_index;

};

struct file dir_list[ 256 ];
//char dir_list[ 256 ][ 256 ];
int curr_dir_idx = -1;

struct file files_list[ 256 ];
//char files_list[ 256 ][ 256 ];
int curr_file_idx = -1;

char files_content[ 256 ][ 256 ];
int curr_file_content_idx = -1;

void add_dir( const char *dir_name )
{
        printf("-----------------------\nadd_dir\n\n");
	curr_dir_idx++;
	strcpy( dir_list[ curr_dir_idx ].name, dir_name );
	dir_list[ curr_dir_idx ].atime = time( NULL );
	dir_list[ curr_dir_idx ].mtime = time( NULL );

}

int is_dir( const char *path )
{
        
	path++; // Eliminating "/" in the path

        int curr_idx;
	
	for ( curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
		if ( strcmp( path, dir_list[ curr_idx ].name ) == 0 )
			return 1;
	
	return 0;
}

void add_file( const char *filename )
{
        printf("-----------------------\nadd_file\n\n");
	curr_file_idx++;
	strcpy( files_list[ curr_file_idx ].name, filename );
	
	curr_file_content_idx++;
	strcpy( files_content[ curr_file_content_idx ], "" );

	files_list[ curr_file_idx ].atime = time( NULL );
	files_list[ curr_file_idx ].mtime = time( NULL );
}

int is_file( const char *path )
{

	path++; // Eliminating "/" in the path
        
        int curr_idx;
	
	for ( curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ].name ) == 0 )
			return 1;
	
	return 0;
}

int get_file_index( const char *path )
{
	path++; // Eliminating "/" in the path
        
        int curr_idx;
	
	for ( curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ].name ) == 0 )
			return curr_idx;
	
	return -1;
}

int get_dir_index( const char *path )
{
	path++; // Eliminating "/" in the path
        
        int curr_idx;
	
	for ( curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ].name ) == 0 )
			return curr_idx;
	
	return -1;
}

void write_to_file( const char *path, const char *new_content )
{
	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 ) // No such file
		return;
		
	strcpy( files_content[ file_idx ], new_content );
	files_list[ file_idx ].atime = time( NULL );
	files_list[ file_idx ].mtime = time( NULL );
}

// ... //

static int do_getattr( const char *path, struct stat *st )
{
	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
        if ( get_file_index( path ) == -1 ){

		printf("New file!!!\n");
		st->st_atime = time( NULL );
		st->st_mtime = time( NULL ); 
	}
	else{
		
		printf("Old file......\n");

	}
	//st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	//st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
	
	if ( strcmp( path, "/" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
		
		int dir_index = get_dir_index( path );
		st->st_atime = dir_list[ dir_index ].atime;
		st->st_mtime = dir_list[ dir_index ].mtime;
	}
	else if ( is_file( path ) == 1 )
	{
		
		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		st->st_size = 1024;

		int file_index = get_file_index( path );
		st->st_atime = files_list[ file_index ].atime;
		st->st_mtime = files_list[ file_index ].mtime;

	}
	else
	{
		return -ENOENT;
	}
	
	return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
    	printf("-----------------------\nread_dir\n\n");
        printf("path is %s\n", path );
        printf("offset is %lld\n", offset );
        int path_index = get_dir_index( path );
        printf("path_index is %d\n", path_index );
         
        filler( buffer, ".", NULL, 0 ); // Current Directory
	filler( buffer, "..", NULL, 0 ); // Parent Directory
        
	
	if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
	{

                int curr_idx;
		
                for ( curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
			filler( buffer, dir_list[ curr_idx ].name, NULL, 0 );
	
		for ( curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
			filler( buffer, files_list[ curr_idx ].name, NULL, 0 );
	}
	
	return 0; 

}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
        printf("-----------------------\nread\n\n");

	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 )
		return -1;
	
	char *content = files_content[ file_idx ];
	
	memcpy( buffer, content + offset, size );
		
	return strlen( content ) - offset;
}

static int do_mkdir( const char *path, mode_t mode )
{
        printf("-----------------------\nmkdir\n\n");
	printf("path is %s\n", path);
        path++;
        printf("path is %s\n", path);
	add_dir( path );
	
	return 0;
}

static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
        printf("-----------------------\nmknod\n\n");
	path++;
	add_file( path );
	
	return 0;
}

static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
	write_to_file( path, buffer );
	
	return size;
}

static int do_rmdir( const char *path )
{
        printf("-----------------------\nrmdir\n\n");
	int dir_index = get_dir_index( path );

	int curr_idx;
		
        for ( curr_idx = dir_index+1; curr_idx < curr_dir_idx; curr_idx++ )
		strcpy( dir_list[ curr_idx-1 ].name, dir_list[ curr_idx ].name);

	curr_dir_idx--;
	return 0;
}

static int do_unlink( const char *path )
{
        printf("-----------------------\nunlink\n\n");
        if ( is_file(path) ){

		int file_index = get_file_index( path );
	
		int curr_idx;
			
	        for ( curr_idx = file_index+1; curr_idx < curr_file_idx; curr_idx++ )
			strcpy( files_list[ curr_idx-1 ].name, files_list[ curr_idx ].name);
	
		curr_file_idx--;

		for ( curr_idx = file_index+1; curr_idx < curr_file_content_idx; curr_idx++ )
			strcpy( files_content[ curr_idx-1 ], files_content[ curr_idx ]);
	
		curr_file_content_idx--;
	}
	return 0;
}

static int do_utimens( const char *path, const struct timespec ts[2])
{
        printf("-----------------------\nutimens\n\n");

        if ( is_file(path) ){
		
		int file_idx = get_file_index( path );
	
		if ( file_idx == -1 ){
			
			path++;
			add_file( path );
		
		}
		else{

			files_list[ file_idx ].atime = time( NULL );
			files_list[ file_idx ].mtime = time( NULL );

		}

	}

	return 0;
}

static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
    .mkdir		= do_mkdir,
    .mknod		= do_mknod,
    .write		= do_write,
    .rmdir              = do_rmdir,
    .unlink             = do_unlink,
    .utimens            = do_utimens,
};

int main( int argc, char *argv[] )
{       

        printf("=======================\n");
	return fuse_main( argc, argv, &operations, NULL );
}