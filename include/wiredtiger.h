/* Copyright (c) 2010 WiredTiger, Inc.  All rights reserved. */

/* vim: set filetype=c.doxygen : */

#ifndef _WIREDTIGER_H_
#define _WIREDTIGER_H_

/*! @defgroup wt WiredTiger API
 * The commonly-used functions, classes and methods that applications use to
 * access and manage data with WiredTiger.
 *
 * @{
 */

#include <sys/types.h>
#include <inttypes.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

struct WT_CONNECTION;	  typedef struct WT_CONNECTION WT_CONNECTION;
struct WT_COLLATOR;	  typedef struct WT_COLLATOR WT_COLLATOR;
struct WT_CURSOR;	  typedef struct WT_CURSOR WT_CURSOR;
struct WT_CURSOR_FACTORY; typedef struct WT_CURSOR_FACTORY WT_CURSOR_FACTORY;
struct WT_ERROR_HANDLER;  typedef struct WT_ERROR_HANDLER WT_ERROR_HANDLER;
struct WT_EXTRACTOR;	  typedef struct WT_EXTRACTOR WT_EXTRACTOR;
struct WT_ITEM;		  typedef struct WT_ITEM WT_ITEM;
struct WT_SESSION;	  typedef struct WT_SESSION WT_SESSION;

#ifdef DOXYGEN
#define __F(func) func
#else
#define __F(func) (*func)
#endif

/*! Type of record numbers. */
typedef uint64_t wiredtiger_recno_t;

/*!
 * A raw item of data to be managed.  Data items have a pointer to the data and
 * a length (limited to 4GB for items stored in tables).  Records consist of a
 * pair of items: a key and an associated value.
 */
struct WT_ITEM {
	/*!
	 * The memory reference of the data item.
	 *
	 * For items returned by a WT_CURSOR, the pointer is only valid until
	 * the next operation on that cursor.  Applications that need to keep
	 * an item across multiple cursor operations must make a copy.
	 */
	const void *data;

	/*!
	 * The number of bytes in the data item.
	 */
	uint32_t size;
};

/*!
 * The WT_CURSOR class is the interface to a cursor.
 *
 * Cursors allow data to be searched, stepped through and updated: the
 * so-called CRUD operations (create, read, update and delete).  Data is
 * represented by WT_ITEM pairs.
 *
 * WT_CURSOR represents a cursor over a collection of data.  Cursors are opened
 * in the context of a session (which may have an associated transaction), and
 * can query and update records.  In the common case, a cursor is used to
 * access records in a table.  However, cursors can be used on subsets of
 * tables (such as a single column or a projection of multiple columns), as an
 * interface to statistics, configuration data or application-specific data
 * sources.  See WT_SESSION::open_cursor for more information.
 *
 * <b>Thread safety:</b> A WT_CURSOR handle cannot be shared between threads:
 * it may only be used within the same thread as the encapsulating WT_SESSION.
 */
struct WT_CURSOR {
	WT_SESSION *session;	/*!< The session handle for this cursor. */
	
	/*!
	 * The format of the data packed into key items.  See
	 * ::wiredtiger_struct_pack for details.  If not set, a default value
	 * of "u" is assumed, and applications use WT_ITEM to manipulate raw
	 * byte arrays.
	 */
	const char *key_format;

	/*!
	 * The format of the data packed into value items.  See
	 * ::wiredtiger_struct_pack for details.  If not set, a default value
	 * of "u" is assumed, and applications use WT_ITEM to manipulate raw
	 * byte arrays.
	 */
	const char *value_format;

	/*! @name Data access
	 * @{
	 */
	/*! Get the key for the current record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline get_key
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(get_key)(WT_CURSOR *cursor, ...);

	/*! Get the value for the current record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline get_value
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(get_value)(WT_CURSOR *cursor, ...);

	/*! Set the key for the next operation.
	 *
	 * @dontinclude ex_all.c
	 * @skip key =
	 * @until set_key
	 *
	 * @param cursor the cursor handle
	 * 
	 * If an error occurs during this operation, a flag will be set in the
	 * cursor, and the next operation to access the key will fail.  This
	 * simplifies error handling in applications.
	 */
	void __F(set_key)(WT_CURSOR *cursor, ...);

