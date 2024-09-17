## MGWS - Support files
### Linux system configuration files

When running MGWS on Linux (specifically Ubuntu/Debian), you may want to run it as a daemon.

Support Files:
  * init.d/mgws - minimal init.d script for manipulating the daemon process
    * Copy to /etc/init.d
    * Needs work: the script is not ready for production use yet: (refers to my development build & certs)
  * 10-mgws.conf - rsyslogd config script to redirect mgws log entries to separate file
    * Copy to /etc/rsyslog.d
    * restart the rsyslogd daemon once copied
