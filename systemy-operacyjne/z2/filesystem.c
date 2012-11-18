/* gcc -Wall `pkg-config fuse --cflags --libs` src.c -o out */

#define FUSE_USE_VERSION 25
#define AUTHOR "Piotr Truszkowski, pt209223"

#include <fuse.h>
#include <fuse_lowlevel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <zlib.h>
#include <pthread.h>
#include "struct.h"

/* katalog z rdiff */ 
static char rdiffpath[PATHLEN]; 
static time_t mounttime = 0;
static char link_current[PATHLEN] = "/aktualna -> ";
static char link_previous[PATHLEN] = "/poprzednia -> ";
static char tmp_template[PATHLEN] = "/tmp/XXXXXX";
static time_t link_curr_atime, link_prev_atime, root_atime;

/* rdiff sesje */
unsigned int sessCounter = 0;
struct SESSION sessions[SESSIONLIMIT];

/*** sortujemy sesje ***/
void sessions_QSort( struct SESSION * s, int K, int N ) {
	int i = K, j = N, x = ( K + N ) / 2;
	do {
		while ( strncmp( s[i].nm, s[x].nm, PATHLEN ) < 0 ) ++i;
		while ( strncmp( s[x].nm, s[j].nm, PATHLEN ) < 0 ) --j;
		if ( i < j ) Swap( SESSION, s[i], s[j] );
		if ( i <= j ) { ++i; --j; }
	} while ( i < j );
	if ( K < j ) sessions_QSort( s, K, j );
	if ( N > i ) sessions_QSort( s, i, N );
}

#define ssort( vbegin, N ) sessions_QSort( (vbegin), 0, (N) - 1 )

/***  sortujemy pliki  ***/
void nodes_QSort( struct node * s, int K, int N ) {
	int i = K, j = N, x = ( K + N ) / 2;
	do {
		while ( strncmp( s[i].nm, s[x].nm, PATHLEN ) < 0 ) ++i;
		while ( strncmp( s[x].nm, s[j].nm, PATHLEN ) < 0 ) --j;
		if ( i < j ) Swap( node, s[i], s[j] );
		if ( i <= j ) { ++i; --j; }
	} while ( i < j );
	if ( K < j ) nodes_QSort( s, K, j );
	if ( N > i ) nodes_QSort( s, i, N );
}

#define nsort( vbegin, N ) nodes_QSort( (vbegin), 0, (N) - 1 )

/*** wyszukujemy pliki ***/
unsigned int findbypath_binary( struct node * vnodes, const char * path, unsigned int N , unsigned int LEN ) {
	unsigned int lt = 0, rt = N, mt;
	int cmp;
	for ( ; lt < rt ; ) {
		mt = ( lt + rt )/2; 
		cmp = strncmp((vnodes + mt)->nm, path, LEN);
		if ( cmp > 0 )
			rt = mt-1;
		else if ( cmp < 0 )
			lt = mt+1;
		else return mt;
	}
	if ( lt == rt && rt != N ) 
		if ( !strncmp((vnodes + rt)->nm, path, LEN) )
			return rt;
	
	return N;/* jak porazka to N */
}

#define findbypath( s, path ) \
	findbypath_binary( (s).nodes, (path), (s).nodeslen, PATHLEN )

#define findbypath_ext( s, path, slen, pathlen ) \
  findbypath_binary( (s).nodes, (path), (slen), (pathlen) )

/***  Otwieranie plikow  ***/

static int restore( const char * path, char * dest ) {
/** rdiff-backup -r date path_1 path_2 **/
	int fd, st;
	unsigned int SID, i, j;
	char buf[PATHLEN];
	if ( ( SID = findsession( path ) ) == -1 ) return -ENOENT;

	i = j = 0;
	strncpy( buf, rdiffpath, PATHLEN );
	while ( buf[++i] ) /* obciecie koncowki /rdiff-backup-data/ **/
		if ( buf[i] == '/' && buf[i+1] ) j = i;
	strncpy( buf + j, hidedate(path), PATHLEN - j ); /* plus lokalizacja pliku **/
	
	strncpy( dest, tmp_template, PATHLEN );

	if ( (fd = mkstemp( dest )) == -1 ) return -errno;

	switch ( fork() ) {
	case -1: return -errno;
	case 0:
		close ( fd ); /* pobieramy wlasciwa wersje rdiffem */
		execlp( "rdiff-backup", "rdiff-backup", "--force", "-r", sessions[SID].date, buf, dest, 0 );
		return -errno;
	default: wait(&st);
	}
	close( fd );
	if ( (fd = open ( dest, O_RDONLY )) == -1 ) { 
		remove( dest ); /* usuwamy plik, bo cos sie popsulo */
		return -errno;
	}

	if ( st ) { /* cos nie tak bylo z execlp */
		close( fd );
		remove( dest );
		return st;
	} else return fd; /* jak wszystko ok, to fd */
}