	/*! Set the data for the next operation.
	 *
	 * @dontinclude ex_all.c
	 * @skip value =
	 * @until set_value
	 *
	 * @param cursor the cursor handle
	 * 
	 * If an error occurs during this operation, a flag will be set in the
	 * cursor, and the next operation to access the value will fail.  This
	 * simplifies error handling in applications.
	 */
	void __F(set_value)(WT_CURSOR *cursor, ...);
	/*! @} */


	/*! @name Cursor positioning
	 * @{
	 */
	/*! Move to the first record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->first
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(first)(WT_CURSOR *cursor);

	/*! Move to the last record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->last
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(last)(WT_CURSOR *cursor);

	/*! Move to the next record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->next
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(next)(WT_CURSOR *cursor);

	/*! Move to the previous record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->prev
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(prev)(WT_CURSOR *cursor);

	/*! Search for a record.
	 *
	 * @dontinclude ex_all.c
	 * @skip ->prev
	 * @skip ->set_key
	 * @until ->search
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(search)(WT_CURSOR *cursor);

	/*! Search for a record.
	 *
	 * @dontinclude ex_all.c
	 * @skip ->search
	 * @skip ->set_key
	 * @until }
	 *
	 * @param cursor the cursor handle
	 * @param exactp the status of the search: 0 if an exact match is
	 * found, -1 if a smaller key is found, +1 if a larger key is found
	 * @errors
	 */
	int __F(search_near)(WT_CURSOR *cursor, int *exactp);
	/*! @} */


	/*! @name Data modification
	 * @{
	 */
	/*! Insert a record.
	 *
	 * @dontinclude ex_all.c
	 * @skip ->search_near
	 * @skip ->set_key
	 * @until ->insert
	 *
	 * @todo describe append
	 * @todo describe how to unconditionally overwrite
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(insert)(WT_CURSOR *cursor);

	/*! Update the current record.  The cursor must be positioned on a
	 * record and the value of the record will be updated.  If the record
	 * is part of a sorted duplicate set, its position must not change as
	 * a result of the update.
	 *
	 * @dontinclude ex_all.c
	 * @skip ->insert
	 * @skip ->set_value
	 * @until ->update
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(update)(WT_CURSOR *cursor);

	/*! Delete the current record.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->del
	 *
	 * @param cursor the cursor handle
	 * @errors
	 */
	int __F(del)(WT_CURSOR *cursor);
	/*! @} */

	/*! Close the cursor.
	 *
	 * @dontinclude ex_all.c
	 * @skipline cursor->close
	 *
	 * @param cursor the cursor handle
	 * @configempty
	 * @errors
	 */
	int __F(close)(WT_CURSOR *cursor, const char *config);
};

/*!
 * All data operations are performed in the context of a WT_SESSION.  This
 * encapsulates the thread and transactional context of the operation.
 *
 * <b>Thread safety:</b> A WT_SESSION handle cannot be shared between threads:
 * it may only be used within a single thread.  Each thread accessing a
 * database should open a separate WT_SESSION handle.
 */
struct WT_SESSION {
	/*! The connection for this session. */
	WT_CONNECTION *connection;

	/*! Close the session.
	 *
	 * @dontinclude ex_all.c
	 * @skipline session->close
	 *
	 * @param session the session handle
	 * @configempty
	 * @errors
	 */
	int __F(close)(WT_SESSION *session, const char *config);

	/*! @name Cursor handles
	 * @{
	 */

