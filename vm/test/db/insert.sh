#!/bin/bash

ASSERTION_LOG="assertion.log"
SUITE_LOG="suite.log"
TEST_LOG="unittest.log"

DELIM=\t
#Since this is run from the context of the tests directory, you must go down to find tmp
LOG_DIR=./tmp
DB_DIR=./db
DB_FILE=miniat.db

ASSERTIONPASSFAILPOS=1;
ASSERTIONTESTPOS=2;
ASSERTIONFILEPOS=3;
ASSERTIONLINEPOS=4;
ASSERTIONMSGPOS=5;

SUITEPASSPOS=2;
SUITEFAILPOS=3;
SUITECNTPOS=4;
SUITEPERPOS=5;

TESTNAMEPOS=2
TESTPASSPOS=3
TESTFAILPOS=4
TESTCNTPOS=5
TESTPERPOS=6

#Test and make sure that all files and directories exist befor continuing...
if ! test -d $LOG_DIR; then
	echo; echo "Log directory: $LOG_DIR does not exist, nothing to be processed.";
	echo "Exiting...";echo;
	exit 0;
fi;

if ! test -f $LOG_DIR/$ASSERTION_LOG; then
	echo; echo "Log: $LOG_DIR/$ASSERTION_LOG does not exist, nothing to be processed.";
	echo "Exiting...";echo;
	exit 0;
fi;

if ! test -f $LOG_DIR/$SUITE_LOG; then
	echo; echo "Log: $LOG_DIR/$SUITE_LOG does not exist, nothing to be processed.";
	echo "Exiting...";echo;
	exit 0;
fi;

if ! test -f $LOG_DIR/$TEST_LOG; then
	echo; echo "Log: $LOG_DIR/$TEST_LOG does not exist, nothing to be processed.";
	echo "Exiting...";echo;
	exit 0;
fi;

#
# Generate the list of files and test suite names for the database
#
file_list=`cat $LOG_DIR/$ASSERTION_LOG | cut -f $ASSERTIONFILEPOS | uniq`;
test_list=`cat $LOG_DIR/$ASSERTION_LOG | cut -f $ASSERTIONTESTPOS | uniq`;

#
# Get the data for the assertion table
#

IFS="
";

declare -a assertionfile

i=0;
echo;echo -n "Reading file: $LOG_DIR/$ASSERTION_LOG";
while read line   
do   
	assertionfile[i]=$line
	i=`expr $i + 1`;
	echo -n -n ".";	   
done < $LOG_DIR/$ASSERTION_LOG 
echo;