/***  Dodanie wezla do sktruktury katalogow  ***/

inline int addnode( unsigned int SID, char * path, unsigned int idn ) {
	unsigned int i = 0, j = 0, k;
	char c;

/* szukamy w jakim katalogu znajduje sie plik */
	while ( path[++j] ) if ( path[j] == '/' && path[j+1] ) i = j;
	if ( i ) {
		c = path[i];
		path[i] = 0;
		if ( ( k = findbypath_ext( sessions[SID], path, idn, PATHLEN ) ) == idn ) return -1;
		path[i] = c;
	} else k = 0; 
	
/* a teraz dodajemy plik do listy sasiadow oraz jesli trzeba dodajemy wpis do katalogu nad */
	if ( sessions[SID].nodes[k].in == NULL )
		sessions[SID].nodes[k].in = &sessions[SID].nodes[idn].next;
	else
		list_next( sessions[SID].nodes[k].in->next, &sessions[SID].nodes[idn].next );
	++sessions[SID].nodes[k].st.size; /* w ten sposob katalog ma rozmiar taki ile ma plikow */
	if ( sessions[SID].nodes[idn].st.mode & S_IFDIR )
		++sessions[SID].nodes[k].st.nlink; /* zwiekszamy liczbe linkow, gdy dodalismy katalog */
	return 0;
}

/***  Wczytanie danych o sesji  ***/ 

static unsigned int session( char * vsname ) {
/** session_statistics.YYYY-MM-DDTHH:MM:SS+01:00.data **/
	FILE * fsess;
	char sessp[PATHLEN], buffer[BUFLEN];
	
	sprintf( sessp, "%ssession_statistics.%s.data", rdiffpath, vsname );
	if ( ( fsess = fopen( sessp, "r" ) ) == NULL ) {
		fprintf( stderr, "BLAD: niemozna wczytac statystyk sesji - %s\n", vsname);
		return -1;
	}

/* rozne ustawienia dla sesji... */
	strncpy( sessions[sessCounter].date, vsname, 25 );
	settime( vsname, sessions[sessCounter].nm, &sessions[sessCounter].st.time );
	sessions[sessCounter].st.uid = getuid();
	sessions[sessCounter].st.gid = getgid();
	++sessCounter;

/* na [0] jest korzen */
	while ( !getline( buffer, fsess ) ) { /* odczytujemy ile plikow w sesji, itp. */
		if ( strncmp( buffer, "SourceFiles ", 12 ) == 0 ) {
			sessions[sessCounter - 1].nodeslen = atoi(buffer + 12);
			sessions[sessCounter - 1].nodes = (struct node *) malloc( sessions[sessCounter - 1].nodeslen*sizeof(struct node) );
			sessions[sessCounter - 1].nodes[0].nm[0] = 0;
			sessions[sessCounter - 1].nodes[0].st.mode = 0500 | S_IFDIR;
			sessions[sessCounter - 1].nodes[0].st.nlink = 2;
			sessions[sessCounter - 1].nodes[0].st.size = 0;
			sessions[sessCounter - 1].nodes[0].in = NULL;
			sessions[sessCounter - 1].nodes[0].st.atime = sessions[sessCounter-1].st.time;
			list_init(&sessions[sessCounter - 1].nodes[0].next);
			pthread_mutex_init( &sessions[sessCounter - 1].nodes[0].mutex, NULL );
			fclose( fsess );
			return sessCounter - 1; /* zwracamy nr sesji */
		}
	}
	return -1;
}

/***  Wczytanie metadanych plikow  ***/

