#include "struct.h"

struct altpinfo_fmt_t altpinfo_fmt = {
	.fmt = "",
	.fmt_charp = "",
	.access = RW_LOCK_UNLOCKED
};

int change_fmt( const char * dscr ) {
/* pid %d, uid %d, comm %s, state %c, vsize %lu, session %d **/	
	int i, count = 1, pos = 0, osize = 0;
	altpinfo_format_t * cpfmt;
	print_debug( "execute: change_format().\n" );
	
	for ( i = 0; dscr[i] != '\0'; ++i )
		if ( dscr[i] == '+' ) ++count;
		
	cpfmt = kmalloc(GFP_KERNEL, FMT_CHAR_LIMIT);
	if ( !cpfmt ) return -1;
	cpfmt[count] = ALTPINFO_FORMAT_NULL;

	i = 0; 
	for ( i = 0; i < count; ++i ) {
		switch ( dscr[pos] ) {
		case 'p': /* pid */
			if ( dscr[pos+1] == 'i' && dscr[pos+2] == 'd' && ( dscr[pos+3] == '+' || dscr[pos+3] == '\0' ) )
				cpfmt[i] = ALTPINFO_FORMAT_PID;
			else return -3;
			pos += 4;
			osize += 10 + 1; /* int + '+' */
			break;
		case 'u': /* uid */
			if ( dscr[pos+1] == 'i' && dscr[pos+2] == 'd' && ( dscr[pos+3] == '+' || dscr[pos+3] == '\0' ) )
				cpfmt[i] = ALTPINFO_FORMAT_UID;
			else return -3;
			pos += 4;
			osize += 10 + 1; /* int + '+' */
			break;
		case 'c': /* comm */ 
		if ( dscr[pos+1] == 'o' && dscr[pos+2] == 'm' && dscr[pos+3] == 'm' && ( dscr[pos+4] == '+' || dscr[pos+4] == '\0' ) )
				cpfmt[i] = ALTPINFO_FORMAT_COMM;
			else return -3;
			pos += 5;
			osize += 16 + 1; /* char[16] + '+' */
			break;
		case 'v': /* vsize */
			if ( dscr[pos+1] == 's' && dscr[pos+2] == 'i' && dscr[pos+3] == 'z' && dscr[pos+4] == 'e' && ( dscr[pos+5] == '+' || dscr[pos+5] == '\0' ) )
				cpfmt[i] = ALTPINFO_FORMAT_VSIZE;
			else return -3;
			pos += 6;
			osize += 10 + 1; /* int + '+' */
			break;
		case 's': /* state, session */
			switch ( dscr[pos+1] ) {
			case 't':
				if ( dscr[pos+2] == 'a' && dscr[pos+3] == 't' && dscr[pos+4] == 'e' && ( dscr[pos+5] == '+' || dscr[pos+5] == '\0' ) )
					cpfmt[i] = ALTPINFO_FORMAT_STATE;
				else return -3;
				pos += 6;
				osize += 1 + 1; /* char + '+' */
				break;
			case 'e':
				if ( dscr[pos+2] == 's' && dscr[pos+3] == 's' && dscr[pos+4] == 'i' && dscr[pos+5] == 'o' && dscr[pos+6] == 'n' 
				&& ( dscr[pos+7] == '+' || dscr[pos+7] == '\0' ) )
					cpfmt[i] = ALTPINFO_FORMAT_SESSION;
				else return -3;
				pos += 8;
				osize += 10 + 1; /* int + '+' */
				break;
			default:
				return -1;
			}
			break;
		default:
			return -1;
		}
	}
/* write_lock: tylko tu piszemy, */
	write_lock( &altpinfo_fmt.access );
	    altpinfo_fmt.outlen = osize;
	    memcpy( altpinfo_fmt.fmt, cpfmt, strlen( cpfmt ) + 1);
	    snprintf( altpinfo_fmt.fmt_charp, FMT_CHAR_LIMIT - 1, dscr );
	write_unlock( &altpinfo_fmt.access );
	kfree(cpfmt);
	return 0;
}

int free_altbuffer( struct altpinfo_t * info ) {
	print_debug( "execute: free_altbuffer().\n" );
	if ( info ) {
		if ( info->buffer ) 
			print_debug ( "bufstat(free): len=%d/%d, pos=%d, buf=%u\n", 
				info->buflen, info->bufreallen, (int)info->bufpos, (unsigned int) info->buffer );
		if ( info->buffer ) 
			kfree( info->buffer );
		info->bufreallen = 0;
		info->buflen = 0;
		info->bufpos = 0;
		info->buffer = 0;
	}
	return 0;
}

int alloc_altbuffer( struct altpinfo_t * info, ssize_t n ) {
	print_debug( "execute: alloc_altbuffer().\n" );
	if ( info ) {
		if ( info->bufreallen < n ) {
			free_altbuffer( info );
			info->buffer = kzalloc( n, GFP_KERNEL );
			if ( !(info->buffer) ) return -1;
			info->bufreallen = n;
		} /* jesli nie ma takiej potrzeby, nie allokujemy, wykorzystujemy
		   * poprzednio przydzielona pamiec, bufreallen zna prawdziwy rozmiar buffer,
		   */
		info->buflen = 0; 
		info->bufpos = 0;
		print_debug ( "bufstat(alloc %d): len=0/%d, pos=%d, buf=%u\n", 
			n, info->bufreallen, (int) info->bufpos, (unsigned int) info->buffer );
	}
	return 0;
}

/*** prawie to co w: fs/proc/array.c#141: ****/
static inline char get_task_state(struct task_struct *tsk) {
	unsigned int state = (tsk->state & 
		(TASK_RUNNING | TASK_INTERRUPTIBLE | TASK_UNINTERRUPTIBLE | TASK_STOPPED | TASK_TRACED)
		) | (tsk->exit_state & (EXIT_ZOMBIE | EXIT_DEAD));
	const char *p = "RSDTTZX";
	while ( state ) {
		p++;
		state >>= 1;
	}
	return *p;
}

int get_task_altpinfo( struct task_struct * tp, struct task_altpinfo * talt ) {
	struct mm_struct * mm;
	print_debug( "execute get_task_altpinfo().\n" );
/* pid %d, uid %d, comm %s, state %c, vsize %lu, session %d */
	read_lock( &tasklist_lock );
	talt->pid = tp->pid;
	talt->uid = tp->uid;
	snprintf( talt->comm, TASK_COMM_LEN, "%s", tp->comm );
	mm = get_task_mm( tp );
	talt->vsize = ( mm ? PAGE_SIZE * mm->total_vm : 0 );
	talt->state = get_task_state(tp);
	talt->sid = tp->signal->session;
	read_unlock( &tasklist_lock );
	talt->comm[TASK_COMM_LEN] = '\0';
	return 0;		
}
