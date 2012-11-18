#include <linux/config.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include "struct.h"

MODULE_AUTHOR( "Piotr Truszkowski" );
MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "Alternatywne zrodlo informacji o procesach" );

int  altpinfo_major   =   42; /* jak chcemy dynamicznie przydzielac = 0, */
char *fmt             =   "pid";

module_param( fmt, charp, 0 );
MODULE_PARM_DESC( fmt, "Format informacji o procesie" );

int altpinfo_open( struct inode *inode, struct file *filp ) {
	struct altpinfo_t * info;
	print_debug( "execute altpinfo_open().\n" );

	info = ( struct altpinfo_t * ) kmalloc( sizeof(struct altpinfo_t), GFP_KERNEL );
	if ( !(info) ) { 
		print_warn( "kmalloc = Error, ( size= %d )\n", sizeof( struct altpinfo_t ) );
		return -ENOMEM;
	} else 
    print_debug( "kmalloc = Ok, ( adress= %u, size= %d )\n", (unsigned int) info, sizeof( struct altpinfo_t ) );
	
	filp->private_data = info;
	sema_init( &info->mutex, 1 );
	sema_init( &info->read , 0 );
	sema_init( &info->write, 1 );
	info->buffer = 0;
	info->buflen = 0;
	info->bufreallen = 0;
	info->bufpos = 0;
	return 0;
}

int altpinfo_release( struct inode *inode, struct file *filp ) {
	print_debug( "execute altpinfo_release(), adress = %u\n", (unsigned int)filp->private_data );
	free_altbuffer( filp->private_data );
	if ( filp->private_data ) {	
		kfree( filp->private_data );
		filp->private_data = NULL;
	}
	return 0;
}

ssize_t altpinfo_read( struct file *filp, char __user *buf, size_t count, loff_t *f_pos ) {	
	struct altpinfo_t * info = filp->private_data;
	print_debug( "execute altpinfo_read(), adress = %u\n", (unsigned int) info );

	if ( down_interruptible( &info->read ) )
		return -ERESTARTSYS;

	if ( down_interruptible( &info->mutex ) ) {
		up( &info->read );
		return -ERESTARTSYS;
	}

	if ( info->bufpos + count < info->buflen )
		up( &info->read );
	
	count = ( info->bufpos + count < info->buflen ? count : info->buflen - info->bufpos );	
	count -= __copy_to_user( buf, info->buffer + info->bufpos, count );
	info->bufpos += count;
	
	if ( info->bufpos == info->buflen )
		up( &info->write );
	
	up( &info->mutex );

	return count;
}

static inline int is_all( const char * buf, int len ) {
	if ( len < 3 ) return 0;
	if ( buf[0] != 'a' || buf[1] != 'l' || buf[2] != 'l' )
		return 0;
	if ( len > 3 )
		if ( buf[3] != '\0' && buf[3] != '\x00' && buf[3] != '\n' )
			return 0;
	return 1;
}

static inline int get_int( const char * buf, int len ) {
	int i, num = 0;
	for ( i = 0; i < len; ++i )
		if ( buf[i] <= '9' && buf[i] >= '0' )
			num = 10*num + buf[i] - '0';
		else return 0;
	return num;
}