	/*! Open a cursor.
	 *
	 * Cursors may be opened on ordinary tables.  A cache of recently-used
	 * tables will be maintained in the WT_SESSION to make this fast.
	 *
	 * However, cursors can be opened on any data source, regardless of
	 * whether it is ultimately stored in a table.  Some cursor types may
	 * have limited functionality (e.g., be read-only, or not support
	 * transactional updates).
	 *
	 * These are some of the common builtin cursor types:
	 *   <table>
	 *   <tr><th>URI</th><th>Function</th></tr>
	 *   <tr><td><tt>table:[\<tablename\>]</tt></td><td>ordinary table cursor</td></tr>
	 *   <tr><td><tt>column:[\<tablename\>.\<columnname\>]</tt></td><td>column cursor</td></tr>
	 *   <tr><td><tt>config:[table:\<tablename\>]</tt></td><td>database or table configuration</td></tr>
	 *   <tr><td><tt>join:\<cursor1\>\&\<cursor2\>[&\<cursor3\>...]</tt></td><td>Join the contents of multiple cursors together.</td></tr>
	 *   <tr><td><tt>statistics:[table:\<tablename\>]</tt></td><td>database or table statistics (key=(string)keyname, data=(int64_t)value)</td></tr>
	 *   </table>
	 *
	 * See @ref cursor_types for more information.
	 *
	 * @dontinclude ex_all.c
	 * @skip cursor;
	 * @until ->open_cursor
	 *
	 * @param session the session handle
	 * @param uri the data source on which the cursor operates
	 * @param to_dup a cursor to duplicate
	 * @param session the session handle
	 * @configstart
	 * @config{dup,,duplicate handling\, one of "all" or "first" or "last";
	 * 	default "all"}
	 * @config{isolation,,the isolation level for this cursor\, one of
	 * 	"snapshot" or "read-committed" or "read-uncommitted"; default
	 * 	"read-committed".  Ignored for transactional cursors}
	 * @config{overwrite,,if an existing key is inserted\,
	 * 	overwrite the existing value}
	 * @config{raw,,ignore the encodings for the key and
	 * 	value\, manage data as if the formats were \c "u"}
	 * @configend
	 * @param cursorp a pointer to the newly opened cursor
	 * @errors
	 */
	int __F(open_cursor)(WT_SESSION *session,
	    const char *uri, WT_CURSOR *to_dup,
	    const char *config, WT_CURSOR **cursorp);
	/*! @} */

	/*! @name Table operations
	 * @{
	 */
	/*! Create a table.
	 *
	 * @todo Allow both sets of syntax for describing schemas.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->create_table
	 *
	 * @param session the session handle
	 * @param name the name of the table
	 * @configstart
	 * @config{columns,,List of the column names.
	 * 	Comma-separated list of the form <code>(column[\,...])</code>.
	 * 	The number of entries must match the total number of values in
	 * 	#key_format and #value_format.}
	 * @config{column_set,,Named set of columns to store together.
	 * 	Name and comma-separated list of the form
	 * 	<code>name(column[\,...])</code>.  Multiple column sets can
	 * 	be specified by repeating the \c "column_set" key in the
	 * 	configuration string.  Each column set is stored separately\,
	 * 	keyed by the primary key of the table.  Any column that does
	 * 	not appear in a column set is stored in an unnamed default
	 * 	column set for the table.}
	 * @config{exclusive,,Fail if the table exists (if "no"\, the
	 * 	default\, verifies that the table exists and has the specified
	 * 	schema.}
	 * @config{index,,Named index on a set of columns. Name and
	 * 	comma-separated list of the form
	 * 	<code>name(column[\,...])</code>.  Multiple indices can be
	 * 	specified by repeating the \c "index" key in the configuration
	 * 	string.}
	 * @config{key_format,,The format of the data packed into key items.
	 * 	See ::wiredtiger_struct_pack for details.  If not set\, a
	 * 	default value of \c "u" is assumed\, and applications use the
	 * 	WT_ITEM struct to manipulate raw byte arrays.}
	 * @config{value_format,,The format of the data packed into value
	 * 	items.  See ::wiredtiger_struct_pack for details.  If not set\,
	 * 	a default value of \c "u" is assumed\, and applications use the
	 * 	WT_ITEM struct to manipulate raw byte arrays.}
	 * @configend
	 * @errors
	 */
	int __F(create_table)(WT_SESSION *session, const char *name,
	    const char *config);

	/*! Rename a table.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->rename_table
	 *
	 * @param session the session handle
	 * @param oldname the current name of the table
	 * @param newname the new name of the table
	 * @configempty
	 * @errors
	 */
	int __F(rename_table)(WT_SESSION *session,
	    const char *oldname, const char *newname, const char *config);

	/*! Drop (delete) a table.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->drop_table
	 *
	 * @param session the session handle
	 * @param name the name of the table
	 * @configempty
	 * @errors
	 */
	int __F(drop_table)(WT_SESSION *session,
	     const char *name, const char *config);