static int metadata ( char * metaname ) {
/** mirror_metadata.YYYY-MM-DDTHH:MM:SS+01:00.snapshot.gz **/
	char buffer[BUFLEN], metap[BUFLEN];
	FILE * ftemp;
	gzFile gz;
	int i, id, inode = 0, len;
	
	strncpy( buffer, metaname + 16, 25 );
	buffer[25] = 0;
	if ( (id = session( buffer )) == -1 ) return 1;

	sprintf( metap, "%s%s", rdiffpath, metaname );
	ftemp = tmpfile(); /* sam ginie po close */
	
	if ( (gz = gzopen( metap, "r" )) == NULL ) { /* czytamy ze spakowanego pliku... */
		fprintf( stderr, "BLAD: Niemozna wczytac metadanych z sesji - %s\n", buffer);
		return 1;
	}
	
	while ( (len = gzread( gz, buffer, BUFLEN )) > 0 )
		fwrite( buffer, len, 1, ftemp );
	
	fseek( ftemp, 0, SEEK_SET );
	gzclose(gz);
	
/* najpierw katalog / ( juz zainicjowany w session() ) */
	for ( i = 0; i < 8; ++i ) getline( buffer, ftemp );

/* + resztaplikow */
	for ( inode = 1; ; ++inode ) {
/* File nazwa */
		if ( getline( buffer, ftemp ) ) break;
		sessions[id].nodes[inode].nm[0] = '/';
		strncpy( sessions[id].nodes[inode].nm + 1, buffer + 5, BUFLEN - 5 );
		sessions[id].nodes[inode].count = 0;
		sessions[id].nodes[inode].st.atime = sessions[id].st.time;
/* Type dir */
		if ( getline( buffer, ftemp ) ) break;
		if ( !strncmp( buffer + 5 + 2, "dir", 3 ) ) {
			sessions[id].nodes[inode].st.mode = S_IFDIR | 0500;
			sessions[id].nodes[inode].st.size = 0;
			sessions[id].nodes[inode].st.nlink = 2;
		} else { /* gdy zwykly plik (w zadaniu rozpatrujemy tylko pliki i katalogi) */
			sessions[id].nodes[inode].st.mode = S_IFREG | 0400;
/* Size 123 */
			if ( getline( buffer, ftemp ) ) break;
			sessions[id].nodes[inode].st.size = atoi( buffer + 5 + 2 );
			sessions[id].nodes[inode].st.nlink = 1;
		}
/* ModTime 1162238668 *//* Uid 1000 *//* Uname pt209223 */
/* Gid 100 *//* Gname users *//* Permissions 493 */	
		for ( i = 0; i < 6; ++i ) getline( buffer, ftemp );		
	}

	nsort( sessions[id].nodes + 1, sessions[id].nodeslen - 1 );  /* na wszelki wypadek */

/* dodajemy do struktury katalogow, tutaj bo w addnode sa potrzebne niektore dane o plikach */
	for ( i = 1; i < sessions[id].nodeslen; ++i ) {
		pthread_mutex_init( &sessions[id].nodes[i].mutex, NULL );
		list_init( &sessions[id].nodes[i].next );
		sessions[id].nodes[i].in = NULL;
		if ( addnode( id, sessions[id].nodes[i].nm, i ) ) 
			fprintf( stderr, "BLAD: Brak pliku(?)\n"); /* ktos pewnie w rdiff-ie grzebal */
	}
	
	fclose(ftemp);
	return 0;
}

/***  Inicjalizator  ***/

static int ptfs_init( const char * path ) {
	DIR * rdiffdir;
	struct dirent * ent;
	mounttime = time(0);
	root_atime = mounttime;
	
	sprintf( rdiffpath, "%s/rdiff-backup-data/", path ); /* strlen("katalog=") == 8 */
	if ( ( rdiffdir = opendir( rdiffpath ) ) == NULL ) {
		fprintf( stderr, "BLAD: Brak dostepu do %s!\n", rdiffpath );
		return 1; 
	}

	while ( ( ent = readdir( rdiffdir ) ) != NULL )
		if ( !strncmp( ent->d_name, "mirror_metadata.", 16 ) )
			if ( metadata( ent->d_name ) ) return 1;

/* To sortowanie jest tylko na wszelki wypadek, gdyby sie okazalo, 
 ze sesje zostaly wczytane w nieodpowiedniej kolejnosci */
	ssort( sessions, sessCounter ); 

	if ( sessCounter == 0 ) {
		fprintf( stderr, "BLAD: katalog rdiff-backup-data nie ma zapisnej zadnej sesji!\n" );
		return 1;
	}
	
	strncpy( link_current + strlen(link_current), sessions[sessCounter-1].nm + 1, BUFLEN - strlen(link_current) );
	link_curr_atime = sessions[sessCounter-1].st.time;
	if ( sessCounter > 1 ) {
		strncpy( link_previous + strlen(link_previous), sessions[sessCounter-2].nm + 1, BUFLEN - strlen(link_previous) );
		link_prev_atime = sessions[sessCounter-2].st.time;
	}

	return 0;
}

