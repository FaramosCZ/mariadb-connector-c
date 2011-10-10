/* Copyright (C) 2000 MySQL AB & MySQL Finland AB & TCX DataKonsult AB
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA */

/* defines for the libmysql library */

#ifndef _mysql_h
#define _mysql_h

#ifdef __CYGWIN__     /* CYGWIN implements a UNIX API */
#undef WIN
#undef _WIN
#undef _WIN32
#undef _WIN64
#undef __WIN__
#endif

#ifndef MYSQL_SERVER
#ifdef	__cplusplus
extern "C" {
#endif
#endif

  
#ifndef _global_h				/* If not standard header */
#include <sys/types.h>
#ifdef __LCC__
#include <winsock.h>				/* For windows */
#endif
typedef char my_bool;
#if (defined(_WIN32) || defined(_WIN64)) && !defined(__WIN__)
#define __WIN__
#endif
#if !defined(__WIN__)
#define STDCALL
#else
#define STDCALL __stdcall
#endif
typedef char * gptr;

#ifndef ST_USED_MEM_DEFINED
#define ST_USED_MEM_DEFINED
typedef struct st_used_mem {			/* struct for once_alloc */
  struct st_used_mem *next;			/* Next block in use */
  unsigned int left;				/* memory left in block  */
  unsigned int size;				/* size of block */
} USED_MEM;

typedef struct st_mem_root {
  USED_MEM *free;
  USED_MEM *used;
  USED_MEM *pre_alloc;
  size_t min_malloc;
  size_t block_size;
  unsigned int block_num;
  unsigned int first_block_usage;
  void (*error_handler)(void);
} MEM_ROOT;
#endif

#ifndef my_socket_defined
#ifdef __WIN__
#define my_socket SOCKET
#else
typedef int my_socket;
#endif
#endif
#endif
#include "mysql_com.h"
#include "mysql_version.h"
#include "my_list.h"


extern unsigned int mysql_port;
extern char *mysql_unix_port;

#define IS_PRI_KEY(n)	((n) & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	((n) & NOT_NULL_FLAG)
#define IS_BLOB(n)	((n) & BLOB_FLAG)
#define IS_NUM(t)	((t) <= FIELD_TYPE_INT24 || (t) == FIELD_TYPE_YEAR)
#define IS_NUM_FIELD(f)	 ((f)->flags & NUM_FLAG)
#define INTERNAL_NUM_FIELD(f) (((f)->type <= MYSQL_TYPE_INT24 && ((f)->type != MYSQL_TYPE_TIMESTAMP || (f)->length == 14 || (f)->length == 8)) || (f)->type == MYSQL_TYPE_YEAR)

typedef struct st_mysql_field {
  char *name;			/* Name of column */
  char *org_name;		/* Name of original column (added after 3.23.58) */
  char *table;			/* Table of column if column was a field */
  char *org_table;		/* Name of original table (added after 3.23.58 */
  char *db;                     /* table schema (added after 3.23.58) */
  char *catalog;                /* table catalog (added after 3.23.58) */
  char *def;			/* Default value (set by mysql_list_fields) */
  unsigned long length;		/* Width of column */
  unsigned long max_length;	/* Max width of selected set */
/* added after 3.23.58 */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
/***********************/
  unsigned int flags;		/* Div flags */
  unsigned int decimals;	/* Number of decimals in field */
  unsigned int charsetnr;       /* char set number (added in 4.1) */
  enum enum_field_types type;	/* Type of field. Se mysql_com.h for types */
  void *extension;              /* added in 4.1 */
} MYSQL_FIELD;

typedef char **MYSQL_ROW;		/* return data as array of strings */
typedef unsigned int MYSQL_FIELD_OFFSET; /* offset to current field */

#if defined(NO_CLIENT_LONG_LONG)
typedef unsigned long my_ulonglong;
#elif defined (__WIN__)
typedef unsigned __int64 my_ulonglong;
#else
typedef unsigned long long my_ulonglong;
#endif

#define SET_CLIENT_ERROR(a, b, c, d) \
{ \
  (a)->net.last_errno= (b);\
  strncpy((a)->net.sqlstate, (c), sizeof((a)->net.sqlstate));\
  strncpy((a)->net.last_error, (d) ? (d) : ER((b)), sizeof((a)->net.last_error));\
}

#define CLEAR_CLIENT_ERROR(a) \
{ \
  (a)->net.last_errno= 0;\
  strcpy((a)->net.sqlstate, "00000");\
  (a)->net.last_error[0]= '\0';\
}

#define MYSQL_COUNT_ERROR (~(my_ulonglong) 0)


typedef struct st_mysql_rows {
  struct st_mysql_rows *next;		/* list of rows */
  MYSQL_ROW data;
  unsigned long length;
} MYSQL_ROWS;

typedef MYSQL_ROWS *MYSQL_ROW_OFFSET;	/* offset to current row */

typedef struct st_mysql_data {
  my_ulonglong rows;
  unsigned int fields;
  MYSQL_ROWS *data;
  MEM_ROOT alloc;
} MYSQL_DATA;

enum mysql_option 
{
  MYSQL_OPT_CONNECT_TIMEOUT, 
  MYSQL_OPT_COMPRESS,
  MYSQL_OPT_NAMED_PIPE,
  MYSQL_INIT_COMMAND,
  MYSQL_READ_DEFAULT_FILE,
  MYSQL_READ_DEFAULT_GROUP,
  MYSQL_SET_CHARSET_DIR,
  MYSQL_SET_CHARSET_NAME,
  MYSQL_OPT_LOCAL_INFILE,
  MYSQL_OPT_PROTOCOL,
  MYSQL_SHARED_MEMORY_BASE_NAME,
  MYSQL_OPT_READ_TIMEOUT,
  MYSQL_OPT_WRITE_TIMEOUT,
  MYSQL_OPT_USE_RESULT,
  MYSQL_OPT_USE_REMOTE_CONNECTION,
  MYSQL_OPT_USE_EMBEDDED_CONNECTION,
  MYSQL_OPT_GUESS_CONNECTION,
  MYSQL_SET_CLIENT_IP,
  MYSQL_SECURE_AUTH,
  MYSQL_REPORT_DATA_TRUNCATION,
  MYSQL_OPT_RECONNECT,
  MYSQL_OPT_SSL_VERIFY_SERVER_CERT,
  MYSQL_OPT_PLUGIN_DIR,
  MYSQL_OPT_DEFAULT_AUTH,
  MYSQL_OPT_PROGRESS_CALLBACK
};

enum mysql_status { MYSQL_STATUS_READY,
                    MYSQL_STATUS_GET_RESULT,
		    MYSQL_STATUS_USE_RESULT,
                    MYSQL_STATUS_QUERY_SENT,
                    MYSQL_STATUS_SENDING_LOAD_DATA,
                    MYSQL_STATUS_FETCHING_DATA,
                    MYSQL_STATUS_NEXT_RESULT_PENDING,
                    MYSQL_STATUS_QUIT_SENT, /* object is "destroyed" at this stage */
};

struct st_mysql_options {
  unsigned int connect_timeout, read_timeout, write_timeout;
  unsigned int port, protocol;
  unsigned long client_flag;
  char *host,*user,*password,*unix_socket,*db;
  struct st_dynamic_array *init_commands;
  char *my_cnf_file,*my_cnf_group, *charset_dir, *charset_name;
  char *ssl_key;				/* PEM key file */
  char *ssl_cert;				/* PEM cert file */
  char *ssl_ca;					/* PEM CA file */
  char *ssl_capath;				/* PEM directory of CA-s? */
  char *ssl_cipher;
  char *shared_memory_base_name;
  unsigned long max_allowed_packet;
  my_bool use_ssl;				/* if to use SSL or not */
  my_bool compress,named_pipe;
  my_bool unused_1, unused_2, unused_3, unused_4;
  enum mysql_option methods_to_use;
  char *client_ip;
  my_bool secure_auth;
  
  /* function pointers for local infile support */
  int (*local_infile_init)(void **, const char *, void *);
  int (*local_infile_read)(void *, char *, unsigned int);
  void (*local_infile_end)(void *);
  int (*local_infile_error)(void *, char *, unsigned int);
  void *local_infile_userdata;
  struct st_mysql_options_extension *extension;
};

typedef struct st_mysql {
  NET		net;			/* Communication parameters */
  unsigned char *connector_fd;		/* ConnectorFd for SSL */
  char		*host,*user,*passwd,*unix_socket,*server_version,*host_info;
  char		*info,*db;
  struct charset_info_st *charset;      /* character set */
  MYSQL_FIELD	*fields;
  MEM_ROOT	field_alloc;
  my_ulonglong affected_rows;
  my_ulonglong insert_id;		/* id if insert on table with NEXTNR */
  my_ulonglong extra_info;		/* Used by mysqlshow */
  unsigned long thread_id;		/* Id for connection in server */
  unsigned long packet_length;
  unsigned int	port;
  unsigned long client_flag,server_capabilities; /* changed from int to long in 4.1 protocol */
  unsigned int	protocol_version;
  unsigned int	field_count;
  unsigned int 	server_status;
  unsigned int  server_language;
  unsigned int  warning_count;          /* warning count, added in 4.1 protocol */
  struct st_mysql_options options;
  enum mysql_status status;
  my_bool	free_me;		/* If free in mysql_close */
  my_bool	reconnect;		/* set to 1 if automatic reconnect */
  char	        scramble_buff[20+ 1];
  /* madded after 3.23.58 */
  my_bool       unused_1;
  void          *unused_2, *unused_3, *unused_4, *unused_5;
  LIST          *stmts;
  const struct  st_mysql_methods *methods;
  void          *thd;
  my_bool       *unbuffered_fetch_owner;
  char          *info_buffer;
  void          *extension;
} MYSQL;


typedef struct st_mysql_res {
  my_ulonglong  row_count;
  unsigned int	field_count, current_field;
  MYSQL_FIELD	*fields;
  MYSQL_DATA	*data;
  MYSQL_ROWS	*data_cursor;
  MEM_ROOT	field_alloc;
  MYSQL_ROW	row;			/* If unbuffered read */
  MYSQL_ROW	current_row;		/* buffer to current row */
  unsigned long *lengths;		/* column lengths of current row */
  MYSQL		*handle;		/* for unbuffered reads */
  my_bool	eof;			/* Used my mysql_fetch_row */
  my_bool       is_ps;
} MYSQL_RES;

enum enum_mysql_timestamp_type
{
  MYSQL_TIMESTAMP_NONE= -2, MYSQL_TIMESTAMP_ERROR= -1,
  MYSQL_TIMESTAMP_DATE= 0, MYSQL_TIMESTAMP_DATETIME= 1, MYSQL_TIMESTAMP_TIME= 2
};

typedef struct st_mysql_time
{
  unsigned int  year, month, day, hour, minute, second;
  unsigned long second_part;
  my_bool       neg;
  enum enum_mysql_timestamp_type time_type;
} MYSQL_TIME;

typedef struct character_set
{
  unsigned int      number;     /* character set number              */
  unsigned int      state;      /* character set state               */
  const char        *csname;    /* collation name                    */
  const char        *name;      /* character set name                */
  const char        *comment;   /* comment                           */
  const char        *dir;       /* character set directory           */
  unsigned int      mbminlen;   /* min. length for multibyte strings */
  unsigned int      mbmaxlen;   /* max. length for multibyte strings */
} MY_CHARSET_INFO;


/* Functions to get information from the MYSQL and MYSQL_RES structures */
/* Should definitely be used if one uses shared libraries */

my_ulonglong STDCALL mysql_num_rows(MYSQL_RES *res);
unsigned int STDCALL mysql_num_fields(MYSQL_RES *res);
my_bool STDCALL mysql_eof(MYSQL_RES *res);
MYSQL_FIELD *STDCALL mysql_fetch_field_direct(MYSQL_RES *res,
					      unsigned int fieldnr);
MYSQL_FIELD * STDCALL mysql_fetch_fields(MYSQL_RES *res);
MYSQL_ROWS * STDCALL mysql_row_tell(MYSQL_RES *res);
unsigned int STDCALL mysql_field_tell(MYSQL_RES *res);

unsigned int STDCALL mysql_field_count(MYSQL *mysql);
my_bool STDCALL mysql_more_results(MYSQL *mysql);
int STDCALL mysql_next_result(MYSQL *mysql);
my_ulonglong STDCALL mysql_affected_rows(MYSQL *mysql);
my_bool STDCALL mysql_autocommit(MYSQL *mysql, my_bool mode);
my_bool STDCALL mysql_commit(MYSQL *mysql);
my_bool STDCALL mysql_rollback(MYSQL *mysql);
my_ulonglong STDCALL mysql_insert_id(MYSQL *mysql);
unsigned int STDCALL mysql_errno(MYSQL *mysql);
char * STDCALL mysql_error(MYSQL *mysql);
char * STDCALL mysql_info(MYSQL *mysql);
unsigned long STDCALL mysql_thread_id(MYSQL *mysql);
const char * STDCALL mysql_character_set_name(MYSQL *mysql);
void STDCALL mysql_get_character_set_info(MYSQL *mysql, MY_CHARSET_INFO *cs);
int STDCALL mysql_set_character_set(MYSQL *mysql, const char *csname);

MYSQL *		STDCALL mysql_init(MYSQL *mysql);
#ifdef HAVE_OPENSSL
int		STDCALL mysql_ssl_set(MYSQL *mysql, const char *key,
				      const char *cert, const char *ca,
				      const char *capath);
char *		STDCALL mysql_ssl_cipher(MYSQL *mysql);
int		STDCALL mysql_ssl_clear(MYSQL *mysql);
#endif /* HAVE_OPENSSL */
MYSQL *		STDCALL mysql_connect(MYSQL *mysql, const char *host,
				      const char *user, const char *passwd);
my_bool		STDCALL mysql_change_user(MYSQL *mysql, const char *user, 
					  const char *passwd, const char *db);
MYSQL *		STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag);
void		STDCALL mysql_close(MYSQL *sock);
int		STDCALL mysql_select_db(MYSQL *mysql, const char *db);
int		STDCALL mysql_query(MYSQL *mysql, const char *q);
int		STDCALL mysql_send_query(MYSQL *mysql, const char *q,
					 unsigned int length);