	/*! Truncate a table.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->truncate_table
	 *
	 * @param session the session handle
	 * @param name the name of the table
	 * @param start optional cursor marking the start of the truncate
	 * operation.  If <code>NULL</code>, the truncate starts from the
	 * beginning of the table
	 * @param end optional cursor marking the end of the truncate
	 * operation.  If <code>NULL</code>, the truncate continues to the end
	 * of the table
	 * @param name the name of the table
	 * @configempty
	 * @errors
	 */
	int __F(truncate_table)(WT_SESSION *session, const char *name,
	    WT_CURSOR *start, WT_CURSOR *end, const char *config);

	/*! Verify a table.
	 *
	 * @todo describe salvage
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->verify_table
	 *
	 * @param session the session handle
	 * @param name the name of the table
	 * @configempty
	 * @errors
	 */
	int __F(verify_table)(WT_SESSION *session, const char *name,
	    const char *config);
	/*! @} */

	/*! @name Transactions
	 * @{
	 */
	/*! Start a transaction in this session.
	 *
	 * All cursors opened in this session that support transactional
	 * semantics will operate in the context of the transaction.  The
	 * transaction remains active until ended with
	 * WT_SESSION::commit_transaction or WT_SESSION::rollback_transaction.
	 *
	 * Ignored if a transaction is in progress.
	 *
	 * @todo describe nested transactions / savepoints
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->begin_transaction
	 *
	 * @param session the session handle
	 * @configstart
	 * @config{isolation,,the isolation level for this transaction\, one of
	 * 	"serializable"\, "snapshot"\, "read-committed" or
	 * 	"read-uncommitted"; default "serializable"}
	 * @config{name,,name of the transaction for tracing and debugging}
	 * @config{sync,,how to sync log records when the transaction commits\,
	 * 	one of "full"\, "flush"\, "write" or "none"; default "full"}
	 * @config{priority,,priority of the transaction for resolving
	 * 	conflicts\, an integer between -100 and 100; default 0}
	 * @configend
	 * @errors
	 */
	int __F(begin_transaction)(WT_SESSION *session, const char *config);

	/*! Commit the current transaction.
	 *
	 * Any cursors opened during the transaction will be closed before
	 * the commit is processed.
	 *
	 * Ignored if no transaction is in progress.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->commit_transaction
	 *
	 * @param session the session handle
	 * @configempty
	 * @errors
	 */
	int __F(commit_transaction)(WT_SESSION *session, const char *config);

	/*! Roll back the current transaction.
	 *
	 * Any cursors opened during the transaction will be closed before
	 * the rollback is processed.
	 *
	 * Ignored if no transaction is in progress.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->rollback_transaction
	 *
	 * @param session the session handle
	 * @configempty
	 * @errors
	 */
	int __F(rollback_transaction)(WT_SESSION *session, const char *config);

	/*! Flush the cache and/or the log and optionally archive log files.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->checkpoint
	 *
	 * @param session the session handle
	 * @configstart
	 * @config{archive,,remove log files no longer required for
	 * 	transactional durabilty}
	 * @config{force,,write a new checkpoint even if nothing
	 * 	has changed since the last one}
	 * @config{flush_cache,,flush the cache; default on}
	 * @config{flush_log,,flush the log; default on}
	 * @config{log_size,,only proceed if more than the specified
	 * 	amount of log records have been written since the last
	 * 	checkpoint}
	 * @config{timeout,,only proceed if more than the specified
	 * 	number of milliseconds have elapsed since the last checkpoint}
	 * @configend
	 * @errors
	 */
	int __F(checkpoint)(WT_SESSION *session, const char *config);
	/*! @} */
};

/*!
 * A connection to a WiredTiger database.  The connection may be opened within
 * the same address space as the caller or accessed over a socket connection.
 *
 * Most applications will open a single connection to a database for each
 * process.  The first process to open a connection to a database will access
 * the database in its own address space.  Subsequent connections (if allowed)
 * will communicate with the first process over a socket connection to perform
 * their operations.
 */
struct WT_CONNECTION {
	/*! Load an extension.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->load_extension
	 *
	 * @param connection the connection handle
	 * @param path the filename of the extension module
	 * @configstart
	 * @config{entry,,the entry point of the
	 * extension; default \c "wiredtiger_extension_init"}
	 * @config{prefix,,a prefix for all names registered by this
	 * extension (e.g.\, to make namespaces distinct or during upgrades}
	 * @configend
	 * @errors
	 */
	int __F(load_extension)(WT_CONNECTION *connection,
	    const char *path, const char *config);