if test ${#assertionfile[@]} -ne `wc -l $LOG_DIR/$ASSERTION_LOG | cut -d " " -f -1`; then
	echo;	
	echo "Assertion array length: ${#assertionfile[@]} does not equal file line count: `wc -l $LOG_DIR/$ASSERTION_LOG`";
	echo "exiting";
	echo;
	exit 2;
fi

#
# Get the data for the test table
#

declare -a testfile

i=0;
echo;echo -n "Reading file: $LOG_DIR/$TEST_LOG";
while read line   
do   
	testfile[i]=$line
	i=`expr $i + 1`;
	echo -n ".";	   
done < $LOG_DIR/$TEST_LOG 

if test ${#testfile[@]} -ne `wc -l $LOG_DIR/$TEST_LOG | cut -d " " -f -1`; then
	echo;	
	echo "Test array length: ${#testfile[@]} does not equal file line count: `wc -l $LOG_DIR/$TEST_LOG`";
	echo "exiting";
	echo;
	exit 2;
fi

#
# Get the data for the suite table
#

declare -a suitefile

i=0;
echo;echo -n "Reading file: $LOG_DIR/$SUITE_LOG";
while read line   
do   
	suitefile[i]=$line
	i=`expr $i + 1`;
	echo -n ".";   
done < $LOG_DIR/$SUITE_LOG 

if test ${#suitefile[@]} -ne `wc -l $LOG_DIR/$SUITE_LOG | cut -d " " -f -1`; then
	echo;	
	echo "Suite array length: ${#suitefile[@]} does not equal file line count: `wc -l $LOG_DIR/$SUITE_LOG`";
	echo "exiting";
	echo;
	exit 2;
fi

######################DONE SANITY CHECKING##############################

while test 1 -eq 1 ; do
	echo;echo -n "Data passed sanity check, would you like to continue (yes or no) [ENTER]: ";
	read choice;
		if test $choice == "yes"; then
			echo "Here goes nothing....";	
			break;
		elif test $choice == "no"; then
			echo "Exiting.....";
			exit 2;
		else
			echo "$choice is not yes or no, try again!";		
		fi
done;
echo;
echo "Processing please be patient!";


#
# Insert file list into the file table
#
IFS="
";
for i in $file_list; do 
	sqlite3 -line $DB_DIR/$DB_FILE "INSERT INTO file_name (name) VALUES (\"$i\");" &> /dev/null;
	#echo "$i";
	echo -n ".";
done;


#
# Insert test names into the file table
#
IFS="
";
for i in $test_list; do 
	sqlite3 -line $DB_DIR/$DB_FILE "INSERT INTO test_name (name) VALUES (\"$i\");"  &> /dev/null;
	echo -n ".";
	#echo "$i";
done;

#
# Get a run id
#
run_id=`sqlite3 -line $DB_DIR/$DB_FILE "SELECT MAX(run_id) FROM suite;" | awk '{print $3}'`;
if test -z run_id; then
	run_id=1;
else 

	run_id=`expr $run_id + 1`;

fi

#echo "run id: $run_id";

######################Suite Table#######################################
for (( i = 0 ; i < ${#suitefile[@]} ; i++ ))
do
	tmp=`echo ${suitefile[$i]} | cut -f 2- | sed "s/\	/,/g"`;
	#echo "INSERT INTO suite (date_time, passed, failed, total, percentage_passed) VALUES (datetime(),$tmp);";
	sqlite3 -line $DB_DIR/$DB_FILE "INSERT INTO suite (date_time, passed, failed, total, percentage_passed) VALUES (datetime(),$tmp);"
	echo -n ".";
done;

#################TEST TABLE#############################################

for (( i = 0 ; i < ${#testfile[@]} ; i++ ))
do
	testname=`echo ${testfile[$i]} | cut -f 2-2`;
	#echo "test name: $testname";	
	test_id=`sqlite3 -line $DB_DIR/$DB_FILE "SELECT test_id FROM test_name where name=\"$testname\";" | awk '{print $3}'`;
	#echo "test id: $test_id";
	tmp=`echo ${testfile[$i]} | cut -f 3-6 | sed "s/\	/,/g"`;
	#echo "INSERT INTO tests (run_id, test_id, passed, failed, total, percentage_passed) VALUES ($run_id, $test_id, $tmp);";
	sqlite3 -line $DB_DIR/$DB_FILE "INSERT INTO tests (run_id, test_id, passed, failed, total, percentage_passed) VALUES ($run_id, $test_id, $tmp);";
	echo -n ".";
done;

########################ASSERTION TABLE ################################
for (( i = 0 ; i < ${#assertionfile[@]} ; i++ ))
do
	testname=`echo ${assertionfile[$i]} | cut -f 2-2`;
	#echo "test name: $testname";	

	test_id=`sqlite3 -line $DB_DIR/$DB_FILE "SELECT test_id FROM test_name where name=\"$testname\";" | awk '{print $3}'`;
	#echo "test id: $test_id";
	
	filename=`echo ${assertionfile[$i]} | cut -f 3-3`;
	#echo "filename: $filename";
	
	file_id=`sqlite3 -line $DB_DIR/$DB_FILE "SELECT file_id FROM file_name where name=\"$filename\";" | awk '{print $3}'`;
	#echo "file id: $file_id";

	line=`echo ${assertionfile[$i]} | cut -f 4-4`;
	msg=`echo ${assertionfile[$i]} | cut -f 5-5`;

	passstring=`echo ${assertionfile[$i]} | cut -f 1-1`;	
	#echo "passstring: $passstring";

	if test $passstring == "Passed"; then
		pass=1;
	else
		pass=0;
	fi
	
	#echo "INSERT INTO assertions (run_id, test_id, file_id, line_num, message, pass_fail) VALUES ($run_id, $test_id, $file_id, $line, \"$msg\", $pass);";
	sqlite3 -line $DB_DIR/$DB_FILE "INSERT INTO assertions (run_id, test_id, file_id, line_num, message, pass_fail) VALUES ($run_id, $test_id, $file_id, $line, \"$msg\", $pass);";
	echo -n ".";
done;

echo;
echo "Finished....exiting";

exit 0;
