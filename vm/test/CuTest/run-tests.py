import os
import sys
import datetime







def main():
  root = get_root_or_cry()
  archive_existing_log_files(root)
  run_tests(root)












def get_root_or_cry():
  if os.environ.has_key('MINIAT_ROOT'):
    root = os.environ['MINIAT_ROOT']
  else:
    sys.exit('Environment variable "MINIAT_ROOT" not found. Please set this variable and run the tests again.')
  return root











def archive_existing_log_files(root=''):

  # Get the path names and check to see if there's anything to archive.
  names = ['assertion.log', 'suite.log', 'unittest.log']
  paths = [ os.path.join(root, name) for name in names ]
  logs_exist = any(os.path.exists(log) for log in paths)
  if not logs_exist: return

  # Create the archive directory if it doesn't exist.
  archive = os.path.join(root, 'test-output')
  if not os.path.exists(archive): os.makedirs(archive)

  # Archive all the things. 
  for log in paths:
    base_name = os.path.basename(log)
    change_time = os.path.getctime(log) 
    date_time = datetime.datetime.fromtimestamp(change_time)
    date = str(date_time.date())
    time = str(date_time.time())
    date_dir = os.path.join(archive, date)
    time_dir = os.path.join(date_dir, time)
    for dir in [date_dir, time_dir]:
      if not os.path.exists(dir): os.makedirs(dir)
    
    os.rename(log, os.path.join(time_dir, base_name))










'''
  Currently, this code just calls the C executable to run the tests and write to the files. 

  Ideally though, the C code could be imported as a module. 
  This code could then just call tests.run(), or something analogous to that which could
  return a python value representing the test results. 

  The python code could then handle all the IO.

  Check it out:  https://docs.python.org/2/extending/extending.html

  The issue is that the compilation/linking of the C program needs to be incorporated into scons
  and I currently suck at scons.  
'''
def run_tests(root=''):
  os.chdir(root)
  exe = os.path.join(root, 'out/exe/miniat_tests')
  os.system(exe)











main()