	/*! Add a new type of cursor.
	 *
	 * @dontinclude ex_all.c
	 *
	 * First the application has to implement the WT_CURSOR_FACTORY interface
	 * as follows:
	 * @skip Implementation of WT_CURSOR_FACTORY
	 * @until End implementation
	 *
	 * Then the implementation is registered with WiredTiger as follows:
	 * @skip WT_CURSOR_FACTORY
	 * @until add_cursor_factory
	 *
	 * @param connection the connection handle
	 * @param prefix the prefix for location strings passed to
	 * WT_SESSION::open_cursor
	 * @param factory the application-supplied code to manage cursors of
	 * this type
	 * @configempty
	 * @errors
	 */
	int __F(add_cursor_factory)(WT_CONNECTION *connection,
	    const char *prefix, WT_CURSOR_FACTORY *factory, const char *config);

	/*! Add a custom collation function.
	 *
	 * @dontinclude ex_all.c
	 *
	 * First the application has to implement the WT_COLLATOR interface
	 * as follows:
	 * @skip Implementation of WT_COLLATOR
	 * @until End implementation
	 *
	 * Then the implementation is registered with WiredTiger as follows:
	 * @skip WT_COLLATOR
	 * @until add_collator
	 *
	 * @param connection the connection handle
	 * @param name the name of the collation to be used in calls to
	 * 	WT_SESSION::create_table
	 * @param collator the application-supplied collation handler
	 * @configempty
	 * @errors
	 */
	int __F(add_collator)(WT_CONNECTION *connection,
	    const char *name, WT_COLLATOR *collator, const char *config);

	/*! Add a custom extractor for index keys or column sets.
	 *
	 * @dontinclude ex_all.c
	 *
	 * First the application has to implement the WT_EXTRACTOR interface
	 * as follows:
	 * @skip Implementation of WT_EXTRACTOR
	 * @until End implementation
	 *
	 * Then the implementation is registered with WiredTiger as follows:
	 * @skip WT_EXTRACTOR
	 * @until add_extractor
	 *
	 * @param connection the connection handle
	 * @param name the name of the extractor to be used in calls to
	 * 	WT_SESSION::create_table
	 * @param extractor the application-supplied extractor
	 * @configempty
	 * @errors
	 */
	int __F(add_extractor)(WT_CONNECTION *connection, const char *name,
	    WT_EXTRACTOR *extractor, const char *config);


	/*! Close a connection.
	 *
	 * Any open sessions will be closed.
	 *
	 * @dontinclude ex_all.c
	 * @skipline conn->close
	 *
	 * @param connection the connection handle
	 * @configempty
	 * @errors
	 */
	int __F(close)(WT_CONNECTION *connection, const char *config);

	/*! The home directory of the connection.
	 *
	 * @dontinclude ex_all.c
	 * @skipline ->get_home
	 *
	 * @param connection the connection handle
	 * @returns a pointer to a string naming the home directory
	 */
	const char *__F(get_home)(WT_CONNECTION *connection);

	/*! Did opening this handle create the database?
	 *
	 * @dontinclude ex_all.c
	 * @skip ->is_new
	 * @until }
	 *
	 * @param connection the connection handle
	 * @returns false (zero) if the connection existed before the call to
	 *    ::wiredtiger_open, true (non-zero) if it was created by opening
	 *    this handle.
	 */
	int __F(is_new)(WT_CONNECTION *connection);

	/*! Open a session.
	 *
	 * @dontinclude ex_all.c
	 * @skip session;
	 * @until open_session
	 *
	 * @param connection the connection handle
	 * @param errhandler An error handler.  If <code>NULL</code>, the
	 * connection's error handler is used
	 * @configempty
	 * @param sessionp the new session handle
	 * @errors
	 */
	int __F(open_session)(WT_CONNECTION *connection,
	    WT_ERROR_HANDLER *errhandler, const char *config,
	    WT_SESSION **sessionp);
};