/***  Finalizator  ***/

static void ptfs_destroy( void * null ) {
	int i, j;

	for ( i = 0; i < sessCounter - 1; ++i )
		for ( j = 0; j < sessions[i].nodeslen; ++j ) {
/* zwalniamy zasoby pthread_mutexow */
			pthread_mutex_trylock( &sessions[i].nodes[j].mutex ); /* destroy wymaga by mutex */
			pthread_mutex_unlock ( &sessions[i].nodes[j].mutex ); /* byl wolny, wiec na wszelki */
			pthread_mutex_destroy( &sessions[i].nodes[j].mutex ); /* wypadek taki maly zabieg. */
/* usuwamy pliki tymczasowe z /tmp/ o ile takie zastawilismy. */	
			if ( sessions[i].nodes[j].count > 0 ) 
				remove( sessions[i].nodes[j].tmpnm );
		}
/* oddajemy pamiec */
	for ( i = 0; i < sessCounter; ++i )
		free( sessions[i].nodes );
		
}

/***  Procedury zwiazane z obsluga systemu plikow  ***/

static int ptfs_getattr(const char *path, struct stat *stbuf) { 
	unsigned int inode, SID;	
	bzero(stbuf, sizeof(struct stat));

	if ( strcmp( path, "/" ) == 0 ) {
		stbuf->st_mode = S_IFDIR | 0500;
		stbuf->st_nlink = sessCounter + 2;
		stbuf->st_mtime = mounttime;
		stbuf->st_atime = root_atime;
		stbuf->st_ctime = mounttime;
		stbuf->st_size = sessCounter + 2;
		stbuf->st_uid = getuid();
		stbuf->st_gid = getgid();
		return 0;
	}

	if ( strncmp( path, link_current, strlen(link_current)-1 ) == 0 ) {
		makestat( stbuf, &sessions[sessCounter-1].nodes[0].st, &sessions[sessCounter-1].st );
		stbuf->st_mode = S_IFLNK | 0500;
		stbuf->st_size = strlen(sessions[sessCounter-1].nm)+1;
		stbuf->st_atime = link_curr_atime;
		return 0;
	}
	
	if ( strncmp( path, link_previous, strlen(link_previous)-1 ) == 0 ) {
		if ( sessCounter < 2 ) return -ENOENT;
		makestat( stbuf, &sessions[sessCounter-2].nodes[0].st, &sessions[sessCounter-2].st );
		stbuf->st_mode = S_IFLNK | 0500;
		stbuf->st_size = strlen(sessions[sessCounter-2].nm)+1;
		stbuf->st_atime = link_prev_atime;
		return 0;
	}
	
	SID = findsession( path );
	if ( SID == -1 ) return -ENOENT;
	inode = findbypath( sessions[SID], hidedate(path) );
	if ( inode == -1 ) return -ENOENT;
	
	makestat( stbuf, &sessions[SID].nodes[inode].st, &sessions[SID].st );
	
	return 0;
}

static int ptfs_readlink(const char *path, char *buf, size_t size) { 
	int ret = -1;
/* w systemie moga byc tylko dwa linki symboliczne */
	if ( strncmp( path, link_current, strlen(link_current) ) == 0 ) {
		ret = strlen( sessions[sessCounter-1].nm ) + 1;
		ret = ( ret < size ? ret : size );
		memcpy( buf+1, sessions[sessCounter-1].nm, ret-1 );
		buf[ret] = 0; buf[0]  = '.';
		link_curr_atime = time(0);
	} else if ( strncmp( path, link_previous, strlen(link_previous) ) == 0 && sessCounter > 1 ) {
		ret = strlen( sessions[sessCounter-2].nm ) + 1;
		ret = ( ret < size ? ret : size );
		memcpy( buf+1, sessions[sessCounter-2].nm, ret-1 );
		buf[ret] = 0; buf[0]  = '.';
		link_prev_atime = time(0);
	} else return -ENOENT;
	return 0;
}

