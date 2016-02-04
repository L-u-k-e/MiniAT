
DBFILE="miniat.db"
SQLITE3="sqlite3"

if test -z `which sqlite3`; then
	echo;echo "sqlite3 not found, install and ensure it is in PATH!";echo;
	exit 2;
fi

if test -f $DBFILE; then
	echo;echo "$DBFILE exists, you must remove it first";echo;
	exit 2;
fi;

touch $DBFILE;

$SQLITE3 --line $DBFILE "

CREATE TABLE suite(
	run_id INTEGER PRIMARY KEY AUTOINCREMENT,
	date_time TIMESTAMP UNIQUE NOT NULL,
	passed INT NOT NULL,
	failed INT NOT NULL,
	total INT NOT NULL,
	percentage_passed int NOT NULL
);

CREATE TABLE tests(
	run_id INT NOT NULL,
	test_id TEXT NOT NULL,
	passed INT NOT NULL,
	failed INT NOT NULL,
	total INT NOT NULL,
	percentage_passed INT NOT NULL
);

CREATE TABLE assertions(
	run_id INT NOT NULL,
	test_id TEXT NOT NULL,
	file_id INT NOT NULL,
	line_num INT NOT NULL,
	message TEXT NOT NULL,
	pass_fail BOOLEAN NOT NULL
);

CREATE TABLE test_name(
	test_id INTEGER PRIMARY KEY AUTOINCREMENT,
	name TEXT UNIQUE NOT NULL	
);

CREATE TABLE file_name(
file_id INTEGER PRIMARY KEY AUTOINCREMENT,
name TEXT UNIQUE NOT NULL
);
"
exit 0;	