/*! Open a connection to a database.
 *
 * @dontinclude ex_all.c
 * @skip conn;
 * @until wiredtiger_open
 *
 * @param home The path to the database home directory
 * @param errhandler An error handler.  If <code>NULL</code>, a builtin error
 * handler is installed that writes error messages to stderr
 * @configstart
 * @config{create,,create the database if it does not exist}
 * @config{exclusive,,fail if the database already exists}
 * @config{error_prefix,,Prefix string for error messages}
 * @config{multiprocess,,permit sharing between processes (will
 * automatically start an RPC server for primary processes and use RPC for
 * secondary processes)}
 * @config{cachesize,,maximum heap memory to allocate for the cache;
 * 	default \c "10MB"}
 * @config{max_threads,,maximum expected number of threads (including
 * 	server threads)}
 * @configend
 * @param connectionp A pointer to the newly opened connection handle
 * @errors
 */
int wiredtiger_open(const char *home,
    WT_ERROR_HANDLER *errhandler, const char *config,
    WT_CONNECTION **connectionp);

/*! Get information about an error as a string.
 *
 * @dontinclude ex_all.c
 * @skipline wiredtiger_strerror
 *
 * @param err a return value from a WiredTiger call
 * @returns a string representation of the error
 */
const char *wiredtiger_strerror(int err);

/*! Pack a structure into a buffer.
 *
 * Uses format strings mostly as specified in the Python struct module:
 *   http://docs.python.org/library/struct
 *
 * The first character of the format string can be used to indicate the byte
 * order, size and alignment of the packed data, according to the following
 * table:
 * 
 * <table>
 * <tr><th>Character</th><th>Byte order</th><th>Size</th><th>Alignment</th></tr>
 * <tr><td><tt>\@</tt></td><td>native</td><td>native</td><td>native</td></tr>
 * <tr><td><tt>=</tt></td><td>native</td><td>standard</td><td>none</td></tr>
 * <tr><td><tt>&lt;</tt></td><td>little-endian</td><td>standard</td><td>none</td></tr>
 * <tr><td><tt>&gt;</tt></td><td>big-endian</td><td>standard</td><td>none</td></tr>
 * <tr><td><tt>!</tt></td><td>network (= big-endian)</td><td>standard</td><td>none</td></tr>
 * </table>
 *
 * If the first character is not one of these, '>' (big-endian) is assumed, in
 * part because it naturally sorts in lexicographic order.
 *
 * Format characters:
 * <table>
<tr><th>Format</th><th>C Type</th><th>Java type</th><th>Python type</th><th>Standard size</th></tr>
<tr><td>x</td><td>pad byte</td><td>N/A</td><td>N/A</td><td>1</td></tr>
<tr><td>c</td><td>char</td><td>char</td><td>string of length 1</td><td>1</td></tr>
<tr><td>b</td><td>signed char</td><td>byte</td><td>integer</td><td>1</td></tr>
<tr><td>B</td><td>unsigned char</td><td>byte</td><td>integer</td><td>1</td></tr>
<tr><td>?</td><td>_Bool</td><td>boolean</td><td>bool</td><td>1</td></tr>
<tr><td>h</td><td>short</td><td>short</td><td>integer</td><td>2</td></tr>
<tr><td>H</td><td>unsigned short</td><td>short</td><td>integer</td><td>2</td></tr>
<tr><td>i</td><td>int</td><td>int</td><td>integer</td><td>4</td></tr>
<tr><td>I</td><td>unsigned int</td><td>int</td><td>integer</td><td>4</td></tr>
<tr><td>l</td><td>long</td><td>int</td><td>integer</td><td>4</td></tr>
<tr><td>L</td><td>unsigned long</td><td>int</td><td>integer</td><td>4</td></tr>
<tr><td>q</td><td>long long</td><td>long</td><td>integer</td><td>8</td></tr>
<tr><td>Q</td><td>unsigned long long</td><td>long</td><td>integer</td><td>8</td></tr>
<tr><td>f</td><td>float</td><td>float</td><td>float</td><td>4</td></tr>
<tr><td>d</td><td>double</td><td>double</td><td>float</td><td>8</td></tr>
<tr><td>r</td><td>::wiredtiger_recno_t</td><td>long</td><td>integer</td><td>8</td></tr>
<tr><td>s</td><td>char[]</td><td>String</td><td>string</td><td>fixed length</td></tr>
<tr><td>S</td><td>char[]</td><td>String</td><td>string</td><td>variable</td></tr>
<tr><td>u</td><td>WT_ITEM</td><td>byte[]</td><td>string</td><td>variable</td></tr>
 * </table>
 *
 * The <code>'S'</code> type is encoded as a C language string terminated by a
 * NUL character.
 *
 * The <code>'u'</code> type is for raw byte arrays: if it appears at the end
 * of a format string (including in the default <code>"u"</code> format for
 * untyped tables), the size is not stored explicitly.  When <code>'u'</code>
 * appears within a format string, the size is stored as a 32-bit integer in
 * the same byte order as the rest of the format string, followed by the data.
 *
 * @section pack_examples Packing Examples
 *
 * For example, the string <code>"iSh"</code> will pack a 32-bit integer
 * followed by a NUL-terminated string, followed by a 16-bit integer.  The
 * default, big-endian encoding will be used, with no alignment.  This could
 * be used in C as follows:
 *
 * @dontinclude ex_all.c
 * @skip char buf
 * @until wiredtiger_struct_pack
 *
 * Then later, the values can be unpacked as follows:
 *
 * @until wiredtiger_struct_unpack
 *
 * @param buffer a pointer to a packed byte array
 * @param size the number of valid bytes in the buffer
 * @param format the data format, see ::wiredtiger_struct_pack
 * @errors
 */