static inline int put_infos( struct altpinfo_t * info, struct task_struct * ts, int offset, altpinfo_format_t * fmt ) {
	struct task_altpinfo talt;
	int i;
	print_debug( "execute put_infos(), task adress = %u\n", (unsigned) ts );
	get_task_altpinfo( ts, &talt );
	print_debug( "info = %d %d %s %c %lu %d\n", talt.pid, talt.uid, talt.comm, talt.state, talt.vsize, talt.sid );
/* pid %d, uid %d, comm %s, state %c, vsize %lu, session %d */
	for ( i = 0; fmt[i] != ALTPINFO_FORMAT_NULL; ++i )
		switch ( fmt[i] ) {
		case ALTPINFO_FORMAT_ERROR:
			if ( i ) /* Tak na wszelki wypadek, */
				sprintf( info->buffer + offset, "%s+Err", info->buffer + offset );
			else sprintf( info->buffer + offset, "Err" );
			break;
		case ALTPINFO_FORMAT_PID:
			if ( i )
				sprintf( info->buffer + offset, "%s+%d" , info->buffer + offset, talt.pid );
			else sprintf( info->buffer + offset, "%d" , talt.pid );
			break;
		case ALTPINFO_FORMAT_UID:
			if ( i )
				sprintf( info->buffer + offset, "%s+%d" , info->buffer + offset, talt.uid );
			else sprintf( info->buffer + offset, "%d" , talt.uid );
			break;
		case ALTPINFO_FORMAT_COMM:
			if ( i )
				sprintf( info->buffer + offset, "%s+%s" , info->buffer + offset, talt.comm );
			else sprintf( info->buffer + offset, "%s" , talt.comm );
			break;
		case ALTPINFO_FORMAT_STATE:
			if ( i )
				sprintf( info->buffer + offset, "%s+%c" , info->buffer + offset, talt.state );
			else sprintf( info->buffer + offset, "%c" , talt.state );
			break;
		case ALTPINFO_FORMAT_VSIZE:
			if ( i )
				sprintf( info->buffer + offset, "%s+%lu", info->buffer + offset, talt.vsize );
			else sprintf( info->buffer + offset, "%lu", talt.vsize );
			break;
		case ALTPINFO_FORMAT_SESSION:
			if ( i )
				sprintf( info->buffer + offset, "%s+%d" , info->buffer + offset, talt.sid );
			else sprintf( info->buffer + offset, "%d" , talt.sid );
			break;
		default:
			print_warn( "put_infos = Warning, ( nieznany format )\n" );
		}
	info->buflen = strlen( info->buffer );
	return strlen( info->buffer + offset );
}

static inline int put_single_infos( struct altpinfo_t * info, int pid, altpinfo_format_t * fmt, int fmtlen ) {
	struct task_struct * ts;
	print_debug( "execute put_single_infos().\n" );

	ts = find_task_by_pid( pid );
	if ( !ts )
		return -EINVAL;

	if ( alloc_altbuffer( info, fmtlen ) )
		return -ENOMEM;
		
	return put_infos( info, ts, 0, fmt );
};

static inline int put_multi_infos( struct altpinfo_t * info, altpinfo_format_t * fmt, int fmtlen ) {
	int size = 0, ret = 0;
	struct task_struct * ts;
	print_debug( "execute put_multi_infos().\n" );
	
	for_each_process( ts ) { size += fmtlen; } 
	if ( alloc_altbuffer( info, size ) )
		return -ENOMEM;
	
	for_each_process( ts ) {
		ret += put_infos( info, ts, ret, fmt );
		info->buffer[ret] = '\n'; 
		info->buflen = ++ret;
	} /* gratisowo dostajemy na koncu enter, */
	
	return ret;
};

ssize_t altpinfo_write( struct file *filp, const char __user *buf, size_t count, loff_t *f_pos ) {
	int val, ret = 0, fmtlen;
	char * mybuf;
	altpinfo_format_t * cpfmt;
	struct altpinfo_t * info = filp->private_data;
	
	print_debug( "execute altpinfo_write(), adress = %u\n", (unsigned int) info );	
	
	if ( count >= WRITE_LIMIT )
		return -EINVAL;
	
	if ( down_interruptible( &info->write ) )
		return -ERESTARTSYS;
	
	if ( down_interruptible( &info->mutex ) ) {
		up( &info->write );
		return -ERESTARTSYS;
	}
/* kopiujemy fmt dla aktualnego write() */

	cpfmt = kmalloc(GFP_KERNEL, BUFLEN);
	mybuf = kmalloc(GFP_KERNEL, BUFLEN);
	
	if ( !cpfmt && !mybuf )
	    return -ENOMEM;

	copy_fmt( cpfmt, fmtlen );

	count -= __copy_from_user( mybuf, buf, count );	
	
	if ( is_all( mybuf, count ) ) 
		ret = put_multi_infos( info , cpfmt, fmtlen );
	else if (	( val = get_int( mybuf, count ) ) > 0 ) 
		ret = put_single_infos( info, val, cpfmt, fmtlen );
	else ret = -EINVAL;

	up( &info->read );
	up( &info->mutex );

	kfree(cpfmt);
	kfree(mybuf);
	return ( ret < 0 ? ret : count );
}