static int ptfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) { 
	struct list * t;
	unsigned int i = 0, SID, inode;

	filler( buf, ".", NULL, 0 );
	filler( buf, "..", NULL, 0 );
	
	if ( strcmp( path, "/" ) == 0 ) { /* gdy czytamy glowny katalog... */
		for ( i = 0; i < sessCounter; ++i ) filler( buf, sessions[i].nm + 1, NULL, 0 );
		filler( buf, link_current + 1, NULL, 0 );
		if ( sessCounter > 1 ) 
			filler( buf, link_previous + 1, NULL, 0 );
/* zmieniamy czas dostepu, gdy czytamy katalog */
		root_atime = time(0);
		return 0;
	}
	
	if ( ( SID = findsession( path ) ) == -1 ) 
		return -ENOENT;
	if ( ( inode = findbypath( sessions[SID], hidedate(path) ) ) == -1 ) 
		return -ENOENT;

	t = sessions[SID].nodes[inode].in;
	if ( t == NULL ) return 0;
	
	do { /* przechodzmy sie po liscie plikow obecnych w katalogu */
		filler( buf, lastname(getnode(t)->nm) , NULL, 0 );
		t = t->next;
	} while ( t != sessions[SID].nodes[inode].in );

	pthread_mutex_lock( &sessions[SID].nodes[inode].mutex );
	sessions[SID].nodes[inode].st.atime = time(0); /* dostep */
	pthread_mutex_unlock( &sessions[SID].nodes[inode].mutex );

	return 0; 
}

static int ptfs_open(const char *path, struct fuse_file_info *fi) { 
	unsigned int SID, inode;
	int fd = -1, i, j, ret = 0;
	if ( (SID = findsession( path )) == -1 ) return -ENOENT;
	if ( (inode = findbypath( sessions[SID], hidedate( path ) ) ) == -1 ) return -ENOENT;
	if ( (fi->flags & 3) != O_RDONLY ) return -EACCES;

	pthread_mutex_lock( &sessions[SID].nodes[inode].mutex );

	sessions[SID].nodes[inode].st.atime = time(0); /* dostep */
	
	if ( sessions[SID].nodes[inode].count == 0 ) { 
/* trzeba otworzyc... */
		if ( SID == sessCounter - 1 ) { /* na szczescie najnowsza wersja, bedzie szybciej */
			strncpy( sessions[SID].nodes[inode].tmpnm, rdiffpath, PATHLEN );
			i = j = 0;
			while ( sessions[SID].nodes[inode].tmpnm[++i] ) /* obciecie koncowki /rdiff-backup-data/ **/
				if ( sessions[SID].nodes[inode].tmpnm[i] == '/' && sessions[SID].nodes[inode].tmpnm[i+1] ) j = i;
			strncpy( sessions[SID].nodes[inode].tmpnm + j, hidedate(path), PATHLEN - j ); /* plus lokalizacja pliku **/
			if ( (fd = open( sessions[SID].nodes[inode].tmpnm, O_RDONLY )) == -1 ) ret = -errno;
			if ( !ret ) sessions[SID].nodes[inode].count = 1;
/* wpp, trzeba przywrocic plik z odpowiedniej wersji, restore() da nam fd */
		} else if ( ( fd = restore( path, sessions[SID].nodes[inode].tmpnm )) < 0 ) 
			ret = fd;
		else
			sessions[SID].nodes[inode].count = 1; /* juz mamy gotowy fd to zwiekszamy licznik */
		
/* Otwieranie otwartego pliku... */
	} else if ( ( fd = open( sessions[SID].nodes[inode].tmpnm, O_RDONLY ) ) == -1 ) 
		ret = -errno; 
			
	else ++sessions[SID].nodes[inode].count; /* juz mamy gotowy fd to zwiekszamy licznik */

	pthread_mutex_unlock( &sessions[SID].nodes[inode].mutex );

	if ( !ret ) fi->fh = fd;
	
	return ret;
}

static int ptfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { 
	lseek( fi->fh, offset, SEEK_SET); /* poprostu przesuwamy sie na pozycje odpowiednia i czytamy */
	return read( fi->fh, buf, size);
}