int wiredtiger_struct_pack(void *buffer, size_t size, const char *format, ...);

/*! Pack a structure into a buffer.
 *
 * stdarg version of ::wiredtiger_struct_pack.
 *
 * @param buffer a pointer to a packed byte array
 * @param size the number of valid bytes in the buffer
 * @param format the data format, see ::wiredtiger_struct_pack
 * @param ap the list of values to pack
 * @errors
 */
int wiredtiger_struct_packv(void *buffer, size_t size,
    const char *format, va_list ap);

/*! Calculate the size required to pack a structure.
 *
 * Note that for variable-sized fields including variable-sized strings and
 * integers, the calculated sized merely reflects the expected sizes specified
 * in the format string itself.
 *
 * @dontinclude ex_all.c
 * @skip size_t size;
 * @until assert
 *
 * @param format the data format, see ::wiredtiger_struct_pack
 * @returns the number of bytes needed for the matching call to
 * ::wiredtiger_struct_pack
 */
size_t wiredtiger_struct_size(const char *format, ...);

/*! Calculate the size required to pack a structure.
 *
 * stdarg version of ::wiredtiger_struct_size.
 *
 * @param format the data format, see ::wiredtiger_struct_pack
 * @param ap the list of values to be packed
 * @returns the number of bytes needed for the matching call to
 * ::wiredtiger_struct_pack
 */
size_t wiredtiger_struct_sizev(const char *format, va_list ap);

/*! Unpack a structure from a buffer.
 *
 * Reverse of ::wiredtiger_struct_pack: gets values out of a packed byte string.
 *
 * @dontinclude ex_all.c
 * @skip int i;
 * @until wiredtiger_struct_unpack
 *
 * @param buffer a pointer to a packed byte array
 * @param size the number of valid bytes in the buffer
 * @param format the data format, see ::wiredtiger_struct_pack
 * @errors
 */
int wiredtiger_struct_unpack(const void *buffer, size_t size,
    const char *format, ...);

/*! Unpack a structure from a buffer.
 *
 * stdarg version of ::wiredtiger_struct_unpack.
 *
 * @param buffer a pointer to a packed byte array
 * @param size the number of valid bytes in the buffer
 * @param format the data format, see ::wiredtiger_struct_pack
 * @param ap the list of locations where values are unpacked
 * @errors
 */
int wiredtiger_struct_unpackv(const void *buffer, size_t size,
    const char *format, va_list ap);

/*! Get version information.
 *
 * @dontinclude ex_all.c
 * @skip int major
 * @until wiredtiger_version
 *
 * @param majorp a location where the major version number is returned
 * @param minorp a location where the minor version number is returned
 * @param patchp a location where the patch version number is returned
 * @returns a string representation of the version
 */
const char *wiredtiger_version(int *majorp, int *minorp, int *patchp);

/*! Concurrent operations caused a deadlock, a transaction must rollback. */
#define	WT_DEADLOCK	(-10000)

/*! No matching record was found, including when reaching the limits of a
 * cursor traversal.
 */
#define	WT_NOTFOUND	(-10001)

/*! Concurrent operations caused a conflict, a transaction must rollback. */
#define	WT_UPDATE_CONFLICT	(-10002)

#ifdef __cplusplus
}
#endif

/*! @} */

#endif /* _WIREDTIGER_H_ */