int altpinfo_ioctl( struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg ) {
	int cp, ret;
	char * buffer;
	buffer = kmalloc(GFP_KERNEL, FMT_CHAR_LIMIT);
	if ( !buffer ) return -ENOMEM;

	print_debug( "execute altpinfo_ioctl().\n" );
	switch ( cmd ) {
	case API_GET_FMT:
		read_lock( &altpinfo_fmt.access );
		cp = __copy_to_user( (char *) arg, altpinfo_fmt.fmt_charp, FMT_CHAR_LIMIT );
		
		read_unlock( &altpinfo_fmt.access );
		if ( cp ) print_debug( "API_GET_FMT - nie skopiowano %d bajtów.\n", cp );
		else print_debug( "API_GET_FMT - %s\n", altpinfo_fmt.fmt_charp );
		break;
	case API_SET_FMT:
		cp = __copy_from_user( buffer, (char *) arg, FMT_CHAR_LIMIT );
		buffer[FMT_CHAR_LIMIT-cp-1] = '\0';
		print_debug( "cp= %d, %d\n", cp, FMT_CHAR_LIMIT-cp );
		if ( cp ) print_debug( "API_SET_FMT - nie skopiowano %d bajtów.\n", cp );
		else print_debug( "API_SET_FMT - %s, %s\n", altpinfo_fmt.fmt_charp, buffer );
		ret = change_fmt( buffer );
		kfree(buffer);
		return ret;
	case API_GET_CURLEN:
		put_user( 
			( (struct altpinfo_t *) filp->private_data )->buflen - 
			( (struct altpinfo_t *) filp->private_data )->bufpos, (int *) arg );
		print_debug( "API_GET_CURLEN\n" );
		break;
	default:
		kfree(buffer);
		return -ENOTTY;
	}
	kfree(buffer);
	return 0;
}

loff_t altpinfo_llseek( struct file *filp, loff_t off, int whence ) {
	print_info( "altpinfo_llseek = Error, ( Brak obs³ugi )\n");
	return -ESPIPE;
}

struct file_operations altpinfo_fops = {
	.owner    = THIS_MODULE,
	.llseek   = altpinfo_llseek,
	.read     = altpinfo_read,
	.write    = altpinfo_write,
	.ioctl    = altpinfo_ioctl,
	.open     = altpinfo_open,
	.release  = altpinfo_release,
};

void altpinfo_cleanup_module( void ) {
	print_debug( "execute altpinfo_cleanup_module().\n" );
	if ( unregister_chrdev( altpinfo_major, "altpinfo" ) != 0 )
		print_warn( "unregister_chrdev = Error, ( major= %d )\n", altpinfo_major );
	else
		print_warn( "unregister_chrdev = Ok,\n" );
}

int altpinfo_init_module( void ) {
	print_debug( "execute altpinfo_init_module().\n" );
	if ( register_chrdev( altpinfo_major, "altpinfo", &altpinfo_fops) < 0 ) {
		print_warn( "register_chrdev = Error, ( major= %d, fops= %lu )\n", altpinfo_major, (unsigned long) &altpinfo_fops );
		return -EIO;
	} else 
		print_warn( "register_chrdev = Ok,\n" );
	change_fmt( fmt );
	return 0;
}

module_init( altpinfo_init_module    );
module_exit( altpinfo_cleanup_module );