static int ptfs_release(const char *path, struct fuse_file_info *fi) {
	unsigned int SID, inode, ret = 0;

	if ( (SID = findsession( path )) == -1 ) return -ENOENT;
	if ( (inode = findbypath( sessions[SID], hidedate( path ) ) ) == -1 ) return -ENOENT;
	
	pthread_mutex_lock( &sessions[SID].nodes[inode].mutex );

	close( fi->fh );
/* zamykamy dopiero gdy ostatni bedzie chcial zamknac */
	if ( !( --sessions[SID].nodes[inode].count ) )
		if ( SID < sessCounter - 1 ) 
			if ( remove( sessions[SID].nodes[inode].tmpnm ) ) 
				ret = -errno;
	
	pthread_mutex_unlock( &sessions[SID].nodes[inode].mutex );
	
	return ret;
}

static struct fuse_operations ptfs_oper;

int main(int argc, char * argv[]) {
	int fuse_argc = 2, len;
	char * fuse_argv[2], buffer[PATHLEN], * pwd = NULL, * root = NULL;
	struct stat st;

/* sprawdzenie zgodnosci argumentow... */
	if ( argc != 4 ) {
		fprintf( stderr, 
			"BLAD: Uzywaj- %s -o katalog=/sciezka/do/katalogu/z/kopiami /miejsce/zamontowania\n", 
			argv[0]);
		return 1;
	} 
	if ( strncmp( argv[1], "-o" , 2 ) || strncmp( argv[2], "katalog=", 8 ) ) {
		fprintf( stderr, 
			"BLAD: Uzywaj- %s -o katalog=/sciezka/do/katalogu/z/kopiami /miejsce/zamontowania\n", 
			argv[0]);
		return 2;
	} 
	if ( access( "/dev/fuse", R_OK | W_OK ) ) {
		fprintf( stderr, "BLAD: Brak dostepu do /dev/fuse!\n" );
		return 5;
	} 
	if ( access( argv[2] + 8, R_OK | X_OK ) ) {
		fprintf( stderr, "BLAD: Brak dostepu do katalogu - %s!\n", argv[2] + 8 );
		return 3;
	} 
	if ( argv[2][8] != '/' ) {
		if ( ( pwd = getenv( "PWD" ) ) == NULL ) {
			fprintf( stderr, "BLAD: Program nie moze pobrac ze srodowiska zmiennej $PWD!\n" );
			fprintf( stderr, "    : Podaj pelna sciezke do katalogu - %s!\n", argv[2] + 8 );
			return 6; 
		} /* else, zakladam ze PWD jest juz dobrze ustawione... */
		strncpy( buffer, pwd, PATHLEN );
		len = strlen( buffer );
		buffer[len] = '/'; buffer[len+1] = '\0';
		strncpy( buffer + len + 1, argv[2] + 8, PATHLEN - len - 1 );
		root = buffer;
	} else root = argv[2] + 8;	
	if ( stat( argv[3], &st ) ) {
		fprintf( stderr, "BLAD: Brak dostepu do katalogu - %s!\n", argv[3] );
		return 4;
	}
	if ( ( (st.st_mode & 0700) != 0700 || getuid() != st.st_uid ) 
	  && ( (st.st_mode & 0070) != 0070 || getgid() != st.st_gid || getuid() == st.st_uid )
	  && ( (st.st_mode & 0007) != 0007 || getgid() == st.st_gid || getuid() == st.st_uid ) 
	) {
		fprintf( stderr, "BLAD: Brak dostepu do katalogu - %s!\n", argv[3] );
		return 4;
	}
	 
	fuse_argv[0] = argv[0]; 
	fuse_argv[1] = argv[3];
	ptfs_oper.readdir = ptfs_readdir;
	ptfs_oper.getattr = ptfs_getattr;
	ptfs_oper.readlink = ptfs_readlink;
	ptfs_oper.open = ptfs_open;
	ptfs_oper.read = ptfs_read;
	ptfs_oper.release = ptfs_release;
	ptfs_oper.destroy = ptfs_destroy;
	
/* inicjujemy struktury danych... */
	if ( ptfs_init( root ) ) { 
/* jak cos poszlo nie tak, to zwalniamy to co sie udalo zainicjowac */
		ptfs_destroy(NULL);
		return 7;
	}	
	
/* montujemy! */
	return fuse_main( fuse_argc, fuse_argv, &ptfs_oper );
}
