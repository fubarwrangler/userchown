General User Chown & Copy
=========================

A program that allows the a specified user to copy files as any other user
belonging to a specified group to a specific set of directories.

Design
------
A setuid binary is required, so we may as well write in C as it isn't that
large a program. Will be packaged as an RPM and installed on PHENIX nodes.

Reads a configuration file to determine the allowed user, target-user's group,
and target locations.

Limitations
-----------
The program will become the target user to copy files, so the inputs must be
readable by that user (likely world-readable). The target directories must be
enumerated in the config file and must be underneath this. They also cannot
contain any back-references "../" or symbolic links that point outside of
this target directory.

Exit Codes
----------

The following possible error codes provided by the program all have special
meanings documented below:


Error Code--Meaning
0) NO_ERROR
	Everything was successful
1) USAGE_ERROR
	Commandline-usage error
2) CONFIG_ERROR
	Error reading or parsing config file, or missing config section
3) USERPERM_ERROR
	User is not authorized in config, target user is in the incorrect group,
	or the user does not have permission to suid to the target.
4) USERABSENT_ERROR
	Target user or the target required-group does not exist
5) PATHPERM_ERROR
	Destination target is not in the list of allowed destination directories
6) FILEPERM_ERROR
	Error opening input to read or creating output file
7) PATHRESOLV_ERROR
	Error fully resolving the output path or any of the configured
	allowed-paths
8) QUOTA_ERROR
9) IO_ERROR

Abnormal Errors
128) MEMORY_ERROR
	A memory allocation failed somewhere
129) LDAP_ERROR
	A lookup of user or group info failed (library call fail)
130) INTERNAL_ERROR
	Something strange that should not happen did