int		STDCALL mysql_read_query_result(MYSQL *mysql);
int		STDCALL mysql_real_query(MYSQL *mysql, const char *q,
					unsigned int length);
int		STDCALL mysql_create_db(MYSQL *mysql, const char *DB);
int		STDCALL mysql_drop_db(MYSQL *mysql, const char *DB);
int		STDCALL mysql_shutdown(MYSQL *mysql);
int		STDCALL mysql_dump_debug_info(MYSQL *mysql);
int		STDCALL mysql_refresh(MYSQL *mysql,
				     unsigned int refresh_options);
int		STDCALL mysql_kill(MYSQL *mysql,unsigned long pid);
int		STDCALL mysql_ping(MYSQL *mysql);
char *		STDCALL mysql_stat(MYSQL *mysql);
char *		STDCALL mysql_get_server_info(MYSQL *mysql);
unsigned long   STDCALL mysql_get_server_version(MYSQL *mysql);
char *		STDCALL mysql_get_client_info(void);
char *		STDCALL mysql_get_host_info(MYSQL *mysql);
unsigned int	STDCALL mysql_get_proto_info(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_list_dbs(MYSQL *mysql,const char *wild);
MYSQL_RES *	STDCALL mysql_list_tables(MYSQL *mysql,const char *wild);
MYSQL_RES *	STDCALL mysql_list_fields(MYSQL *mysql, const char *table,
					 const char *wild);
MYSQL_RES *	STDCALL mysql_list_processes(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_store_result(MYSQL *mysql);
MYSQL_RES *	STDCALL mysql_use_result(MYSQL *mysql);
int		STDCALL mysql_options(MYSQL *mysql,enum mysql_option option,
				      const char *arg);
void		STDCALL mysql_free_result(MYSQL_RES *result);
void		STDCALL mysql_data_seek(MYSQL_RES *result,
					my_ulonglong offset);
MYSQL_ROW_OFFSET STDCALL mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET);
MYSQL_FIELD_OFFSET STDCALL mysql_field_seek(MYSQL_RES *result,
					   MYSQL_FIELD_OFFSET offset);
MYSQL_ROW	STDCALL mysql_fetch_row(MYSQL_RES *result);
unsigned long * STDCALL mysql_fetch_lengths(MYSQL_RES *result);
MYSQL_FIELD *	STDCALL mysql_fetch_field(MYSQL_RES *result);
unsigned long	STDCALL mysql_escape_string(char *to,const char *from,
					    unsigned long from_length);
unsigned long STDCALL mysql_real_escape_string(MYSQL *mysql,
					       char *to,const char *from,
					       unsigned long length);
void		STDCALL mysql_debug(const char *debug);
char *		STDCALL mysql_odbc_escape_string(MYSQL *mysql,
						 char *to,
						 unsigned long to_length,
						 const char *from,
						 unsigned long from_length,
						 void *param,
						 char *
						 (*extend_buffer)
						 (void *, char *to,
						  unsigned long *length));
void 		STDCALL myodbc_remove_escape(MYSQL *mysql,char *name);
unsigned int	STDCALL mysql_thread_safe(void);
unsigned int STDCALL mysql_warning_count(MYSQL *mysql);
const char * STDCALL mysql_sqlstate(MYSQL *mysql);
int STDCALL mysql_server_init(int argc __attribute__((unused)),
			      char **argv __attribute__((unused)),
			      char **groups __attribute__((unused)));
void STDCALL mysql_server_end(void);
void STDCALL mysql_thread_end(void);
my_bool STDCALL mysql_thread_init(void);

#include <mysql_stmt.h>
  
#define mysql_reload(mysql) mysql_refresh((mysql),REFRESH_GRANT)

/* new api functions */

#define HAVE_MYSQL_REAL_CONNECT

#ifndef MYSQL_SERVER  
#ifdef	__cplusplus
}
#endif
#endif

#endif